#include "Engine_pch.h"
#include "PhysicsRagdoll.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "PhysicsCCT.h"

#include "PartObject.h"
#include "Bone.h"
#include "Transform.h"
#include "Model.h"
#include "StructuredBuffer.h"
#include "Channel.h"
#include "ComputeShader.h"

#include "EngineConsole.h"

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
	auto pOwner = static_cast<CGameObject*>(pArg);
	m_iObjectID = pOwner->Get_ID();

	if ((m_pSharedModel = pOwner->Get_Component<CModel>()) == nullptr)
		MSG_BOX("Failed clone : none model : CPhysicsRagdoll");

	m_tRagdollElements = m_pGameInstance->CreateRagdoll(m_pSharedModel->Get_RagdollBoneDesc());

	m_pGameInstance->AddRagdoll(m_tRagdollElements.pArticulation);
	m_tRagdollElements.pArticulation->putToSleep();

	m_pGameInstance->RagdollRegister(pOwner);

	return S_OK;
}

void CPhysicsRagdoll::Awake(vector<CChannel*>& vecChannels)
{
	if (m_pGameInstance->CheckRagdollState(m_iObjectID) == false)
		return;

	m_pGameInstance->RemoveRagdoll(m_tRagdollElements.pArticulation);

	Matrix objectWorld = static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();
	PxTransform pxObjectWorld = m_pGameInstance->XMMatrixToPxTransform(objectWorld);
	vector<CBone*>& vecBone = m_pSharedModel->Get_Bones();

	if (pxObjectWorld.q.magnitudeSquared() < 1e-6f)
		pxObjectWorld.q = PxQuat(PxIdentity);
	else
		pxObjectWorld.q.normalize();

	pxObjectWorld.p.y += 0.5f;

	{
		auto& link = m_tRagdollElements.vecPhysicsLink[RAGDOLLJOINT::PELVIS];

		PxTransform pose = link.first->getGlobalPose();
		if (!pose.isValid())
		{
			MSG_BOX("failed awake : invalid pose : ragdoll");
			return;
		}

		KEYFRAME	lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
		Vec3		translation = lastKeyframe.vTranslation;
		Vec4		quaternion = lastKeyframe.vQuaterion;

		PxQuat pxRot = PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		pxRot.normalize();

		PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z), pxRot);

		if (link.second.iParentIndex >= 0)
		{
			CBone* currentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[RAGDOLLJOINT::PELVIS].c_str());
			//CBone* parentJoint = nullptr;
			pxLocal = BoneCombine(currentJoint, pxLocal, nullptr, vecChannels, vecBone);
		}

		PxTransform pxGlobal = pxObjectWorld * pxLocal;
		//PxTransform finalPose = pxGlobal * link.second.matOffsetTransform;
		if (!pxGlobal.q.isFinite() || pxGlobal.q.magnitudeSquared() < 1e-6f)
			pxGlobal.q = PxQuat(PxIdentity);
		else
			pxGlobal.q.normalize();

		if (!pxGlobal.p.isFinite())
			return;

		//link.first->setGlobalPose(finalPose);

		//for (size_t i = RAGDOLLJOINT::PELVIS + 1; i < RAGDOLLJOINT::END; i++)
		//{
		//	auto desc = m_tRagdollElements.vecPhysicsLink[i].second;

		//	if (desc.eParentJoint == RAGDOLLJOINT::PELVIS)
		//		CombinedJoint((RAGDOLLJOINT::Enum)i, pxObjectWorld, pxLocal, vecChannels, vecBone);
		//}

		//m_tRagdollElements.pArticulation->updateKinematic(PxArticulationKinematicFlag::ePOSITION);
		
		m_tRagdollElements.pArticulation->setRootGlobalPose(pxGlobal, false);

		m_pGameInstance->AddRagdoll(m_tRagdollElements.pArticulation);

		m_tRagdollElements.pArticulation->wakeUp();
	}
}

