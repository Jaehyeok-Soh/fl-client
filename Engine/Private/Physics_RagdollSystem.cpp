#include "Engine_pch.h"

#include "Physics_RagdollSystem.h"

#include "GameInstance.h"
#include "EngineConsole.h"

#include "GameObject.h"
#include "PhysicsRagdoll.h"
#include "StructuredBuffer.h"

CPhysics_RagdollSystem::CPhysics_RagdollSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
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

HRESULT CPhysics_RagdollSystem::Initialize()
{
	return S_OK;
}

RAGDOLLELEMENTS CPhysics_RagdollSystem::CreateRagdoll(array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc)
{
	RAGDOLLELEMENTS elements{};
	elements.pArticulation = m_pPhysics->createArticulationReducedCoordinate(); // RCA : Featherstone's algorithm
	elements.vecPhysicsLink.resize(RAGDOLLJOINT::END);
	elements.vecRagdollLiveTransform.resize(RAGDOLLJOINT::END);

	PxTransform pxTransform = m_pGameInstance->XMMatrixToPxTransform(arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS].matLocalTransform);
	auto link = elements.pArticulation->createLink(NULL, pxTransform); // local transform
	link->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	link->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);

	PxMaterial* material = m_pGameInstance->GetPhysicsMaterial(EPhysicsMaterial::PLAYER);

	PxRigidBodyExt::updateMassAndInertia(*link, 10.f /*arrRagdollBoneDesc[i].fMass*/);
	PxRigidActorExt::createExclusiveShape(*link, PxCapsuleGeometry(arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS].fRadius, arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS].fHeight), *material);

	elements.vecPhysicsLink[arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS].eJoint] = std::make_pair(link, arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS]);

	for (_int i = RAGDOLLJOINT::PELVIS + 1; i < RAGDOLLJOINT::END; i++)
	{
		if (arrRagdollBoneDesc[i].eParentJoint == arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS].eJoint)
			CreateRagdollLink(&elements, arrRagdollBoneDesc, i, link);
	}

	return elements;
}

void CPhysics_RagdollSystem::Register(CGameObject* obj)
{
	uint64 id = obj->Get_ID();

	auto item = m_umapRegisteredMap.find(id);
	if (item != m_umapRegisteredMap.end())
		return;

	m_umapRegisteredMap.emplace(id, std::pair(obj, ERagdollState::SLEEP));
}

void CPhysics_RagdollSystem::Unregister(int64 objID)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	m_umapRegisteredMap.erase(item);
}

void CPhysics_RagdollSystem::RequestStart(uint64 objID)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	if (item->second.second == ERagdollState::PROCESSING)
		return;

	item->second.second = ERagdollState::PENDING;
}

void CPhysics_RagdollSystem::SyncStates(uint64 objID, vector<class CChannel*>& vecChannels)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	if (item->second.second == ERagdollState::PENDING)
	{
		Awake(objID, vecChannels);
		return;
	}
	else if (item->second.second == ERagdollState::PROCESSING)
	{
		Process(objID, vecChannels);
		return;
	}
}

void CPhysics_RagdollSystem::Finish(uint64 objID)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	item->second.second = ERagdollState::SLEEP;

	item->second.first->Get_Component<CPhysicsRagdoll>()->Sleep();
}

void CPhysics_RagdollSystem::CreateRagdollLink(RAGDOLLELEMENTS* elements, array<RAGDOLLBONEDESC, RAGDOLLJOINT::END> arrRagdollBoneDesc, _int index, PxArticulationLink* parentLink)
{
	PxTransform pxTransform = m_pGameInstance->XMMatrixToPxTransform(arrRagdollBoneDesc[index].matLocalTransform);
	auto link = elements->pArticulation->createLink(parentLink, pxTransform); // local transform
	link->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	link->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);

	PxMaterial* material = m_pGameInstance->GetPhysicsMaterial(EPhysicsMaterial::PLAYER);

	PxRigidBodyExt::updateMassAndInertia(*link, arrRagdollBoneDesc[index].fMass);
	PxRigidActorExt::createExclusiveShape(*link, PxCapsuleGeometry(arrRagdollBoneDesc[index].fRadius, arrRagdollBoneDesc[index].fHeight), *material);

	PxArticulationJointReducedCoordinate* articulationJoint = static_cast<PxArticulationJointReducedCoordinate*>(link->getInboundJoint());
	articulationJoint->setParentPose(articulationJoint->getParentArticulationLink().getGlobalPose().getInverse() * pxTransform);
	articulationJoint->setChildPose(link->getGlobalPose().getInverse() * pxTransform);

	elements->vecPhysicsLink[arrRagdollBoneDesc[index].eJoint] = std::make_pair(link, arrRagdollBoneDesc[index]);

	if (index + 1 >= RAGDOLLJOINT::END)
		return;

	for (_int i = index + 1; i < RAGDOLLJOINT::END; i++)
	{
		if (arrRagdollBoneDesc[i].eParentJoint == arrRagdollBoneDesc[index].eJoint)
			CreateRagdollLink(elements, arrRagdollBoneDesc, i, link);
	}
}

void CPhysics_RagdollSystem::Awake(uint64 objID, vector<class CChannel*>& vecChannels)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	item->second.second = ERagdollState::PROCESSING;

	item->second.first->Get_Component<CPhysicsRagdoll>()->Awake(vecChannels);
}

void CPhysics_RagdollSystem::Process(uint64 objID, vector<class CChannel*>& vecChannels)
{
	auto item = m_umapRegisteredMap.find(objID);
	if (item == m_umapRegisteredMap.end())
		return;

	item->second.first->Get_Component<CPhysicsRagdoll>()->Update();
}

CPhysics_RagdollSystem* CPhysics_RagdollSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, PxPhysics* pPhysics, PxScene* pScene)
{
	CPhysics_RagdollSystem* pInstance = new CPhysics_RagdollSystem(pDevice, pContext, pPhysics, pScene);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPhysics_RagdollSystem");
	}

	return pInstance;
}

void CPhysics_RagdollSystem::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
