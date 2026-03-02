#include "Engine_pch.h"

#include "Physics_CCTBehaviorCallback.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"

PxControllerBehaviorFlags CPhysics_CCTBehaviorCallback::getBehaviorFlags(const PxController& controller)
{
	//PxControllerStats cctStats;
	//controller.getStats(cctStats);

	//PxControllerState cctState;
	//controller.getState(cctState);

	return PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CPhysics_CCTBehaviorCallback::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	PxFilterData filterData = shape.getSimulationFilterData();

	if (filterData.word0 & (PHYSICSFILTERGROUP::PLAYER | PHYSICSFILTERGROUP::MONSTER))
	{
		return PxControllerBehaviorFlags(0);
	}

	return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT | PxControllerBehaviorFlag::eCCT_SLIDE;
}

PxControllerBehaviorFlags CPhysics_CCTBehaviorCallback::getBehaviorFlags(const PxObstacle& obstacle)
{
	PX_UNUSED(obstacle);

	return PxControllerBehaviorFlags();
}

CPhysics_CCTBehaviorCallback::CPhysics_CCTBehaviorCallback()
{
}

HRESULT CPhysics_CCTBehaviorCallback::Initialize()
{
	return S_OK;
}

CPhysics_CCTBehaviorCallback* CPhysics_CCTBehaviorCallback::Create()
{
	CPhysics_CCTBehaviorCallback* pInstance = new CPhysics_CCTBehaviorCallback();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_CCTBehaviorCallback");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_CCTBehaviorCallback::Free()
{
	Super::Free();
}