void CPhysicsRagdoll::Awake()
{
	if (m_pGameInstance->CheckRagdollState(m_iObjectID) == false)
		return;

	m_pGameInstance->RemoveRagdoll(m_tRagdollElements.pArticulation);

	Matrix objectWorld = static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix();
	PxTransform pxObjectWorld = m_pGameInstance->XMMatrixToPxTransform(objectWorld);
	vector<CBone*>& vecBone = m_pSharedModel->Get_Bones();

	if (pxObjectWorld.q.magnitudeSquared() < 1e-6f)
		pxObjectWorld.q = PxQuat(PxIdentity);
	else
		pxObjectWorld.q.normalize();

	pxObjectWorld.p.y += 1.f;

	{
		auto& link = m_tRagdollElements.vecPhysicsLink[RAGDOLLJOINT::PELVIS];

		PxTransform pose = link.first->getGlobalPose();
		if (!pose.isValid())
		{
			MSG_BOX("failed awake : invalid pose : ragdoll");
			return;
		}

		_uint i = { 0 };
		//for (auto& link : m_tRagdollElements.vecPhysicsLink)
		{
			int boneIndex = link.second.iBoneIndex;

			Matrix boneCombined = vecBone[boneIndex]->Get_CombinedTransformMatrix();

			PxTransform pxBoneCombine = m_pGameInstance->XMMatrixToPxTransform(boneCombined);

			PxTransform pxGlobal = pxObjectWorld * pxBoneCombine;
			//PxTransform finalPose = pxGlobal * link.second.matOffsetTransform;
			if (!pxGlobal.q.isFinite() || pxGlobal.q.magnitudeSquared() < 1e-6f)
				pxGlobal.q = PxQuat(PxIdentity);
			else
				pxGlobal.q.normalize();

			if (!pxGlobal.p.isFinite())
				return;


			//link.first->setGlobalPose(pxBoneWorld, false);

			if(i == 0)
				m_tRagdollElements.pArticulation->setRootGlobalPose(pxGlobal, false);
			//else
			//	link.first->setGlobalPose(pxGlobal, false);

			i++;
		}

		m_tRagdollElements.pArticulation->setRootLinearVelocity(PxVec3(0.f));
		m_tRagdollElements.pArticulation->setRootAngularVelocity(PxVec3(0.f));

		PxArticulationCache* pCache = m_tRagdollElements.pArticulation->createCache();
		if (pCache)
		{
			m_tRagdollElements.pArticulation->copyInternalStateToCache(*pCache, PxArticulationCacheFlag::eVELOCITY);

			if (pCache->jointVelocity)
			{
				memset(pCache->jointVelocity, 0, sizeof(PxReal) * m_tRagdollElements.pArticulation->getDofs());
			}

			m_tRagdollElements.pArticulation->applyCache(*pCache, PxArticulationCacheFlag::eVELOCITY);
			pCache->release();
		}

		m_pGameInstance->AddRagdoll(m_tRagdollElements.pArticulation);

		m_tRagdollElements.pArticulation->wakeUp();
	}
}

