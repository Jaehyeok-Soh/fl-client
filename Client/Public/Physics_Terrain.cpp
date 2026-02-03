#include "pch.h"
#include "Client_Defines.h"
#include "Physics_Terrain.h"
#include "GameInstance.h"

#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

CPhysics_Terrain::CPhysics_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CPhysics_Terrain::CPhysics_Terrain(const CPhysics_Terrain& rhs)
	: Super(rhs)
{
}

HRESULT CPhysics_Terrain::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Terrain::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Terrain::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

	return S_OK;
}

void CPhysics_Terrain::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPhysics_Terrain::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CPhysics_Terrain::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CPhysics_Terrain::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif // _DEBUG
}

HRESULT CPhysics_Terrain::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Terrain::Ready_Components()
{
	if (FAILED(Ready_Physics()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPhysics_Terrain::Ready_Physics()
{
	if (FAILED(Ready_PhysicsCollider()))
		return E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Terrain::Ready_PhysicsCollider()
{
	PHYSICSCOLLIDER_DESC desc{};
	desc.bIsTrigger = false;
	desc.vCenter = { 0.f, -0.5f, 0.f };
	desc.fRadius = 0.f;
	desc.fHeight = 0.f;
	desc.vExtents = { 100.f, 1.f, 100.f };
	desc.vXAxis = { 0.f, 1.f, 0.f };
	desc.eShape = EPhysicsShape::BOX;
	desc.eConvexShape = EPhysicsConvexShape::END;
	desc.fMargin = 0.f;
	desc.tMaterial = {};
	desc.bIsActive = true;

	if (FAILED(Add_Component<CPhysicsCollider>(0/*static*/, L"Prototype_Component_Physics_Collider", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysics_Terrain::Ready_PhysicsRigidBody()
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

CPhysics_Terrain* CPhysics_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPhysics_Terrain* pInstance = new CPhysics_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CPhysics_Terrain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPhysics_Terrain::Clone(void* pArg)
{
	CPhysics_Terrain* pInstance = new CPhysics_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CPhysics_Terrain::Clone, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_Terrain::Free()
{
	Super::Free();
}
