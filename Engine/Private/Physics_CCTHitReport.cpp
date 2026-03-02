#include "Engine_pch.h"

#include "Physics_CCTHitReport.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"
#include "PhysicsCCT.h"

//struct PxControllerHit
//{
//	PxController* controller;		//!< Current controller
//	PxExtendedVec3	worldPos;		//!< Contact position in world space
//	PxVec3			worldNormal;	//!< Contact normal in world space
//	PxVec3			dir;			//!< Motion direction
//	PxF32			length;			//!< Motion length
//};

///**
//\brief Describes a hit between a CCT and a shape. Passed to onShapeHit()
//
//\see PxUserControllerHitReport.onShapeHit()
//*/
//struct PxControllerShapeHit : public PxControllerHit
//{
//	PxShape* shape;			//!< Touched shape
//	PxRigidActor* actor;			//!< Touched actor
//	PxU32			triangleIndex;	//!< touched triangle index (only for meshes/heightfields)
//};

///**
//\brief Describes a hit between a CCT and another CCT. Passed to onControllerHit().
//
//\see PxUserControllerHitReport.onControllerHit()
//*/
//struct PxControllersHit : public PxControllerHit
//{
//	PxController* other;			//!< Touched controller
//};

void CPhysics_CCTHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
	// 아직 미사용 2026 02 26
	return;

	if (hit.shape->getQueryFilterData().word0 & PHYSICSFILTERGROUP::MAP)
		return;

	GAMEOBJECTINFO info = Get_GameObject(hit.controller->getUserData(), hit.actor->userData);

	if (hit.actor->getType() == PxActorType::Enum::eRIGID_STATIC)
	{
		// 지형
	}
	else if (hit.actor->getType() == PxActorType::Enum::eRIGID_DYNAMIC)
	{
		// 동적 오브젝트
	}

	//#ifdef _DEBUG
	//	Debug_Log(HITEVENT::Enum::ON_SHAPE_HIT, info);
	//#endif // _DEBUG
}

void CPhysics_CCTHitReport::onControllerHit(const PxControllersHit& hit)
{
	GAMEOBJECTINFO info = Get_GameObject(hit.controller->getUserData(), hit.other->getUserData());

	auto pCCT = static_cast<CGameObject*>(hit.controller->getUserData())->Get_Component<CPhysicsCCT>();

	if (hit.worldNormal.y > 0.3f)
	{
		PxExtendedVec3 myPos = hit.controller->getPosition();
		PxExtendedVec3 otherPos = hit.other->getPosition();

		PxVec3 pushDir;
		pushDir.x = (float)(myPos.x - otherPos.x);
		pushDir.y = 0.f;
		pushDir.z = (float)(myPos.z - otherPos.z);

		if (pushDir.magnitudeSquared() < 1e-6f)
			pushDir = PxVec3(1.f, 0.f, 0.f);

		pushDir.normalize();

		pushDir *= 0.05f;

		Vec3 disp(pushDir.x, -0.1f, pushDir.z);
		pCCT->Add_Disp(disp);
		pCCT->SetIsSteppingOnCCT();
		return;
	}

	if (hit.dir.y < -0.8f)
	{
		PxExtendedVec3 lowerPos = hit.other->getPosition();
		PxExtendedVec3 upperPos = hit.controller->getPosition();

		PxVec3 slideDir;
		slideDir.x = (float)(upperPos.x - lowerPos.x);
		slideDir.y = 0.f;
		slideDir.z = (float)(upperPos.z - lowerPos.z);

		if (slideDir.magnitudeSquared() < 1e-6f)
		{
			slideDir = PxVec3(0.1f, 0.f, 0.1f);
		}
		slideDir.normalize();
		slideDir = slideDir * 0.01f;
		Vec3 disp(slideDir.x, slideDir.y, slideDir.z);
		pCCT->Add_Disp(disp);
		pCCT->SetIsSteppingOnCCT();
	}

	if (abs(hit.worldNormal.y) < 0.5f)
	{
		PxVec3 normal = hit.worldNormal.getNormalized();
		Vec3 disp(normal.x, 0.f, normal.z);
		disp *= 0.01f;
		pCCT->Add_Disp(disp);
		pCCT->SetIsSideOnCCT();
	}

//#ifdef _DEBUG
//	Debug_Log(HITEVENT::Enum::ON_CCT_HIT, info);
//#endif // _DEBUG
}

CPhysics_CCTHitReport::CPhysics_CCTHitReport()
{
}

HRESULT CPhysics_CCTHitReport::Initialize()
{
	Ready_EventCallChain();

#ifdef _DEBUG
	m_arrEventString[HITEVENT::Enum::ON_SHAPE_HIT] = L"[On shape hit]\n";
	m_arrEventString[HITEVENT::Enum::ON_CCT_HIT] = L"[On character hit]\n";
#endif // _DEBUG

	return S_OK;
}

CGameObject* CPhysics_CCTHitReport::Conversion_GameObject(void* userData)
{
	return static_cast<CGameObject*>(userData);
}

CPhysics_CCTHitReport::GAMEOBJECTINFO CPhysics_CCTHitReport::Get_GameObject(void* leftArgs, void* rightArgs)
{
	CGameObject* leftObject = Conversion_GameObject(leftArgs);
	PHYSICSCOLLIDER_DESC* leftColliderDesc = { nullptr };
	if (leftObject)
		leftColliderDesc = leftObject->Get_Component<CPhysicsCollider>()->GetDesc();

	CGameObject* rightObject = Conversion_GameObject(rightArgs);
	PHYSICSCOLLIDER_DESC* rightColliderDesc = { nullptr };
	if (rightObject)
		rightColliderDesc = rightObject->Get_Component<CPhysicsCollider>()->GetDesc();

	return GAMEOBJECTINFO(leftObject, leftColliderDesc, rightObject, rightColliderDesc);
}

void CPhysics_CCTHitReport::Ready_EventCallChain()
{
}

#ifdef _DEBUG
void CPhysics_CCTHitReport::Debug_Log(HITEVENT::Enum event, GAMEOBJECTINFO& info)
{
	if (event == HITEVENT::Enum::END)
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

CPhysics_CCTHitReport* CPhysics_CCTHitReport::Create()
{
	CPhysics_CCTHitReport* pInstance = new CPhysics_CCTHitReport();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_CCTHitReport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_CCTHitReport::Free()
{
	Super::Free();
}