void CPhysicsRagdoll::Update()
{
	Matrix objectWorldInverse = static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>()->Get_WorldMatrix_Inverse();
	_uint iRagDollSize = ENUM_TO_UINT(ERagdollJoint::END);
	CS_OUT_BONE* pInitialData = new CS_OUT_BONE[iRagDollSize];
	// 1패스: 오브젝트 공간 combined 먼저 전부 계산
	for (_int i = 0; i < RAGDOLLJOINT::END; i++)
	{
		auto& link = m_tRagdollElements.vecPhysicsLink[i];
		if (!link.first) continue;
		PxTransform pose = link.first->getGlobalPose();
		Matrix matGlobal = m_pGameInstance->PxTransformToXMMatrix(pose);
		m_tRagdollElements.vecRagdollLiveTransform[i] = matGlobal * objectWorldInverse;
	}
	// 2패스: combined → bone-local 변환
	for (_int i = 0; i < RAGDOLLJOINT::END; i++)
	{
		auto& link = m_tRagdollElements.vecPhysicsLink[i];
		RAGDOLLJOINT::Enum eParentJoint = link.second.eParentJoint;
		Matrix matCombined = m_tRagdollElements.vecRagdollLiveTransform[i];
		Matrix matBoneLocal;
		if (eParentJoint < 0 || i == RAGDOLLJOINT::PELVIS)
		{
			// 루트는 그냥 combined = bone-local
			matBoneLocal = matCombined;
		}
		else
		{
			// 부모 combined 역행렬 * 자신 combined = bone-local
			Matrix matParentCombined = m_tRagdollElements.vecRagdollLiveTransform[eParentJoint];
			matBoneLocal = matCombined * matParentCombined.Invert();// * matCombined;
		}
		pInitialData[i].matCombinedTransform = matBoneLocal;
	}

	m_pMatrixBuffer->Copy_Data(pInitialData, sizeof(CS_OUT_BONE), iRagDollSize);
	Safe_Delete_Array(pInitialData);

	PxTransform rootTransform = m_tRagdollElements.pArticulation->getRootGlobalPose();;
	//rootTransform.p.y += 0.3f;
	CTransform* pOwnerTransform = static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>();
	rootTransform.q.normalize();
	Quat quat = ToQuaternion(rootTransform.q);

	static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CPhysicsCCT>()->SetFootPosition(ToVector3(rootTransform.p));
	pOwnerTransform->Rotation(quat);
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
	m_tRagdollElements.pArticulation->setRootLinearVelocity(PxVec3(0.f));
	m_tRagdollElements.pArticulation->setRootAngularVelocity(PxVec3(0.f));

	PxArticulationCache* pCache = m_tRagdollElements.pArticulation->createCache();
	if (pCache)
	{
		m_tRagdollElements.pArticulation->copyInternalStateToCache(*pCache, PxArticulationCacheFlag::eVELOCITY);

		if (pCache->jointVelocity)
		{
			memset(pCache->jointVelocity, 0, sizeof(PxReal) * m_tRagdollElements.pArticulation->getDofs());
		}

		m_tRagdollElements.pArticulation->applyCache(*pCache, PxArticulationCacheFlag::eVELOCITY);
		pCache->release();
	}

	m_tRagdollElements.pArticulation->putToSleep();

	PxTransform resetPose(PxVec3(0.f, -1000.f, 0.f), PxQuat(PxIdentity));
	m_tRagdollElements.pArticulation->setRootGlobalPose(resetPose, false);

	Quat quat = ToQuaternion(PxQuat(PxIdentity));

	CTransform* pOwnerTransform = static_cast<CPartObject*>(Get_Owner())->Get_Parent()->Get_Component<CTransform>();
	pOwnerTransform->Rotation(quat);

	m_pGameInstance->RemoveRagdoll(m_tRagdollElements.pArticulation);
}

