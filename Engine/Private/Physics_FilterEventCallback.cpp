#include "Engine_pch.h"

#include "Physics_FilterEventCallback.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"

void CPhysics_FilterEventCallback::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	CGameObject* leftObject = Conversion_GameObject(pairHeader.actors[0]->userData);
	CGameObject* rightObject = Conversion_GameObject(pairHeader.actors[1]->userData);

	if (leftObject == nullptr || rightObject == nullptr || leftObject->IsDead() || rightObject->IsDead())
		return;

	GAMEOBJECTINFO info = Get_GameObject(pairHeader.actors[0]->userData, pairHeader.actors[1]->userData);

	for (PxU32 i = 0; i < nbPairs; i++)
	{
		if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// HitPoint
			PxContactPairPoint points[4];
			const PxU32 N = pairs[i].extractContacts(points, PX_ARRAY_SIZE(points));

			if (N > 0)
			{
				info.bHasHitPoint = true;
				// 추후에 BestPoint를 검출하려면 연산이 필요함
				::memcpy(&info.vHitPoint.x, &points[0].position.x, sizeof(Vec3));
				::memcpy(&info.vRawNormal.x, &points[0].normal.x, sizeof(Vec3));
				const _float fSep = points[0].separation;
				info.fDepth = (std::max)(0.0f, -fSep);
			}
			else
			{
				info.bHasHitPoint = false;
				info.fDepth = 0.0f;
			}

			// On collision enter
			OnCollisionEnter(info);
		}

		else if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			// On collision
			OnCollision(info);
		}

		else if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			// On collision exit
			OnCollisionExit(info);
		}
	}
}

void CPhysics_FilterEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
		CGameObject* leftObject = Conversion_GameObject(pairs[i].triggerActor->userData);
		CGameObject* rightObject = Conversion_GameObject(pairs[i].otherActor->userData);

		if (leftObject == nullptr || rightObject == nullptr ||
			leftObject->IsDead() || rightObject->IsDead())
			return;

		GAMEOBJECTINFO info = Get_GameObject(pairs[i].triggerActor->userData, pairs[i].otherActor->userData);

		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			// On trigger enter
			m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_TRIGGER_ENTER](info);
		}

		else if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			// On trigger exit
			m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_TRIGGER_EXIT](info);
		}
	}
}

void CPhysics_FilterEventCallback::ProcessOverlap(CGameObject* pOwner, const PxVec3& vOverlapPoint, PxOverlapHit* pOverlapHit, PxPairFlag::Enum event, DTO::HITBOX_DESC* hitboxDesc)
{
	GAMEOBJECTINFO info = Get_GameObject(pOwner, Conversion_GameObject(pOverlapHit->actor->userData));

	info.leftColliderDesc.eFilterLayer = hitboxDesc->eFilterLayer;
	info.leftColliderDesc.iFilterMask = hitboxDesc->iFilterMask;
	info.leftColliderDesc.iAttackPresetID = hitboxDesc->iAttackPresetID;

	if (event & PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
		PxVec3 closetPoint{};
		PxU32 closetIndex{};

		PxTransform shapeGlobalPose = pOverlapHit->actor->getGlobalPose() * pOverlapHit->shape->getLocalPose();

		float dist = PxGeometryQuery::pointDistance(vOverlapPoint,
			pOverlapHit->shape->getGeometry(),
			shapeGlobalPose,
			&closetPoint,
			&closetIndex);

		PxVec3 hitPoint;
		PxVec3 normal;

		{
			if (dist > 1e-6f)
			{
				hitPoint = closetPoint;
				normal = vOverlapPoint - closetPoint;
				normal.normalize();
			}
			else
			{
				PxVec3 shapeCenter = pOverlapHit->actor->getGlobalPose().p;
				hitPoint = vOverlapPoint;
				normal = vOverlapPoint - shapeGlobalPose.p;

				if (normal.magnitudeSquared() < 1e-6f)
					normal = PxVec3(0.f, 1.f, 0.f);
				else
					normal.normalize();
			}
		}

		info.bHasHitPoint = true;
		// 추후에 BestPoint를 검출하려면 연산이 필요함
		::memcpy(&info.vHitPoint.x, &hitPoint.x, sizeof(Vec3));
		::memcpy(&info.vRawNormal.x, &normal.x, sizeof(Vec3));
		info.fDepth = (std::max)(0.0f, -dist);

		// On collision enter
		OnCollisionEnter(info);
	}

	else if (event & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
	{
		// On collision
		OnCollision(info);
	}

	else if (event & PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
		// On collision exit
		OnCollisionExit(info);
	}
}

void CPhysics_FilterEventCallback::OnCollisionEnter(GAMEOBJECTINFO& info)
{
	// On collision enter
	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_ENTER](info);
}

void CPhysics_FilterEventCallback::OnCollision(GAMEOBJECTINFO& info)
{
	// On collision
	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_STAY](info);
}

void CPhysics_FilterEventCallback::OnCollisionExit(GAMEOBJECTINFO& info)
{
	// On collision exit
	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_EXIT](info);
}


CPhysics_FilterEventCallback::CPhysics_FilterEventCallback()
{
}

HRESULT CPhysics_FilterEventCallback::Initialize()
{
	Ready_EventCallChain();

#ifdef _DEBUG
	m_arrEventString[COLLISIONEVENT::Enum::ON_COLLISION_ENTER] = L"[On collision enter]\n";
	m_arrEventString[COLLISIONEVENT::Enum::ON_COLLISION_STAY] = L"[On collision stay]\n";
	m_arrEventString[COLLISIONEVENT::Enum::ON_COLLISION_EXIT] = L"[On collision exit]\n";
	m_arrEventString[COLLISIONEVENT::Enum::ON_TRIGGER_ENTER] = L"[On trigger enter]\n";
	m_arrEventString[COLLISIONEVENT::Enum::ON_TRIGGER_EXIT] = L"[On trigger exit]\n";
#endif // _DEBUG

	return S_OK;
}

