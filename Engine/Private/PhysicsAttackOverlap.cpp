#include "Engine_pch.h"
#include "PhysicsAttackOverlap.h"
#include "GameInstance.h"

#include "GameObject.h"
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
	m_tDesc = *static_cast<ATTACKOVERLAP_DESC*>(pArg);
	
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
	PoolClear();

	CActiveAttackOverlap* poolingItem = { nullptr };
	for (size_t i = 0; i < m_tDesc.iNumPool; i++)
	{
		poolingItem = CActiveAttackOverlap::Create();
		m_eventPool.push(poolingItem);
	}

	m_pOwnerMatrix = & static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();

	m_pFilterCallback = m_pGameInstance->GetQueryFilterCallback();
	m_pFilterCallback->SetOwner(Get_Owner());

	GetAnimation();

	Ready_OverlapInfo();
}

void CPhysicsAttackOverlap::Update(_float fTimeDelta)
{
	CheckAnim();

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

void CPhysicsAttackOverlap::GetAnimation()
{
	m_pOwnerModel = Get_Owner()->Get_Component<CModel>();
	Safe_AddRef(m_pOwnerModel);
}

void CPhysicsAttackOverlap::CheckAnim()
{
	_int curAnimIndex = m_pOwnerModel->Get_CurrentAnimationIndex();
	_float curAnimTrackPosition = m_pOwnerModel->Get_AnimTrackPosition();

	if (m_iPrevAnimIndex != curAnimIndex)
	{
		m_iPrevAnimIndex = curAnimIndex;
		m_fPrevTrackPosition = 0.f;
	}

	for (auto& attackDesc : m_tDesc.attackEvents)
	{
		if (attackDesc.fAnimIndex == curAnimIndex)
		{
			if (m_fPrevTrackPosition <= attackDesc.fStartTrackPosition
				&& curAnimTrackPosition >= attackDesc.fStartTrackPosition)
			{
				if (m_eventPool.empty())
					continue;

				if (attackDesc.tHitboxDesc.geometry.getType() == -1)
					continue;

				auto& event = m_eventPool.front();
				m_eventPool.pop();
				
				event->Set(&attackDesc.tHitboxDesc, *m_pOwnerMatrix, m_pOwner);
				m_activeEvents.push_back(event);
			}
		}
	}

	m_fPrevTrackPosition = curAnimTrackPosition;
}

void CPhysicsAttackOverlap::Ready_OverlapInfo()
{
	for (auto& event : m_tDesc.attackEvents)
	{
		PHYSICSCOLLIDER_DESC desc;

		switch (event.tHitboxDesc.eType)
		{
		case CPhysicsAttackOverlap::Enum::BOX:
		{
			desc.eShape = EPhysicsShape::BOX;
			desc.vExtents = event.tHitboxDesc.vExtents;
		}
			break;
		case CPhysicsAttackOverlap::Enum::SPHERE:
		{
			desc.eShape = EPhysicsShape::SPHERE;
			desc.fRadius = event.tHitboxDesc.fRadius;
		}
			break;
		default:
			continue;
		}
		
		vector<PxShape*> shapes = m_pGameInstance->GetShape(&desc);
		if (shapes.size() > 0)
			event.tHitboxDesc.geometry = shapes.front()->getGeometry();
		else
			continue;
		
		for (auto& shape : shapes)
			PX_RELEASE(shape);

		event.tHitboxDesc.filterData.data.word0 = event.tHitboxDesc.eFilterLayer;
		event.tHitboxDesc.filterData.data.word1 = event.tHitboxDesc.iFilterMask;
		event.tHitboxDesc.filterData.flags = PxQueryFlag::ePREFILTER | PxQueryFlag::eDYNAMIC | PxQueryFlag::eNO_BLOCK;
		event.tHitboxDesc.matOffset = Matrix::CreateTranslation(event.tHitboxDesc.vOffset);

		event.tHitboxDesc.filterCallback = m_pFilterCallback;
	}
}

void CPhysicsAttackOverlap::PoolClear()
{
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
	for (auto& event : m_activeEvents)
		Safe_Release(event);

	m_activeEvents.clear();

	PoolClear();

	Safe_Release(m_pOwnerModel);
	Safe_Release(m_pFilterCallback);

	Super::Free();
}
