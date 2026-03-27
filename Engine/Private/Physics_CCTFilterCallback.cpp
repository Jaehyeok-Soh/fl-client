#include "Engine_pch.h"

#include "Physics_CCTFilterCallback.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"

#include "GameObject.h"

bool CPhysics_CCTFilterCallback::filter(const PxController& a, const PxController& b)
{
	PxRigidDynamic* actorA = a.getActor();
	PxRigidDynamic* actorB = b.getActor();
	if (!actorA || !actorB)
		return false;

	PxShape* shapeA;
	PxShape* shapeB;
	actorA->getShapes(&shapeA, 1);
	actorB->getShapes(&shapeB, 1);

	PxFilterData filterA = shapeA->getQueryFilterData();
	PxFilterData filterB = shapeB->getQueryFilterData();

	if ((filterA.word0 & PHYSICSFILTERGROUP::GENIEMON) ||
		(filterB.word0 & PHYSICSFILTERGROUP::GENIEMON))
	{
		return false;
	}

	if (a.getUserData() == nullptr || b.getUserData() == nullptr)
		return false;

	CGameObject* pObjA = static_cast<CGameObject*>(a.getUserData());
	CGameObject* pObjB = static_cast<CGameObject*>(b.getUserData());

	if (!pObjA || !pObjB)
		return false;

	_bool aIsAlive = pObjA->IsAlive();
	_bool bIsAlive = pObjB->IsAlive();

	if (!aIsAlive || !bIsAlive)
		return false;

	if (pObjA->Get_Component<CPhysicsCollider>()->GetDesc()->eFilterLayer & PHYSICSFILTERGROUP::GENIEMON
		|| pObjB->Get_Component<CPhysicsCollider>()->GetDesc()->eFilterLayer & PHYSICSFILTERGROUP::GENIEMON)
	{
		return false;
	}

	return true;
}

CPhysics_CCTFilterCallback::CPhysics_CCTFilterCallback()
{
}

HRESULT CPhysics_CCTFilterCallback::Initialize()
{
	return S_OK;
}

CPhysics_CCTFilterCallback* CPhysics_CCTFilterCallback::Create()
{
	CPhysics_CCTFilterCallback* pInstance = new CPhysics_CCTFilterCallback();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_CCTFilterCallback");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_CCTFilterCallback::Free()
{
	Super::Free();
}