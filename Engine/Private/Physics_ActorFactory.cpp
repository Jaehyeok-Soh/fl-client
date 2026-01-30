#include "Engine_pch.h"

#include "GameInstance.h"
#include "Physics_ActorFactory.h"

CPhysics_ActorFactory::CPhysics_ActorFactory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
	: m_pGameInstance{ CGameInstance::GetInstance() },
	m_pDevice{ pDevice },
	m_pContext{ pContext },
	m_pPhysics{ pPhysics },
	m_pScene{ pScene }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CPhysics_ActorFactory::Initialize()
{
	return S_OK;
}

PxRigidActor* CPhysics_ActorFactory::GetActor(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	switch (rigidBodyDesc->eType)
	{
	case EPhysicsActorType::STATIC:
		return MakeStatic(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::DYNAMIC:
		return MakeDynamic(rigidBodyDesc, colliderDesc, shapes);

	case EPhysicsActorType::KINEMATIC:
		return MakeKinematic(rigidBodyDesc, colliderDesc, shapes);

	default:
		return MakeStatic(rigidBodyDesc, colliderDesc, shapes);
	}
}

PxRigidActor* CPhysics_ActorFactory::MakeStatic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	//PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pParentMatrix);

	//PxRigidStatic* staticActor = m_pPhysics->createRigidStatic(transform);
	//for (auto& shape : shapes)
	//	staticActor->attachShape(*shape);

	//return staticActor;
	return nullptr;
}

PxRigidActor* CPhysics_ActorFactory::MakeDynamic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	//PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pParentMatrix);

	//PxRigidDynamic* dynamicActor = m_pPhysics->createRigidDynamic(transform);
	//for (auto& shape : shapes)
	//	dynamicActor->attachShape(*shape);

	//PxRigidBodyExt::updateMassAndInertia(*dynamicActor, rigidBodyDesc->fDensity);

	//return dynamicActor;
	return nullptr;
}

PxRigidActor* CPhysics_ActorFactory::MakeKinematic(PHYSICSRIGIDBODY_DESC* rigidBodyDesc, PHYSICSCOLLIDER_DESC* colliderDesc, vector<PxShape*>& shapes)
{
	//PxTransform transform = m_pGameInstance->XMMatrixToPxTransform(*rigidBodyDesc->pParentMatrix);

	//PxRigidDynamic* kinematicActor = m_pPhysics->createRigidDynamic(transform);
	//for (auto& shape : shapes)
	//	kinematicActor->attachShape(*shape);

	//PxRigidBodyExt::updateMassAndInertia(*kinematicActor, rigidBodyDesc->fDensity);

	//kinematicActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);

	//return kinematicActor;
	return nullptr;
}

CPhysics_ActorFactory* CPhysics_ActorFactory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
{
	CPhysics_ActorFactory* pInstance = new CPhysics_ActorFactory(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_ActorFactory");
	}

	return pInstance;
}

void CPhysics_ActorFactory::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
