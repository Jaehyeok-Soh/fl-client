#include "pch.h"
#include "TriggerCollidePart.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CTriggerCollidePart::CTriggerCollidePart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CTriggerCollidePart::CTriggerCollidePart(const CTriggerCollidePart& rhs)
	: Super(rhs)
	, m_vCollidedPos(rhs.m_vCollidedPos)
	, m_iCollidedID(rhs.m_iCollidedID)
	, m_FUpdate_Flags(rhs.m_FUpdate_Flags)
	, m_bDeadObj(rhs.m_bDeadObj)
{
}

HRESULT CTriggerCollidePart::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriggerCollidePart::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TRIGGER_COLLIDEPART_DESC* pDesc = static_cast<TRIGGER_COLLIDEPART_DESC*>(pArg);
	m_FUpdate_Flags = pDesc->FUpdate_Flags;

	if (pDesc->pMatSocket)
	{
		m_eState = EState::WithBone;
		m_pMatSocket = pDesc->pMatSocket;
		m_matPreScale = pDesc->vPreScale;
	}
	else
	{
		m_eState = EState::OnlyOwner;
	}

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

	auto actor = Get_Component<CPhysicsRigidBody>()->GetActor(0);
	PxShape* pShape = { nullptr };
	actor->getShapes(&pShape, 1);

	pShape->setLocalPose(PxTransform(PxVec3(0.f), PxQuat(0.f, 0.f, PxHalfPi, 1.f)));

	return S_OK;
}

HRESULT CTriggerCollidePart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
	return S_OK;
}

HRESULT CTriggerCollidePart::Clear_WhenChangeLevel()
{
	m_pCollidedObj = nullptr;
	return S_OK;
}

void CTriggerCollidePart::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTriggerCollidePart::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	switch (m_eState)
	{
	case Client::CTriggerCollidePart::None:
		break;
	case Client::CTriggerCollidePart::WithBone:
		Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));
		break;
	case Client::CTriggerCollidePart::OnlyOwner:
		Super::Update_CombinedWorldMatrix((*m_pMatParent));
		break;
	}

	Get_Component<CPhysicsRigidBody>()->Move(
		Vec3{ m_matCombinedWorld._41, m_matCombinedWorld._42,m_matCombinedWorld._43 },
		fTimeDelta);
}

void CTriggerCollidePart::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTriggerCollidePart::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	// render 전에 캐싱 유효 검사
	if (m_pCollidedObj && !(m_pCollidedObj->IsAlive()))
	{
		m_pCollidedObj = nullptr;
		//Safe_Release(m_pCollidedObj);
	}

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

void CTriggerCollidePart::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if(Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Call_ParentTirggerEnter)))
		Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

	if (pOther && pOther->IsAlive())
	{
		if (Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Check_CollidedPos_Enter)))
		{
			CTransform* pTrans = pOther->Get_Component<CTransform>();

			if (pTrans)
			{
				m_vCollidedPos = pTrans->Get_Info(TRANSFORM_INFO_STATE::POS);
				m_iCollidedID = static_cast<_int>(pOther->Get_ID());
			}
		}


		if (Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Check_CollidedObj_Enter)))
		{
			if (m_pCollidedObj != pOther)
			{
				if (m_pCollidedObj)
				{
					//Safe_Release(m_pCollidedObj);
				}

				m_pCollidedObj = pOther;
				//Safe_AddRef(m_pCollidedObj);
			}
		}
	}
}

void CTriggerCollidePart::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Call_ParentTirggerExit)))
		Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

	// 충돌 position 리셋
	if (Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Check_CollidedPos_Exit)) &&
		pOther && pOther->Get_ID() == m_iCollidedID)
		m_vCollidedPos = Vec3::Zero;


	if (Engine_Utils::Has_Flag(m_FUpdate_Flags, ENUM_TO_UINT(UPDATEFLAGS::Check_CollidedObj_Exit)) &&
		m_pCollidedObj == pOther
		)
	{
		m_pCollidedObj = nullptr;
		//Safe_Release(m_pCollidedObj);
	}
}

HRESULT CTriggerCollidePart::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

const Vec3& CTriggerCollidePart::Get_Collided_ObjPos()
{
	// render 전에 캐싱 유효 검사
	if (m_pCollidedObj)
	{
		if((m_pCollidedObj->IsAlive()))
			return m_pCollidedObj->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	}

	return Vec3::Zero;
}

HRESULT CTriggerCollidePart::Ready_Components(TRIGGER_COLLIDEPART_DESC* pDesc)
{
	if (FAILED(Add_Component<CPhysicsCollider>(0, L"Prototype_Component_Physics_Collider", pDesc->pColliderDesc)))
		return E_FAIL;

	// 만약 외부에서 지정 하지 않았다면
	// 이거의 transform을 따라가도록	설정
	if (pDesc->pRigidbodyDesc->pOwnerMatrix == nullptr)
	{
		pDesc->pRigidbodyDesc->pOwnerMatrix = Get_Component<CTransform>()->Get_WorldMatrixPtr();
	}
	if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", pDesc->pRigidbodyDesc)))
		return E_FAIL;

	return S_OK;
}

CTriggerCollidePart* CTriggerCollidePart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTriggerCollidePart* pInstance = new CTriggerCollidePart(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CTriggerCollidePart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTriggerCollidePart::Clone(void* pArg)
{
	CTriggerCollidePart* pInstance = new CTriggerCollidePart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CTriggerCollidePart::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTriggerCollidePart::Free()
{
	Super::Free();

	//if (m_pCollidedObj)
	//	Safe_Release(m_pCollidedObj);
}

