#include "Engine_pch.h"
#include "PhysicsAttackOverlap.h"
#include "GameInstance.h"

#include "GameObject.h"
#include "ContainerObject.h"
#include "PartObject.h"

#include "ActiveAttackOverlap.h"

#include "Model.h"

CPhysicsAttackOverlap::CPhysicsAttackOverlap()
	: Super()
{
}

CPhysicsAttackOverlap::CPhysicsAttackOverlap(const CPhysicsAttackOverlap& rhs)
	: Super(rhs),
	m_tDesc(rhs.m_tDesc)
{
}

HRESULT CPhysicsAttackOverlap::Initialize_Prototype(void* pArg)
{
	m_tDesc = *static_cast<DTO::ATTACKOVERLAP_DESC*>(pArg);
	
	return S_OK;
}

HRESULT CPhysicsAttackOverlap::Initialize(void* pArg)
{
	return S_OK;
}

#ifdef _DEBUG
void CPhysicsAttackOverlap::Render()
{
	for (auto& event : m_activeEvents)
		event->Render();
}
#endif

void CPhysicsAttackOverlap::Awake()
{
	Safe_Release(m_pFilterCallback);

	PoolClear();

	if (m_pOwner == nullptr)
		return;
	
	CActiveAttackOverlap* poolingItem = { nullptr };
	for (size_t i = 0; i < m_tDesc.iNumPool; i++)
	{
		poolingItem = CActiveAttackOverlap::Create();
		m_eventPool.push(poolingItem);
	}

	auto partObject = dynamic_cast<CPartObject*>(Get_Owner());
	if (partObject != nullptr)
		m_pOwnerMatrix = &partObject->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();
	else
		m_pOwnerMatrix = &Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrix();

	m_pFilterCallback = m_pGameInstance->GetQueryFilterCallback();
	m_pFilterCallback->SetOwner(Get_Owner());

	GetAnimation();

	Ready_OverlapInfo();
}

void CPhysicsAttackOverlap::Update(_float fTimeDelta)
{
	vector<_uint> finishedIndex;
	_uint curIndex = {};

	vector<CActiveAttackOverlap*>::iterator iter = m_activeEvents.begin();

	while (iter != m_activeEvents.end())
	{
		if ((*iter)->GetState() == CActiveAttackOverlap::Enum::FIN)
		{
			(*iter)->Reset();
			m_eventPool.push(*iter);
			iter = m_activeEvents.erase(iter);
		}
		else
		{
			(*iter)->Update(fTimeDelta);
			iter++;
		}
	}
}

void CPhysicsAttackOverlap::Ready_Event()
{
	Release_Event();

	m_EventHandle = m_pOwnerModel->OnNotify.Subscribe(
		[this](const AnimNotifyKey& key)
		{
			this->CallbackEvent(key);
		});
}

void CPhysicsAttackOverlap::Release_Event()
{
	if (m_pOwner)
	{
		if (m_pOwnerModel)
			m_pOwnerModel->OnNotify.Unsubscribe(m_EventHandle);
	}
		
}

void CPhysicsAttackOverlap::CallbackEvent(const AnimNotifyKey& key)
{
	//struct AnimNotifyKey
	//{
	//	EAnimNotifyId eID{ EAnimNotifyId::Hitbox };
	//	float fTrackPosition{ 0.f };

	//	unsigned int  iParam0{ 0 }; // 이벤트 인덱스
	//	unsigned int  iParam1{ 0 }; // 애니메이션 인덱스
	//	unsigned int  iParam2{ 0 };
	//	unsigned int  iParam3{ 0 };
	//	float		  fParam0{ 0.0f };
	//	float		  fParam1{ 0.0f };
	//	bool		  bParam0{ false };
	//	bool		  bParam1{ false };
	//	string		  strParam{ "" };
	//};

	if (key.eID != EAnimNotifyId::Hitbox)
		return;

	if (m_tDesc.attackEvents.size() == 0)
		return;

	DTO::ATTACKEVENT& event = m_tDesc.attackEvents[key.iParam0];
	if (event.iAnimIndex != key.iParam1)
		return;

	if (m_eventPool.empty())
		return;

	if (event.tHitboxDesc.geometry.getType() == -1)
		return;

	auto& a = event.tHitboxDesc.geometry.any();

	auto eventInstance = m_eventPool.front();
	m_eventPool.pop();

	eventInstance->Set(&event.tHitboxDesc, *m_pOwnerMatrix, m_pOwner);
	m_activeEvents.push_back(eventInstance);
}

