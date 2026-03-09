#include "Engine_pch.h"
#include "PhysicsRagdoll.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "PhysicsCollider.h"

#include "Transform.h"
#include "Model.h"
#include "StructuredBuffer.h"
#include "Channel.h"

CPhysicsRagdoll::CPhysicsRagdoll()
	: Super()
{
}

CPhysicsRagdoll::CPhysicsRagdoll(const CPhysicsRagdoll& rhs)
	: Super(rhs)
{
}

HRESULT CPhysicsRagdoll::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPhysicsRagdoll::Initialize(void* pArg)
{
	if ((m_pSharedModel = m_pOwner->Get_Component<CModel>()) == nullptr)
		MSG_BOX("Failed clone : none model : CPhysicsRagdoll");

	m_tRagdollElements = m_pGameInstance->CreateRagdoll(m_pSharedModel->Get_RagdollBoneDesc());

	m_pGameInstance->AddRagdoll(m_tRagdollElements.pArticulation);

	return S_OK;
}

void CPhysicsRagdoll::Awake(vector<CChannel*>& vecChannels)
{
	Matrix objectWorld = Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrix();
	PxTransform pxObjectWorld = m_pGameInstance->XMMatrixToPxTransform(objectWorld);

	auto& link = m_tRagdollElements.vecPhysicsLink[RAGDOLLJOINT::PELVIS];
	link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

	KEYFRAME lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
	Vec3 translation = lastKeyframe.vTranslation;
	Vec4 quaternion = lastKeyframe.vQuaterion;

	PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z),
		PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

	PxTransform pxGlobal = pxObjectWorld * pxLocal;

	link.first->setGlobalPose(pxGlobal * link.second.matOffsetTransform);

	for (size_t i = RAGDOLLJOINT::PELVIS + 1; i < RAGDOLLJOINT::END; i++)
	{
		auto desc = m_tRagdollElements.vecPhysicsLink[i].second;
		
		if (desc.eParentJoint == RAGDOLLJOINT::PELVIS)
			CombinedJoint((RAGDOLLJOINT::Enum)i, pxObjectWorld, pxLocal, vecChannels);
	}
}

void CPhysicsRagdoll::Update()
{
}

void CPhysicsRagdoll::Sleep()
{
	for (auto& link : m_tRagdollElements.vecPhysicsLink)
		link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

#ifdef _DEBUG
void CPhysicsRagdoll::Render()
{
	//for (auto& link : m_tRagdollElements.vecPhysicsLink)
	//{
	//}
}
#endif // _DEBUG

void CPhysicsRagdoll::SetUserData(CGameObject* pObject)
{
	void* userData = static_cast<void*>(pObject);

	for (auto& actor : m_pActors)
		actor->userData = userData;
}

void CPhysicsRagdoll::CombinedJoint(RAGDOLLJOINT::Enum eJoint, PxTransform ObjectWorldTransform, PxTransform parentTransform, vector<CChannel*>& vecChannels)
{
	auto& link = m_tRagdollElements.vecPhysicsLink[eJoint];
	link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

	KEYFRAME lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
	Vec3 translation = lastKeyframe.vTranslation;
	Vec4 quaternion = lastKeyframe.vQuaterion;
	PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z),
		PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

	PxTransform pxCombined = parentTransform * pxLocal;

	PxTransform pxGlobal = ObjectWorldTransform * pxCombined;

	link.first->setGlobalPose(pxGlobal * link.second.matOffsetTransform);

	for (size_t i = eJoint + 1; i < RAGDOLLJOINT::END; i++)
	{
		auto desc = m_tRagdollElements.vecPhysicsLink[i].second;

		if (desc.eParentJoint == eJoint)
			CombinedJoint((RAGDOLLJOINT::Enum)i, ObjectWorldTransform, pxCombined, vecChannels);
	}
}

CPhysicsRagdoll* CPhysicsRagdoll::Create()
{
	CPhysicsRagdoll* pInstance = new CPhysicsRagdoll();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPhysicsRagdoll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CPhysicsRagdoll::Clone(void* pArg)
{
	CPhysicsRagdoll* pInstance = new CPhysicsRagdoll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPhysicsRagdoll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPhysicsRagdoll::Free()
{
	Super::Free();
}
