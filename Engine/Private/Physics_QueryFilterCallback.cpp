#include "Engine_pch.h"

#include "Physics_QueryFilterCallback.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "GameObject.h"

PxQueryHitType::Enum CPhysics_QueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	if (actor->userData == m_pOwner)
		return PxQueryHitType::eNONE;

	CGameObject* pTarget = static_cast<CGameObject*>(actor->userData);
	if (pTarget && pTarget->IsDead())
		return PxQueryHitType::eNONE;

	return PxQueryHitType::eTOUCH;
}

CPhysics_QueryFilterCallback::CPhysics_QueryFilterCallback()
	: Super()
{
}

HRESULT CPhysics_QueryFilterCallback::Initialize()
{
	return S_OK;
}

void CPhysics_QueryFilterCallback::SetOwner(CGameObject* pOwner)
{
	m_pOwner = pOwner;
}

CPhysics_QueryFilterCallback* CPhysics_QueryFilterCallback::Create()
{
	CPhysics_QueryFilterCallback* pInstance = new CPhysics_QueryFilterCallback();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_QueryFilterCallback");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_QueryFilterCallback::Free()
{
	Super::Free();
}

PxQueryHitType::Enum CPhysics_QueryFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor)
{
	PX_UNUSED(filterData);
	PX_UNUSED(hit);
	PX_UNUSED(shape);
	PX_UNUSED(actor);

	return PxQueryHitType::eTOUCH;
}
