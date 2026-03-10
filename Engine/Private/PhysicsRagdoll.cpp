#include "Engine_pch.h"
#include "PhysicsRagdoll.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "PhysicsCollider.h"

#include "Bone.h"
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

	m_pGameInstance->RagdollRegister(Get_Owner());

	return S_OK;
}

void CPhysicsRagdoll::Awake(vector<CChannel*>& vecChannels)
{
	Matrix objectWorld = Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrix();
	PxTransform pxObjectWorld = m_pGameInstance->XMMatrixToPxTransform(objectWorld);
	vector<CBone*>& vecBone = m_pSharedModel->Get_Bones();

	{
		auto& link = m_tRagdollElements.vecPhysicsLink[RAGDOLLJOINT::PELVIS];
		link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

		KEYFRAME lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
		Vec3 translation = lastKeyframe.vTranslation;
		Vec4 quaternion = lastKeyframe.vQuaterion;

		PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z),
			PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

		if (link.second.iParentIndex >= 0)
		{
			CBone* currentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[RAGDOLLJOINT::PELVIS].c_str());
			//CBone* parentJoint = nullptr;
			pxLocal = BoneCombine(currentJoint, pxLocal, nullptr, vecChannels, vecBone);
		}

		PxTransform pxGlobal = pxObjectWorld * pxLocal;

		link.first->setGlobalPose(pxGlobal * link.second.matOffsetTransform);

		for (size_t i = RAGDOLLJOINT::PELVIS + 1; i < RAGDOLLJOINT::END; i++)
		{
			auto desc = m_tRagdollElements.vecPhysicsLink[i].second;

			if (desc.eParentJoint == RAGDOLLJOINT::PELVIS)
				CombinedJoint((RAGDOLLJOINT::Enum)i, pxObjectWorld, pxLocal, vecChannels, vecBone);
		}
	}
}

void CPhysicsRagdoll::Update()
{
	//Vec3 objScale = Get_Owner()->Get_Component<CTransform>()->Get_Scaled();
	//Matrix matScale = Matrix::CreateScale(objScale);

	//for (_int i = RAGDOLLJOINT::PELVIS; i < RAGDOLLJOINT::END; i++)
	//{
	//	auto& link = m_tRagdollElements.vecPhysicsLink[i];
	//	PxTransform pose = link.first->getGlobalPose() * link.second.matOffsetTransform.getInverse();
	//	Matrix matGlobal = m_pGameInstance->PxTransformToXMMatrix(pose);

	//	Matrix matParentGlobal = Matrix::Identity;
	//	if (link.second.eParentJoint != RAGDOLLJOINT::END)
	//	{
	//		auto& parentLink = m_tRagdollElements.vecPhysicsLink[link.second.eParentJoint];
	//		PxTransform parentPose = parentLink.first->getGlobalPose() * parentLink.second.matOffsetTransform.getInverse();
	//		matParentGlobal = m_pGameInstance->PxTransformToXMMatrix(parentPose);
	//	}
	//	else
	//	{
	//		matParentGlobal = Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrix();
	//	}

	//	Matrix matLocal = matGlobal * matParentGlobal.Invert();

	//	m_tRagdollElements.vecRagdollLiveTransform[i] = matLocal;
	//}

	Matrix objectWorldInverse = Get_Owner()->Get_Component<CTransform>()->Get_WorldMatrix_Inverse();

	for (_int i = 0; i < RAGDOLLJOINT::END; i++)
	{
		auto& link = m_tRagdollElements.vecPhysicsLink[i];

		PxTransform pose = link.first->getGlobalPose() * link.second.matOffsetTransform.getInverse();
		Matrix matGlobal = m_pGameInstance->PxTransformToXMMatrix(pose);

		Matrix matLocal = matGlobal * objectWorldInverse;

		m_tRagdollElements.vecRagdollLiveTransform[i] = matLocal;
	}
}

_int CPhysicsRagdoll::FindRagdollJointByBoneIndex(_uint boneIdx)
{
	for (_int i = 0; i < RAGDOLLJOINT::END; i++)
	{
		if (m_tRagdollElements.vecPhysicsLink[i].second.iBoneIndex == boneIdx)
			return i;
	}
	return -1;
}

void CPhysicsRagdoll::Sleep()
{
	for (auto& link : m_tRagdollElements.vecPhysicsLink)
		link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
}

#ifdef _DEBUG
void CPhysicsRagdoll::Render()
{
	for (auto& link : m_tRagdollElements.vecPhysicsLink)
	{
		if (!link.first)
			continue;

		m_pGameInstance->Physics_Render(static_cast<PxRigidActor*>(link.first), DirectX::Colors::LimeGreen);
	}
}
#endif // _DEBUG

void CPhysicsRagdoll::CombinedJoint(RAGDOLLJOINT::Enum eJoint, PxTransform ObjectWorldTransform, PxTransform parentTransform, vector<CChannel*>& vecChannels, vector<class CBone*>& vecBone)
{
	auto& link = m_tRagdollElements.vecPhysicsLink[eJoint];
	link.first->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

	KEYFRAME lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
	Vec3 translation = lastKeyframe.vTranslation;
	Vec4 quaternion = lastKeyframe.vQuaterion;
	PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z),
		PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

	if (link.second.iParentIndex >= 0)
	{
		CBone* currentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[eJoint].c_str());
		CBone* parentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[link.second.eParentJoint].c_str());
		pxLocal = BoneCombine(currentJoint, pxLocal, parentJoint, vecChannels, vecBone);
	}

	PxTransform pxCombined = parentTransform * pxLocal;

	PxTransform pxGlobal = ObjectWorldTransform * pxCombined;

	link.first->setGlobalPose(pxGlobal * link.second.matOffsetTransform);

	for (size_t i = eJoint + 1; i < RAGDOLLJOINT::END; i++)
	{
		auto desc = m_tRagdollElements.vecPhysicsLink[i].second;

		if (desc.eParentJoint == eJoint)
			CombinedJoint((RAGDOLLJOINT::Enum)i, ObjectWorldTransform, pxCombined, vecChannels, vecBone);
	}
}

PxTransform CPhysicsRagdoll::BoneCombine(class CBone* pCurrentJoint, PxTransform pxLocal, class CBone* pParentJoint, vector<CChannel*>& vecChannels, vector<class CBone*>& vecBone)
{
	PxTransform pxTemp = pxLocal;

	_int parentIndex = pCurrentJoint->Get_ParentIndex();

	if (parentIndex < 0 || vecBone[parentIndex] == pParentJoint)
		return pxTemp;

	auto& bone = vecBone[parentIndex];

	KEYFRAME lastKeyframe = vecChannels[parentIndex]->Get_KeyFrames().back();
	Vec3 translation = lastKeyframe.vTranslation;
	Vec4 quaternion = lastKeyframe.vQuaterion;

	PxTransform pxParentLocal = PxTransform(PxVec3(translation.x, translation.y, translation.z),
		PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w));

	pxTemp = pxParentLocal * pxTemp;

	pxTemp = BoneCombine(bone, pxTemp, pParentJoint, vecChannels, vecBone);

	return pxTemp;
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