HRESULT CPhysicsRagdoll::Setting_CS(CComputeShader* pRagDollCS, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	if (FAILED(Bind_RagDollCS_ImmuData(pRagDollCS)))
		return E_FAIL;

	if (FAILED(Setting_SB(pRagDollCS, pDevice, pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPhysicsRagdoll::Bind_RagDollCS_ImmuData(CComputeShader* pRagDollCS)
{
	// bone 불변 데이터 넣어줌

	_uint iRagDollSize = ENUM_TO_UINT(ERagdollJoint::END);
	_uint iBoneNums = m_pSharedModel->Get_BoneCount();

	// 1. 버퍼 내용 생성
	CS_IMMU_RAGDOLL* pInitialData = new CS_IMMU_RAGDOLL[iRagDollSize];

	for (size_t i = 0; i < (size_t)iRagDollSize; i++)
	{
		pInitialData[i].iBoneIndex			= m_tRagdollElements.vecPhysicsLink[i].second.iBoneIndex;
		pInitialData[i].iTotalBoneNums		= iBoneNums;
		pInitialData[i].iRagDollBoneNums	= iRagDollSize;
		pInitialData[i].Padding0			= 0;
	}

	// 2. 바로 바인딩
	pRagDollCS->Bind_InputStructuredBuffer_Data(ENUM_TO_UINT(CS_IDX::IMMU_BONEDATA), pInitialData, sizeof(CS_IMMU_RAGDOLL), iRagDollSize);
	Safe_Delete_Array(pInitialData);

	return S_OK;
}

HRESULT CPhysicsRagdoll::Setting_SB(CComputeShader* pRagDollCS, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	_uint iRagDollSize = ENUM_TO_UINT(ERagdollJoint::END);

	if (m_pMatrixBuffer)
		Safe_Release(m_pMatrixBuffer);
	if (m_pMatrixSB_SRV)
		Safe_Release(m_pMatrixSB_SRV);

	// 3. struct buffer class 생성
	m_pMatrixBuffer = StructuredBuffer::Create(pDevice, pDeviceContext, sizeof(CS_OUT_BONE), iRagDollSize);

	CS_OUT_BONE* pInitialData = new CS_OUT_BONE[iRagDollSize];
	for (size_t i = 0; i < (size_t)iRagDollSize; i++)
	{
		pInitialData[i].matCombinedTransform = m_tRagdollElements.vecRagdollLiveTransform[i];
	}

	m_pMatrixBuffer->Copy_Data(pInitialData, sizeof(CS_OUT_BONE), iRagDollSize);

	Safe_Delete_Array(pInitialData);

	// 4. SRV 연결
	m_pMatrixSB_SRV = pRagDollCS->Get_SRV("RAGDOLL_LOCAL_TRANSFORMS");
	m_pMatrixSB_SRV->SetResource(m_pMatrixBuffer->Get_SRV());

	return S_OK;
}

void CPhysicsRagdoll::Bind_RagDollCS_MuData(CComputeShader* pRagDollCS)
{
	pRagDollCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_IDX::MU_MATRIX), m_pMatrixSB_SRV, m_pMatrixBuffer);
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

	PxTransform pose = link.first->getGlobalPose();
	if (!pose.isValid())
	{
		MSG_BOX("failed awake : invalid pose : ragdoll");
		return;
	}

	KEYFRAME lastKeyframe = vecChannels[link.second.iBoneIndex]->Get_KeyFrames().back();
	Vec3 translation = lastKeyframe.vTranslation;
	Vec4 quaternion = lastKeyframe.vQuaterion;

	PxQuat pxRot = PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	pxRot.normalize();

	PxTransform pxLocal(PxVec3(translation.x, translation.y, translation.z), pxRot);

	if (link.second.iParentIndex >= 0)
	{
		CBone* currentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[eJoint].c_str());
		CBone* parentJoint = m_pSharedModel->Get_Bone(PhysicsJointNames[link.second.eParentJoint].c_str());
		pxLocal = BoneCombine(currentJoint, pxLocal, parentJoint, vecChannels, vecBone);
	}

	PxTransform pxCombined = parentTransform * pxLocal;
	PxTransform pxGlobal = ObjectWorldTransform * pxCombined;
	//PxTransform finalPose = pxGlobal * link.second.matOffsetTransform;
	if (!pxGlobal.q.isFinite() || pxGlobal.q.magnitudeSquared() < 1e-6f)
		pxGlobal.q = PxQuat(PxIdentity);
	else
		pxGlobal.q.normalize();

	if (!pxGlobal.p.isFinite())
		return;

	link.first->setGlobalPose(pxGlobal);

	//PxArticulationJointReducedCoordinate* joint = static_cast<PxArticulationJointReducedCoordinate*>(link.first->getInboundJoint());
	//if (joint)
	//{
	//	pxLocal.q.normalize();
	//	joint->setParentPose(pxLocal);
	//	joint->setChildPose(PxTransform(PxIdentity));
	//}

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

	PxQuat pxRot = PxQuat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	pxRot.normalize();

	PxTransform pxParentLocal = PxTransform(PxVec3(translation.x, translation.y, translation.z), pxRot);

	pxTemp = pxParentLocal * pxTemp;
	pxTemp.q.normalize();

	pxTemp = BoneCombine(bone, pxTemp, pParentJoint, vecChannels, vecBone);

	return pxTemp;
}

PxTransform CPhysicsRagdoll::BoneCombine(CBone* pCurrentJoint, PxTransform pxLocal, CBone* pParentJoint, vector<class CBone*>& vecBone)
{
	PxTransform pxTemp = pxLocal;

	_int parentIndex = pCurrentJoint->Get_ParentIndex();

	if (parentIndex < 0 || vecBone[parentIndex] == pParentJoint)
		return pxTemp;

	auto& bone = vecBone[parentIndex];

	Matrix matCombine = bone->Get_CombinedTransformMatrix();

	PxTransform pxParentLocal = m_pGameInstance->XMMatrixToPxTransform(matCombine);

	if (pxParentLocal.q.magnitudeSquared() < 1e-6f)
		pxParentLocal.q = PxQuat(PxIdentity);
	else
		pxParentLocal.q.normalize();

	pxParentLocal.p.y += 1.f;


	pxTemp = pxParentLocal * pxTemp;
	pxTemp.q.normalize();

	pxTemp = BoneCombine(bone, pxTemp, pParentJoint, vecBone);

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

	Safe_Release(m_pMatrixBuffer);
	Safe_Release(m_pMatrixSB_SRV);
}
