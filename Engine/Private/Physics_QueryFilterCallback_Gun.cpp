#include "Engine_pch.h"

#include "Physics_QueryFilterCallback_Gun.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "GameObject.h"

PxQueryHitType::Enum CPhysics_QueryFilterCallback_Gun::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	if (shape == nullptr || actor == nullptr)
		return PxQueryHitType::eNONE;

	if (actor->userData == nullptr)
		return PxQueryHitType::eNONE;

	if (actor->userData == m_pOwner)
		return PxQueryHitType::eNONE;

	CGameObject* pTarget = static_cast<CGameObject*>(actor->userData);
	if (pTarget && pTarget->IsDead())
		return PxQueryHitType::eNONE;

	PxFilterData shapeFilter = shape->getQueryFilterData();

	if (shapeFilter.word0 & PHYSICSFILTERGROUP::MAP)
		return PxQueryHitType::eNONE;

	if (filterData.word1 & shapeFilter.word0)
		return PxQueryHitType::eBLOCK;

	return PxQueryHitType::eNONE;
}

CPhysics_QueryFilterCallback_Gun::CPhysics_QueryFilterCallback_Gun()
	: Super()
{
}

HRESULT CPhysics_QueryFilterCallback_Gun::Initialize()
{
	return S_OK;
}

void CPhysics_QueryFilterCallback_Gun::SetOwner(CGameObject* pOwner)
{
	m_pOwner = pOwner;
}

CPhysics_QueryFilterCallback_Gun* CPhysics_QueryFilterCallback_Gun::Create()
{
	CPhysics_QueryFilterCallback_Gun* pInstance = new CPhysics_QueryFilterCallback_Gun();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_QueryFilterCallback_Gun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_QueryFilterCallback_Gun::Free()
{
	Super::Free();
}

PxQueryHitType::Enum CPhysics_QueryFilterCallback_Gun::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
{
	PX_UNUSED(filterData);
	PX_UNUSED(hit);
	PX_UNUSED(shape);
	PX_UNUSED(actor);

	return PxQueryHitType::eTOUCH;
}