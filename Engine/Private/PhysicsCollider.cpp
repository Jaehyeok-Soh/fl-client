#include "Engine_pch.h"
#include "PhysicsCollider.h"
#include "GameInstance.h"

CPhysicsCollider::CPhysicsCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CPhysicsCollider::CPhysicsCollider(const CPhysicsCollider& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pColliderShapes(rhs.m_pColliderShapes)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CPhysicsCollider::Initialize_Prototype(void* pArg)
{
	if (pArg != nullptr)
	{
		PHYSICSCOLLIDER_DESC* pDesc = static_cast<PHYSICSCOLLIDER_DESC*>(pArg);
		m_tDesc = *pDesc;

		m_pColliderShapes = m_pGameInstance->GetMeshShape(&m_tDesc);
	}

	return S_OK;
}

HRESULT CPhysicsCollider::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	PHYSICSCOLLIDER_DESC* pDesc = static_cast<PHYSICSCOLLIDER_DESC*>(pArg);
	m_tDesc.bIsTrigger = pDesc->bIsTrigger;
	m_tDesc.vCenter = pDesc->vCenter;
	m_tDesc.fRadius = pDesc->fRadius;
	m_tDesc.fHeight = pDesc->fHeight;
	m_tDesc.vExtents = pDesc->vExtents;
	m_tDesc.vXAxis = pDesc->vXAxis;

	m_tDesc.eShape = pDesc->eShape;

	m_tDesc.eConvexShape = pDesc->eConvexShape;
	m_tDesc.fMargin = pDesc->fMargin;
	m_tDesc.tMaterial = pDesc->tMaterial;
	m_tDesc.bIsActive = pDesc->bIsActive;

	vector<PxShape*> shapes = m_pGameInstance->GetShape(pDesc);

	for (auto& shape : shapes)
		m_pColliderShapes.push_back(shape);

	return S_OK;
}

void CPhysicsCollider::Awake()
{
}

void CPhysicsCollider::Update(const Matrix& matWorld)
{
}

void CPhysicsCollider::OnCollision(PxRigidActor* _pOther)
{
}

void CPhysicsCollider::OnCollisionEnter(PxRigidActor* _pOther)
{
}

void CPhysicsCollider::OnCollisionExit(PxRigidActor* _pOther)
{
}

void CPhysicsCollider::SetCenter(Vec3 vCenter)
{
	for (auto* shape : m_pColliderShapes)
		shape->setLocalPose(PxTransform(PxVec3(vCenter.x, vCenter.y, vCenter.z)));
}

#ifdef _DEBUG
void CPhysicsCollider::Render()
{
	return;
}
#endif

CPhysicsCollider* CPhysicsCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CPhysicsCollider* pInstance = new CPhysicsCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CPhysicsCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsCollider::Clone(void* pArg)
{
	CPhysicsCollider* pInstance = new CPhysicsCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsCollider::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Super::Free();
}
