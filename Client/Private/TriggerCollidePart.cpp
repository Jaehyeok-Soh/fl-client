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
	, m_vColliedPos(rhs.m_vColliedPos)
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
	return S_OK;
}

HRESULT CTriggerCollidePart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
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
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

void CTriggerCollidePart::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

	if (pOther)
	{
		CTransform* pTrans = pOther->Get_Component<CTransform>();

		if (pTrans)
		{
			m_vColliedPos = pTrans->Get_Info(TRANSFORM_INFO_STATE::POS);
		}
	}
}

void CTriggerCollidePart::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CTriggerCollidePart::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
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
}