void CPhysicsAttackOverlap::Modify_AttackOverlap(_uint eventIdx, DTO::ATTACKEVENT event)
{
	PHYSICSCOLLIDER_DESC desc;

	switch (event.tHitboxDesc.eType)
	{
	case EOverlapType::Enum::BOX:
	{
		desc.eShape = EPhysicsShape::BOX;
		desc.vExtents = event.tHitboxDesc.vExtents;
	}
	break;
	case EOverlapType::Enum::SPHERE:
	{
		desc.eShape = EPhysicsShape::SPHERE;
		desc.fRadius = event.tHitboxDesc.fRadius;
	}
	break;
	case EOverlapType::Enum::CAPSULE:
	{
		desc.eShape = EPhysicsShape::CAPSULE;
		desc.fRadius = event.tHitboxDesc.fRadius;
		desc.fHeight = event.tHitboxDesc.fHeight;
	}
	break;
	default:
		return;
	}

	vector<PxShape*> shapes = m_pGameInstance->GetShape(&desc);
	if (shapes.size() > 0)
		event.tHitboxDesc.geometry = shapes.front()->getGeometry();
	else
		return;

	for (auto& shape : shapes)
		PX_RELEASE(shape);

	event.tHitboxDesc.filterData.data.word0 = event.tHitboxDesc.eFilterLayer;
	event.tHitboxDesc.filterData.data.word1 = event.tHitboxDesc.iFilterMask;
	event.tHitboxDesc.filterData.flags = PxQueryFlag::ePREFILTER | PxQueryFlag::eDYNAMIC | PxQueryFlag::eNO_BLOCK;
	event.tHitboxDesc.matOffset = Matrix::CreateTranslation(event.tHitboxDesc.vOffset);

	event.tHitboxDesc.filterCallback = m_pFilterCallback;

	auto& animations = m_pOwnerModel->Get_Animations();
	for (auto& anim : animations)
		anim->Clear_Notifies();

	m_tDesc.attackEvents[eventIdx] = event;
	Ready_OverlapInfo();
}

void CPhysicsAttackOverlap::Modify_AttackOverlap(vector<DTO::ATTACKEVENT> events)
{
	auto& animations = m_pOwnerModel->Get_Animations();
	for (auto& anim : animations)
		anim->Clear_Notifies();

	m_tDesc.attackEvents = events;
	Ready_OverlapInfo();
}

void CPhysicsAttackOverlap::GetAnimation()
{
	auto partObject = dynamic_cast<CPartObject*>(Get_Owner());
	if (partObject != nullptr)
	{
		m_pOwnerModel = partObject->Get_Component<CModel>();
	}
	else
	{
		CContainerObject* containerObj = static_cast<CContainerObject*>(Get_Owner());
		if (containerObj->Get_PartSize() > 0)
			m_pOwnerModel = containerObj->Get_Part<CPartObject>(0)->Get_Component<CModel>();
		else
			m_pOwnerModel = containerObj->Get_Component<CModel>();
	}
}