CGameObject* CPhysics_FilterEventCallback::Conversion_GameObject(void* userData)
{
	return static_cast<CGameObject*>(userData);
}

CPhysics_FilterEventCallback::GAMEOBJECTINFO CPhysics_FilterEventCallback::Get_GameObject(void* leftArgs, void* rightArgs)
{
	CGameObject* leftObject = Conversion_GameObject(leftArgs);
	PHYSICSCOLLIDER_DESC leftColliderDesc{};
	if (leftObject)
		leftColliderDesc = *leftObject->Get_Component<CPhysicsCollider>()->GetDesc();

	CGameObject* rightObject = Conversion_GameObject(rightArgs);
	PHYSICSCOLLIDER_DESC rightColliderDesc{};
	if (rightObject)
		rightColliderDesc = *rightObject->Get_Component<CPhysicsCollider>()->GetDesc();

	return GAMEOBJECTINFO(leftObject, leftColliderDesc, rightObject, rightColliderDesc);
}

CPhysics_FilterEventCallback::GAMEOBJECTINFO CPhysics_FilterEventCallback::Get_GameObject(CGameObject* leftObj, CGameObject* rightObj)
{
	PHYSICSCOLLIDER_DESC leftColliderDesc{};
	if (leftObj)
		leftColliderDesc = *leftObj->Get_Component<CPhysicsCollider>()->GetDesc();

	PHYSICSCOLLIDER_DESC rightColliderDesc{};
	if (rightObj)
		rightColliderDesc = *rightObj->Get_Component<CPhysicsCollider>()->GetDesc();

	return GAMEOBJECTINFO(leftObj, leftColliderDesc, rightObj, rightColliderDesc);
}

void CPhysics_FilterEventCallback::Ready_EventCallChain()
{
	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_ENTER] = [=](GAMEOBJECTINFO& info) {
		info.leftObject->OnCollision_Enter(info.leftColliderDesc.eFilterLayer, info.rightColliderDesc.eFilterLayer, info.rightObject,
			COL_HIT_INFO{ info.bHasHitPoint, COLLISIONEVENT::Enum::ON_COLLISION_ENTER, info.leftColliderDesc.iAttackPresetID , info.rightColliderDesc.iAttackPresetID, info.fDepth, info.vHitPoint, info.vRawNormal });
#ifdef _DEBUG
		Debug_Log(COLLISIONEVENT::Enum::ON_COLLISION_ENTER, info);
#endif // _DEBUG
		};

	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_STAY] = [=](GAMEOBJECTINFO& info) {
		info.leftObject->OnCollision(info.leftColliderDesc.eFilterLayer, info.rightColliderDesc.eFilterLayer, info.rightObject);
#ifdef _DEBUG
		Debug_Log(COLLISIONEVENT::Enum::ON_COLLISION_STAY, info);
#endif // _DEBUG
		};

	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_COLLISION_EXIT] = [=](GAMEOBJECTINFO& info) {
		info.leftObject->OnCollision_Exit(info.leftColliderDesc.eFilterLayer, info.rightColliderDesc.eFilterLayer, info.rightObject);
#ifdef _DEBUG
		Debug_Log(COLLISIONEVENT::Enum::ON_COLLISION_EXIT, info);
#endif // _DEBUG
		};

	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_TRIGGER_ENTER] = [=](GAMEOBJECTINFO& info) {
		info.leftObject->OnTrigger_Enter(info.leftColliderDesc.eFilterLayer, info.rightColliderDesc.eFilterLayer, info.rightObject);
#ifdef _DEBUG
		Debug_Log(COLLISIONEVENT::Enum::ON_TRIGGER_ENTER, info);
#endif // _DEBUG
		};

	m_arrCollisionEvent[COLLISIONEVENT::Enum::ON_TRIGGER_EXIT] = [=](GAMEOBJECTINFO& info) {
		info.leftObject->OnTrigger_Exit(info.leftColliderDesc.eFilterLayer, info.rightColliderDesc.eFilterLayer, info.rightObject);
#ifdef _DEBUG
		Debug_Log(COLLISIONEVENT::Enum::ON_TRIGGER_EXIT, info);
#endif // _DEBUG
		};
}

#ifdef _DEBUG
void CPhysics_FilterEventCallback::Debug_Log(COLLISIONEVENT::Enum event, GAMEOBJECTINFO& info)
{
	if (event == COLLISIONEVENT::Enum::END)
		return;

	wstring logHeader = m_arrEventString[event];
	wstring leftInfo = {};
	wstring rightInfo = {};

	if (info.leftObject)
		leftInfo = info.leftName + L", ID : " + std::to_wstring(info.leftID) + L"\n";
	else
		leftInfo = L"NULL\n";

	if (info.rightObject)
		rightInfo = info.rightName + L", ID : " + std::to_wstring(info.rightID) + L"\n";
	else
		rightInfo = L"NULL\n";

	CLOG_INFO(logHeader + leftInfo + rightInfo);
}
#endif // _DEBUG

CPhysics_FilterEventCallback* CPhysics_FilterEventCallback::Create()
{
	CPhysics_FilterEventCallback* pInstance = new CPhysics_FilterEventCallback();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_FilterEventCallback");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_FilterEventCallback::Free()
{
	Super::Free();
}