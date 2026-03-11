#include "Engine_pch.h"

#include "Physics_QueryFilterCallback_SpringArm.h"

#include "GameInstance.h"

#include "GameObject.h"

PxQueryHitType::Enum CPhysics_QueryFilterCallback_SpringArm::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	if (shape == nullptr || actor == nullptr)
		return PxQueryHitType::eNONE;

	if (actor->userData == nullptr)
		return PxQueryHitType::eNONE;

	if (actor->userData == m_pOwner)
		return PxQueryHitType::eNONE;

	CGameObject* pTarget = static_cast<CGameObject*>(actor->userData);
	if (pTarget && !pTarget->IsAlive())
		return PxQueryHitType::eNONE;

	PxFilterData shapeFilter = shape->getQueryFilterData();

	_bool isMap = (shapeFilter.word0 & PHYSICSFILTERGROUP::MAP) != 0;

	if (!isMap)
		return PxQueryHitType::eNONE;

	if (isMap)
		return PxQueryHitType::eBLOCK;

	return PxQueryHitType::eNONE;
}

CPhysics_QueryFilterCallback_SpringArm::CPhysics_QueryFilterCallback_SpringArm()
	: Super()
{
}

HRESULT CPhysics_QueryFilterCallback_SpringArm::Initialize()
{
	return S_OK;
}

void CPhysics_QueryFilterCallback_SpringArm::SetOwner(CGameObject* pOwner)
{
	m_pOwner = pOwner;
}

CPhysics_QueryFilterCallback_SpringArm* CPhysics_QueryFilterCallback_SpringArm::Create()
{
	CPhysics_QueryFilterCallback_SpringArm* pInstance = new CPhysics_QueryFilterCallback_SpringArm();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_QueryFilterCallback_SpringArm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_QueryFilterCallback_SpringArm::Free()
{
	Super::Free();
}

PxQueryHitType::Enum CPhysics_QueryFilterCallback_SpringArm::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
{
	PX_UNUSED(filterData);
	PX_UNUSED(hit);
	PX_UNUSED(shape);
	PX_UNUSED(actor);

	return PxQueryHitType::eTOUCH;
}
