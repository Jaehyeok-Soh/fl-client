#include "Engine_pch.h"

#include "Physics_CCTFilterCallback.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "PhysicsCollider.h"

#include "GameObject.h"

bool CPhysics_CCTFilterCallback::filter(const PxController& a, const PxController& b)
{
	if (a.getUserData() == nullptr || b.getUserData() == nullptr)
		return false;

	_bool aIsAlive = static_cast<CGameObject*>(a.getUserData())->IsAlive();
	_bool bIsAlive = static_cast<CGameObject*>(b.getUserData())->IsAlive();

	if (!aIsAlive || !bIsAlive)
		return false;

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