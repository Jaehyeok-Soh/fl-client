#include "Engine_pch.h"
#include "ColliderModule.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CColliderModule::CColliderModule(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CColliderModule::CColliderModule(const CColliderModule& rhs)
	: Super(rhs)
{
}

HRESULT CColliderModule::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CColliderModule::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg == nullptr)
		return E_FAIL;

	COLLIDERMODULE_COPY_DESC* pDesc = static_cast<COLLIDERMODULE_COPY_DESC*>(pArg);

	if (pDesc->pOwner == nullptr)
		return E_FAIL;

	m_pOwner = pDesc->pOwner;
	if (FAILED(Add_Component<CPhysicsCollider>(0, L"Prototype_Component_Physics_Collider", pDesc->pPhysicsColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", pDesc->pPhysicsRigidbodyDesc)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
	return S_OK;
}

void CColliderModule::Enable(CGameObject* pOwner)
{
	m_pOwner = pOwner;
	CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
	if (pRigidBody)
		pRigidBody->EnableCollision(true);
}

void CColliderModule::Disable()
{
	m_pOwner = nullptr;
	CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>();
	if (pRigidBody)
		pRigidBody->EnableCollision(false);
}

void CColliderModule::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>();
	CTransform* pTransform = Get_Component<CTransform>();
	pTransform->Set_WorldMatrix(pTransform->Get_WorldMatrix() * pOwnerTransform->Get_WorldMatrix());
}

void CColliderModule::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

void CColliderModule::OnTrigger_Enter(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	m_pOwner->OnTrigger_Enter(iMyLayer, iOtherLayer, pOther, tHitInfo);
}

void CColliderModule::OnTrigger_Exit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther)
{
	m_pOwner->OnTrigger_Exit(iMyLayer, iOtherLayer, pOther);
}

HRESULT CColliderModule::Awake(_uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
	return S_OK;
}

CColliderModule* CColliderModule::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CColliderModule* pInstance = new CColliderModule(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CColliderModule::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CColliderModule::Clone(void* pArg)
{
	CColliderModule* pInstance = new CColliderModule(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CColliderModule::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CColliderModule::Free()
{
	Super::Free();
}
