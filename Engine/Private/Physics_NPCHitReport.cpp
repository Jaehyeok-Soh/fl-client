#include "Engine_pch.h"

#include "Physics_NPCHitReport.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"

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

void CPhysics_NPCHitReport::onShapeHit(const PxControllerShapeHit& hit)
{
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

void CPhysics_NPCHitReport::onControllerHit(const PxControllersHit& hit)
{
	GAMEOBJECTINFO info = Get_GameObject(hit.controller->getUserData(), hit.other->getUserData());

#ifdef _DEBUG
	Debug_Log(HITEVENT::Enum::ON_CCT_HIT, info);
#endif // _DEBUG
}

CPhysics_NPCHitReport::CPhysics_NPCHitReport()
{
}

HRESULT CPhysics_NPCHitReport::Initialize()
{
	Ready_EventCallChain();

	m_arrEventString[HITEVENT::Enum::ON_SHAPE_HIT] = L"[On player to shape hit]\n";
	m_arrEventString[HITEVENT::Enum::ON_CCT_HIT] = L"[On player to character hit]\n";

	return S_OK;
}

CGameObject* CPhysics_NPCHitReport::Conversion_GameObject(void* userData)
{
	return static_cast<CGameObject*>(userData);
}

CPhysics_NPCHitReport::GAMEOBJECTINFO CPhysics_NPCHitReport::Get_GameObject(void* leftArgs, void* rightArgs)
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

void CPhysics_NPCHitReport::Ready_EventCallChain()
{
}

#ifdef _DEBUG
void CPhysics_NPCHitReport::Debug_Log(HITEVENT::Enum event, GAMEOBJECTINFO& info)
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

CPhysics_NPCHitReport* CPhysics_NPCHitReport::Create()
{
	CPhysics_NPCHitReport* pInstance = new CPhysics_NPCHitReport();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPhysics_NPCHitReport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPhysics_NPCHitReport::Free()
{
	Super::Free();
}
