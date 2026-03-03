#include "Engine_pch.h"
#include "PhysicsCollider.h"
#include "GameInstance.h"

#include "GameObject.h"

#include "PhysicsRigidBody.h"

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
	, m_tDesc(rhs.m_tDesc)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	auto srcShapes = rhs.m_pColliderShapes;
	DeepCopy_Shapes(srcShapes);
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

	m_tDesc.eFilterLayer = pDesc->eFilterLayer;
	m_tDesc.iFilterMask = pDesc->iFilterMask;
	if(m_tDesc.strAttackPresetTag.empty() == false)
		m_tDesc.iAttackPresetID = m_pGameInstance->Get_AttackPresetIdByTag(m_tDesc.strAttackPresetTag);

	if (!pDesc->bSetOnlyFilter)
	{
		vector<PxShape*> shapes = m_pGameInstance->GetShape(pDesc);

		for (auto& shape : shapes)
			m_pColliderShapes.push_back(shape);
	}

	SetCollisionFilter();

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

CPhysicsCollider* CPhysicsCollider::SetTransform(_uint iIndex, const Matrix& matWorld)
{
	PxTransform ptf = m_pGameInstance->XMMatrixToPxTransform(matWorld);

	m_pColliderShapes[iIndex]->setLocalPose(ptf);

	UpdateActor();

	return this;
}

CPhysicsCollider* CPhysicsCollider::SetTransform(const Matrix& matWorld)
{
	PxTransform ptf = m_pGameInstance->XMMatrixToPxTransform(matWorld);

	for (auto* shape : m_pColliderShapes)
		shape->setLocalPose(ptf);

	UpdateActor();

	return this;
}

CPhysicsCollider* CPhysicsCollider::SetCenter(Vec3 vCenter)
{
	for (auto* shape : m_pColliderShapes)
		shape->setLocalPose(PxTransform(PxVec3(vCenter.x, vCenter.y, vCenter.z)));

	UpdateActor();

	return this;
}

CPhysicsCollider* CPhysicsCollider::SetCenter(_uint iIndex, Vec3 vCenter)
{
	m_pColliderShapes[iIndex]->setLocalPose(PxTransform(PxVec3(vCenter.x, vCenter.y, vCenter.z)));

	UpdateActor();

	return this;
}

CPhysicsCollider* CPhysicsCollider::Rotation(_uint iIndex, Quat vQuat)
{
	m_pColliderShapes[iIndex]->setLocalPose(PxTransform(PxQuat(vQuat.x, vQuat.y, vQuat.z, vQuat.w)));

	UpdateActor();

	return this;
}

CPhysicsCollider* CPhysicsCollider::Rotation(Quat vQuat)
{
	PxQuat pq(vQuat.x, vQuat.y, vQuat.z, vQuat.w);

	for (auto* shape : m_pColliderShapes)
		shape->setLocalPose(PxTransform(pq));

	UpdateActor();

	return this;
}

void CPhysicsCollider::SetCollisionFilter()
{
	PxFilterData filterData(m_tDesc.eFilterLayer, m_tDesc.iFilterMask, 0, 0);
	for (auto& shape : m_pColliderShapes)
	{
		shape->setSimulationFilterData(filterData);
		shape->setQueryFilterData(filterData);
	}
}

void CPhysicsCollider::UpdateActor()
{
	for (auto& shape : m_pColliderShapes)
	{
		PxActor* actor = shape->getActor();
		_float density = (static_cast<CGameObject*>(actor->userData))->Get_Component<CPhysicsRigidBody>()->GetDesc()->fDensity;

		if (actor && actor->getType() != PxActorType::eRIGID_STATIC)
			PxRigidBodyExt::updateMassAndInertia(*static_cast<PxRigidDynamic*>(actor), density);
	}
}

void CPhysicsCollider::DeepCopy_Shapes(vector<PxShape*>& shapes)
{
	for (auto& shape : m_pColliderShapes)
		PX_RELEASE(shape);

	m_pColliderShapes.clear();

	m_pColliderShapes = m_pGameInstance->CopyShapes(shapes);
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
	for (auto& shape : m_pColliderShapes)
	{
		if (shape != nullptr && shape->isReleasable())
		{
			shape->userData = nullptr;
			PxRigidActor* actor = shape->getActor();
			if (actor)
				actor->detachShape(*shape);

			PX_RELEASE(shape);
		}
	}

	m_pColliderShapes.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Super::Free();
}