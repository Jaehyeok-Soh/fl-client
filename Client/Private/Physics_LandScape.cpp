#include "pch.h"
#include "Client_Defines.h"
#include "Physics_LandScape.h"
#include "GameInstance.h"

#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

CPhysics_LandScape::CPhysics_LandScape(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CPhysics_LandScape::CPhysics_LandScape(const CPhysics_LandScape& rhs)
	: Super(rhs)
{
}

HRESULT CPhysics_LandScape::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Initialize(void* pArg)
{
	PXLANDSCAPE_DESC* pDesc = static_cast<PXLANDSCAPE_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

	return S_OK;
}

void CPhysics_LandScape::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPhysics_LandScape::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CPhysics_LandScape::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CPhysics_LandScape::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif // _DEBUG
}

HRESULT CPhysics_LandScape::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Ready_Components(PXLANDSCAPE_DESC* desc)
{
	if (FAILED(Ready_Physics(desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Ready_Physics(PXLANDSCAPE_DESC* desc)
{
	if (FAILED(Ready_PhysicsCollider(desc->wstrColliderPrototypeName)))
		return E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Ready_PhysicsCollider(wstring prototypeTag)
{
	if (FAILED(Add_Component<CPhysicsCollider>(0/*static*/, prototypeTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_LandScape::Ready_PhysicsRigidBody()
{
	PHYSICSRIGIDBODY_DESC desc{};
	desc.eType = EPhysicsActorType::STATIC;
	desc.detection = EPhysicsCollisionDetection::DISCRETE;
	desc.fDensity = 10.f;
	desc.bUseGravity = false;
	desc.bIsKinematic = false;
	desc.fLinearDamping = 0.f;
	desc.fAngularDamping = 0.f;
	desc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();

	if (FAILED(Add_Component<CPhysicsRigidBody>(0/*static*/, L"Prototype_Component_Physics_RigidBody", &desc)))
		return E_FAIL;

	return S_OK;
}

CPhysics_LandScape* CPhysics_LandScape::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPhysics_LandScape* pInstance = new CPhysics_LandScape(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CPhysics_Terrain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPhysics_LandScape::Clone(void* pArg)
{
	CPhysics_LandScape* pInstance = new CPhysics_LandScape(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPhysics_LandScape::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_LandScape::Free()
{
	Super::Free();
}