void CPhysicsAttackOverlap::Ready_OverlapInfo()
{
	Ready_Event();

	string animTag;
	wstring wAnimTag;

	auto& animations = m_pOwnerModel->Get_Animations();
	auto findTag = [&animTag](CModelAnimation* anim) {
		return anim->Get_Name() == Engine_Utils::ToWString(animTag);
		};

	auto wFindTag = [&wAnimTag](CModelAnimation* anim) {
		return anim->Get_Name() == wAnimTag;
		};

	_uint eventIdx = { 0 };
	for (auto& event : m_tDesc.attackEvents)
	{
		PHYSICSCOLLIDER_DESC desc;

		switch (event.tHitboxDesc.eType)
		{
		case EOverlapType::Enum::BOX:
		{
			desc.eShape = EPhysicsShape::BOX;
			desc.vExtents = event.tHitboxDesc.vExtents;
		}
			break;
		case EOverlapType::Enum::SPHERE:
		{
			desc.eShape = EPhysicsShape::SPHERE;
			desc.fRadius = event.tHitboxDesc.fRadius;
		}
		break;
		case EOverlapType::Enum::CAPSULE:
		{
			desc.eShape = EPhysicsShape::CAPSULE;
			desc.fRadius = event.tHitboxDesc.fRadius;
			desc.fHeight = event.tHitboxDesc.fHeight;
		}
		break;
		default:
		{
			eventIdx++;
			continue;
		}
		}
		
		vector<PxShape*> shapes = m_pGameInstance->GetShape(&desc);
		if (shapes.size() > 0)
			event.tHitboxDesc.geometry = shapes.front()->getGeometry();
		else
		{
			eventIdx++;
			continue;
		}
		
		for (auto& shape : shapes)
			PX_RELEASE(shape);

		event.tHitboxDesc.filterData.data.word0 = event.tHitboxDesc.eFilterLayer;
		event.tHitboxDesc.filterData.data.word1 = event.tHitboxDesc.iFilterMask;
		event.tHitboxDesc.filterData.flags = PxQueryFlag::ePREFILTER | PxQueryFlag::eDYNAMIC | PxQueryFlag::eNO_BLOCK;
		event.tHitboxDesc.matOffset = Matrix::CreateTranslation(event.tHitboxDesc.vOffset);

		event.tHitboxDesc.filterCallback = m_pFilterCallback;

		animTag = event.strAnimTag;
		auto animIter = std::find_if(animations.begin(), animations.end(), findTag);
		
		_int animIdx = m_pOwnerModel->Get_AnimationIndex(Engine_Utils::ToWString(animTag));

		// 재할당
		if (animIdx != -1)
			event.iAnimIndex = animIdx;
		else
		{
			wAnimTag = m_pOwnerModel->Get_AnimationName(event.iAnimIndex);
			animIter = std::find_if(animations.begin(), animations.end(), wFindTag);
			event.iAnimIndex = m_pOwnerModel->Get_AnimationIndex(wAnimTag);
		}

		AnimNotifyKey key{};
		key.fTrackPosition = event.fStartTrackPosition;
		key.iParam0 = eventIdx;
		key.iParam1 = event.iAnimIndex;
		(*animIter)->Pushback_Notifies(event.ePhase, key);
		eventIdx++;
		(*animIter)->Sort_Notifies();
	}
}

void CPhysicsAttackOverlap::PoolClear()
{
	for (auto& event : m_activeEvents)
		Safe_Release(event);

	m_activeEvents.clear();

	while (!m_eventPool.empty())
	{
		Safe_Release(m_eventPool.front());
		m_eventPool.pop();
	}
}

CPhysicsAttackOverlap* CPhysicsAttackOverlap::Create(void* pArg)
{
	CPhysicsAttackOverlap* pInstance = new CPhysicsAttackOverlap();

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysicsAttackOverlap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsAttackOverlap::Clone(void* pArg)
{
	CPhysicsAttackOverlap* pInstance = new CPhysicsAttackOverlap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsAttackOverlap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsAttackOverlap::Free()
{
	Release_Event();

	PoolClear();

	Safe_Release(m_pFilterCallback);

	Super::Free();
}
