#include "Engine_pch.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "ModelAnimation.h"

#include "GameDataManager.h"
#include "PhysicsCCT.h"
#include "Transform.h"
#include "ComputeShader.h"
#include "GameObject.h"
#include "StructuredBuffer.h"

#include "GameInstance.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice) 
	, m_pDeviceContext(pDeviceContext)
{
	m_matPreTransform = Matrix::Identity;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CModel::CModel(const CModel& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_eType(rhs.m_eType)
	, m_matPreTransform(rhs.m_matPreTransform)
	, m_vecMeshes(rhs.m_vecMeshes)
	, m_vecMaterials(rhs.m_vecMaterials)
	, m_vecMaterialInstances(rhs.m_vecMaterialInstances)
	, m_umapAnimationIndexTable(rhs.m_umapAnimationIndexTable)
	, m_iRootBoneIdx(rhs.m_iRootBoneIdx)
	, m_bStageBones(rhs.m_bStageBones)
	, m_iStageBoneCounts(rhs.m_iStageBoneCounts)
	, m_pStaticModel_MinMax{rhs.m_pStaticModel_MinMax}
	, m_iFrameIndex(rhs.m_iFrameIndex)
	, m_iCpuBoneCount(rhs.m_iCpuBoneCount)
	, m_bLoopAnimDone(rhs.m_bLoopAnimDone)
	, m_fAnimationSpeed(rhs.m_fAnimationSpeed)
	, m_bMixAnim(rhs.m_bMixAnim)
	, m_iAdditivRef_AnimIdx(rhs.m_iAdditivRef_AnimIdx)
	, m_iAdditivePos_AnimIdx(rhs.m_iAdditivePos_AnimIdx)
	, m_fAdditiveOffset(rhs.m_fAdditiveOffset)
	, m_arrRagdollBoneDesc(rhs.m_arrRagdollBoneDesc)
	, m_vPreMainPosition(rhs.m_vPreMainPosition)
	, m_vPreMixPosition(rhs.m_vPreMixPosition)
	, m_vPreBlendPosition(rhs.m_vPreBlendPosition)
	, m_vPrePosNon(rhs.m_vPrePosNon)
	, m_bRagDollOn(rhs.m_bRagDollOn)
	, m_eAnim_UpdateState(rhs.m_eAnim_UpdateState)
	, m_vPreQuat(rhs.m_vPreQuat)
	, m_vPreQuatMix(rhs.m_vPreQuatMix)
	, m_vPreQuatNon(rhs.m_vPreQuatNon)
	, m_vPreBlendQuat(rhs.m_vPreBlendQuat)
	, m_bMoveBone(rhs.m_bMoveBone)
	, m_tBoneMoveCB(rhs.m_tBoneMoveCB)
{
	m_vecPrevAnimationPose.resize(rhs.m_vecPrevAnimationPose.size());
	m_vecCurrAnimationPose.resize(rhs.m_vecCurrAnimationPose.size());

	//m_pBoneOuputStagingBuffer[0] = rhs.m_pBoneOuputStagingBuffer[0];
	//m_pBoneOuputStagingBuffer[1] = rhs.m_pBoneOuputStagingBuffer[1];

	m_vecStageBoneIndices = rhs.m_vecStageBoneIndices;

	m_vecAnimations.reserve(rhs.m_vecAnimations.size());
	for (auto& pAnimation : rhs.m_vecAnimations)
		m_vecAnimations.push_back(pAnimation->Clone());

	m_vecBones.reserve(rhs.m_vecBones.size());
	for (auto& pBone : rhs.m_vecBones)
		m_vecBones.push_back(pBone->Clone());

	for (auto& pMesh : m_vecMeshes)
		Safe_AddRef(pMesh);

	for (auto& pMaterial : m_vecMaterials)
		Safe_AddRef(pMaterial);

	for (auto& pMaterialInstance : m_vecMaterialInstances)
		Safe_AddRef(pMaterialInstance);
	
	if (m_eType == EModelType::ANIM)
	{
		m_vecBoneGroups.resize(rhs.m_vecBoneGroups.size());
		size_t i = 0;
		for (auto& pBoneGroup : rhs.m_vecBoneGroups)
		{
			//m_vecBoneGroups.push_back(pBoneGroup);

			m_vecBoneGroups[i].BoneIndices = pBoneGroup.BoneIndices;
			m_vecBoneGroups[i].pIndexBuffer = nullptr;
			m_vecBoneGroups[i].pInputGroupSB_SRV = nullptr;

			i++;
		}

		//if (m_bStageBones)
		//{
		//	Safe_AddRef(m_pBoneOuputStagingBuffer[0]);
		//	Safe_AddRef(m_pBoneOuputStagingBuffer[1]);
		//}
	}

	{
		m_tBoneMoveCB.iBoneNums = Get_BoneCount();
	}

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CModel::Initialize_Prototype(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	MODEL_ORIGIN_DESC* pDesc = static_cast<MODEL_ORIGIN_DESC*>(pArg);
	m_eType = pDesc->eType;
	if (pDesc->pMatPreTransform)
		m_matPreTransform = *pDesc->pMatPreTransform;

	HRESULT hr = { E_FAIL };
	switch (pDesc->eType)
	{
	case EModelType::NONANIM:
	{
		hr = Load_NonAnimModel(pDesc->wstrModelFolderName);
	} break;
	case EModelType::CUSTOMPARTS:
	{
		hr = Load_CustomPartsModel(pDesc->wstrModelFolderName);
	} break;
	case EModelType::ANIM:
	{
		hr = Load_AnimModel(pDesc->wstrModelFolderName, pDesc->pAniChannelData);
	} break;
	case EModelType::BONE:
	{
		hr = Load_OnlyBone(pDesc->wstrModelFolderName);
	} break;
	case EModelType::STATIC:
	{
		hr = Load_StaticModel(pDesc->wstrModelFolderName);
		hr = Ready_StaticModelMinMax();
	} break;
	default:
		return E_FAIL;
	}

	if (FAILED(hr))
		return E_FAIL;

	if (m_vecAnimations.size() > 0)
	{
		m_umapAnimationIndexTable.reserve(m_vecAnimations.size());
		Build_AnimationIndexTable();
	}
	m_vecPrevAnimationPose.resize(m_vecBones.size());
	m_vecCurrAnimationPose.resize(m_vecBones.size());
	for (size_t i = 0; i < m_vecBones.size(); ++i)
		m_vecBones[i]->Setup_BindPoseTransformMatrix(m_vecBones, m_matPreTransform);


	/* bone 정보 빼돌릴래? */
	{
		Flags FStageBone = pDesc->FStageBone;
		m_bStageBones = true;
		
		// 안 빼돌리고 싶다면 : return
		if (Engine_Utils::Has_Flag(FStageBone, STAGEING_BONE::SB_ZEROBONE))
		{
			m_bStageBones = false;
		}

		// 빼돌릴건데 모든 뼈를 빼돌리고 싶다면
		else if (Engine_Utils::Has_Flag(FStageBone, STAGEING_BONE::SB_ALLBONE))
		{
			pDesc->vecStageBoneIndices.reserve(Get_BoneCount());

			for (size_t i = 0; i < Get_BoneCount(); i++)
			{
				pDesc->vecStageBoneIndices.push_back((_uint)i);
			}
		}

		// staging 정보 생성
		//Make_Staging(pDesc);

		// 
		if (m_bStageBones)
		{
			// 재귀로 지정뼈 ~ 부모뼈 update on
			for (auto& pBondIdx : pDesc->vecStageBoneIndices)
			{
				Set_CpuBone(pBondIdx);
			}

			// animation에 있는 channel 돌면서 channel update on
			for (auto& pAnim : m_vecAnimations)
			{
				pAnim->Check_UpdateCpu(m_vecBones);
			}

		}

	}


	//for (size_t i = 0; i < m_vecAnimations.size(); i++)
	//{
	//	if (Get_AnimationName((_uint)i) == TEXT("Animation_PlayerMoon_Land_Inplace"))
	//		m_vecAnimations[i]->Set_ApplyRootMotion(false);
	//}

	Mapping_Ragdoll_Bone();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	size_t iMaterialCount = m_vecMaterials.size();
	m_vecPasses.resize(iMaterialCount, 0);
	m_vecMaterialInstances.resize(iMaterialCount);
	if (iMaterialCount > 0)
	{
		for (size_t i = 0; i < iMaterialCount; ++i)
		{
			if (FAILED(Change_MI((_uint)i, EMaterialInstanceType::Default)))
				return E_FAIL;
		}

		if (pArg) 
		{
			MODEL_COPY_DESC* pDesc = static_cast<MODEL_COPY_DESC*>(pArg);
			if (pDesc->spanMIs.size() <= 0)
				return S_OK;

			if (pDesc->spanMIs.size() != iMaterialCount)
				return E_FAIL;

			for (size_t i = 0; i < iMaterialCount; ++i)
			{
				if (FAILED(Change_MI((_uint)i, pDesc->spanMIs[i])))
					return E_FAIL;

				m_vecPasses[i] = pDesc->spanShaderPassesByMesh[i];
			}
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_StaticModelMinMax()
{
	if (m_vecMeshes.empty()) return E_FAIL;

	m_pStaticModel_MinMax = new Vec3[2]{Vec3(FLT_MAX,FLT_MAX,FLT_MAX),Vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};
	if (m_pStaticModel_MinMax == nullptr) return E_FAIL;

	const Vec3* pCurMinMax{ nullptr };

	for (auto& Mesh : m_vecMeshes)
	{
		pCurMinMax = Mesh->Get_MinMax();
		if (pCurMinMax == nullptr)
		{
			Safe_Delete_Array(m_pStaticModel_MinMax);
			return S_OK;
		}
		Engine_Utils::Merge_MinMax(pCurMinMax , m_pStaticModel_MinMax[MIN] , m_pStaticModel_MinMax[MAX]);
	}

	return S_OK;
}

void CModel::Set_CpuBone(_uint iBoneIdx)
{
	// 이미 true라면 stop
	if (m_vecBones[iBoneIdx]->Get_IsUpdateCpu())
		return;

	m_vecBones[iBoneIdx]->Set_UpdateCpu(true);

	// 부모가 -1 전까지 확인
	_int iParentIdx = (m_vecBones[iBoneIdx]->Get_ParentIndex());
	if (iParentIdx > 0)
		Set_CpuBone(iParentIdx);

	m_iCpuBoneCount++;
}

HRESULT CModel::Change_Animation(CComputeShader* pAnimEComShader, const wstring& wstrName, _bool bBlend, _bool isLoop, _bool bForce)
{
	if (pAnimEComShader == nullptr)
		return E_FAIL;

	_int iAnimIdx = Get_AnimationIndex(wstrName);

	if (iAnimIdx < 0)
		return E_FAIL;

	if (m_iCurrentAnimIndex == iAnimIdx && bForce == false)
		return S_OK;

	if (bBlend)
	{
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		Change_AnimationPlayState(AnimationPlayState::BLEND, nullptr, iAnimIdx,false);
	}
	else
		Change_AnimationPlayState(AnimationPlayState::PLAY, pAnimEComShader, iAnimIdx);

	m_iCurrentAnimIndex = iAnimIdx;
	m_vecAnimations[m_iCurrentAnimIndex]->Clear();
	m_isAnimLoop = isLoop;

	return S_OK;
}

HRESULT CModel::Change_Animation(CComputeShader* pAnimEComShader, _uint iAnimationIndex, _bool bBlend, _bool isLoop, _bool bForce)
{
	if (m_iCurrentAnimIndex == iAnimationIndex && bForce == false)
		return S_OK;

	if (bBlend)
	{
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		Change_AnimationPlayState(AnimationPlayState::BLEND,nullptr, iAnimationIndex,false);
	}
	else
		Change_AnimationPlayState(AnimationPlayState::PLAY, pAnimEComShader, iAnimationIndex);

	m_iCurrentAnimIndex = iAnimationIndex;
	m_vecAnimations[m_iCurrentAnimIndex]->Clear();
	m_isAnimLoop = isLoop;

	return S_OK;
}

void CModel::Update_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEComShader, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimBlendCS, CComputeShader* pAnimMixCS, CComputeShader* pAdditiveCS, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	Update_AnimationPlayState(pBoneComBineCS, pAnimEComShader, pAnimBlendCS, fTimeDelta * m_fAnimationSpeed, pOwnerTransform, pOwnerPhyCCT, pAnimMixCS, pAdditiveCS, pRagDollCS, pBoneMoveCS);
}

void CModel::Update_PartModel(CComputeShader* pParentBoneComBineCS, CComputeShader* pChildBonePartCS)
{
	if (pParentBoneComBineCS && pChildBonePartCS)
	{
		// 보무 combine cs 결과를 child bone cs에 넘겨줌
		pChildBonePartCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_PARTBONE_IDX::MU_PARENTTRANSFORM),
			pChildBonePartCS->Get_SRV("PARENT_BONECOMBINED_TRANSFORMS"), pParentBoneComBineCS->Get_Output_Buffer());

		// dispatch
		_uint iGroupX = (Get_BoneCount() + 31) / 32;
		pChildBonePartCS->Dispatch(iGroupX, 1, 1);
	}
}

_bool CModel::Has_GhostTrailSnapshots() const
{
	return m_vecGhostSnapshots.empty() == false;
}

void CModel::Set_GhostTrailDesc(const GHOST_TRAIL_DESC& desc)
{
	m_tGhostTrail = desc;
}

void CModel::Enable_GhostTrail()
{
	Clear_GhostTrail();

	m_tGhostTrail = Init_GhostTrailDesc(m_tGhostTrail);
	m_bGhostActive = true;
	m_iGhostSpawnedCount = 0;
	m_fGhostAccTime = 0.f;
	m_vecGhostSnapshots.reserve(m_tGhostTrail.iMaxCount);
}

void CModel::Disable_GhostTrail()
{
	m_bGhostActive = false;
	m_fGhostAccTime = 0.f;
}

void CModel::Clear_GhostTrail()
{
	m_bGhostActive = false;
	m_iGhostSpawnedCount = 0;
	m_fGhostAccTime = 0.f;

	for (auto& s : m_vecGhostSnapshots)
		Safe_Release(s.pBoneBuffer);

	m_vecGhostSnapshots.clear();
}

void CModel::Set_GhostColor(const Vec4& vColor)
{
	m_tGhostTrail.vColor = vColor;
}

void CModel::Update_GhostTrail(_float fTimeDelta)
{
	if (m_vecGhostSnapshots.empty() && !m_bGhostActive)
		return;

	for (auto it = m_vecGhostSnapshots.begin(); it != m_vecGhostSnapshots.end(); )
	{
		it->fElapsed += fTimeDelta;

		const _float fLifeTime = (std::max)(g_XMEpsilon.f[0], it->fLifeTime);
		if (it->fElapsed >= fLifeTime)
		{
			Safe_Release(it->pBoneBuffer);
			it = m_vecGhostSnapshots.erase(it);
		}
		else
		{
			++it;
		}
	}

	m_fGhostTrailDelta = fTimeDelta;
}

void CModel::Capture_Ghsot(CComputeShader* pBoneCombineCS, const Matrix& matWorld)
{
	if (m_bGhostActive == false || !pBoneCombineCS)
		return;

	if (m_tGhostTrail.iMaxCount == 0)
		return;

	if (m_iGhostSpawnedCount >= m_tGhostTrail.iMaxCount)
	{
		m_bGhostActive = false;
		return;
	}

	m_fGhostAccTime += m_fGhostTrailDelta;
	if (m_fGhostAccTime < m_tGhostTrail.fInterval)
		return;

	m_fGhostAccTime -= m_tGhostTrail.fInterval;

	StructuredBuffer* pSrc = pBoneCombineCS->Get_Output_Buffer();
	if (pSrc == nullptr)
		return;

	StructuredBuffer* pCopy = StructuredBuffer::Create(
		m_pDevice, m_pDeviceContext,
		pSrc->m_iElementSize,
		pSrc->m_iElementCount);

	pCopy->CopyFrom(pSrc);

	GHOST_SNAPSHOT tSnap;
	tSnap.pBoneBuffer = pCopy;
	tSnap.matWorld = matWorld;
	tSnap.fLifeTime = m_tGhostTrail.fLifeTime;
	tSnap.fElapsed = 0.f;

	m_vecGhostSnapshots.push_back(std::move(tSnap));
	++m_iGhostSpawnedCount;

	if (m_iGhostSpawnedCount >= m_tGhostTrail.iMaxCount)
		m_bGhostActive = false;
}

HRESULT CModel::Render_GhostTrail(CShader* pShader, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS, _uint iGhostPass)
{
	if (m_vecGhostSnapshots.empty())
		return S_OK;

	if (pShader == nullptr || pBoneMeshCS == nullptr || pBoneCombineCS == nullptr)
		return E_FAIL;

	// 원본 백업
	StructuredBuffer* pOriginal = pBoneCombineCS->Get_Output_Buffer();
	if (pOriginal == nullptr)
		return E_FAIL;
	_uint iOriginPassIndex = pShader->Get_CurrentPass();
	
	pShader->Set_Pass(iGhostPass);
	for (auto& tSnap : m_vecGhostSnapshots)
	{
		if (tSnap.pBoneBuffer == nullptr)
			continue;

		const _float fLifeTime = (std::max)(g_XMEpsilon.f[0], tSnap.fLifeTime);
		const _float fT = (std::min)(1.f, tSnap.fElapsed / fLifeTime);

		// 선형보다 약간 더 부드럽게 사라지게
		const _float fAlpha = 1.f - fT * fT;

		SHADER_GHOST_TRAIL trailDesc{};
		trailDesc.vColor = m_tGhostTrail.vColor;
		trailDesc.vColor.w *= (std::max)(0.f, fAlpha);

		pShader->Bind_GhostTrail(trailDesc);
		pShader->Bind_TransformData(tSnap.matWorld);

		pBoneCombineCS->Set_OutputStructuredBuffer(tSnap.pBoneBuffer);

		for (_uint i = 0; i < Get_MeshCount(); ++i)
		{
			// 전제:
			// Bind_Bones 내부가 snapshot output buffer를 덮어쓰지 않아야 함
			Bind_Bones(pShader, i, pBoneMeshCS, pBoneCombineCS);

			pShader->Apply();
			m_vecMeshes[i]->Bind_Resource();
			m_vecMeshes[i]->Render();
		}
	}

	// 원본 복원
	pBoneCombineCS->Set_OutputStructuredBuffer(pOriginal);
	pShader->Set_Pass(iOriginPassIndex);
	return S_OK;
}

HRESULT CModel::Set_PassByMesh(class CShader* pShader, _uint iMeshIndex)
{
	if (pShader == nullptr || iMeshIndex >= m_vecMeshes.size())
		return E_FAIL;

	pShader->Set_Pass(m_vecPasses[iMeshIndex]);
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (FAILED(m_vecMeshes[iMeshIndex]->Bind_Resource()))
		return E_FAIL;

	m_vecMeshes[iMeshIndex]->Render();
	return S_OK;
}

HRESULT CModel::Render_Instance(_uint iMeshIndex, _uint iInstanceCount)
{
	if (FAILED(m_vecMeshes[iMeshIndex]->Bind_Resource()))
		return E_FAIL;

	m_vecMeshes[iMeshIndex]->Render_Instance(iInstanceCount);

	return S_OK;
}

HRESULT CModel::Bind_Material(class CShader* pShader, _uint iMeshIndex)
{
	if (iMeshIndex >= m_vecMeshes.size())
		return E_FAIL;

	return m_vecMaterials[m_vecMeshes[iMeshIndex]->Get_MaterialIndex()]->Bind_ShaderResource(pShader);
}



HRESULT CModel::Bind_MaterialInstance(CShader* pShader, _uint iMeshIndex)
{
	if (iMeshIndex >= m_vecMaterialInstances.size())
		return S_OK;

	return m_vecMaterialInstances[m_vecMeshes[iMeshIndex]->Get_MaterialIndex()]->Bind_ShaderResource(pShader);
}

HRESULT CModel::Bind_Bones(CShader* pShader, _uint iMeshIndex, CComputeShader* pBoneMeshCS, CComputeShader* pBoneCombineCS, _uint iIndexDistance)
{
	if (iMeshIndex >= m_vecMeshes.size())
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Bind_Bones(pShader, pBoneMeshCS, pBoneCombineCS, Get_BoneCount(), iIndexDistance);
}

void CModel::Set_RootBone(_int iRootIdx)
{
	// model 정보 엄데이트
	m_iRootBoneIdx = iRootIdx;

	// bone 정보 업데이트
	for (auto& pBone : m_vecBones)
	{
		pBone->Set_MotionBone(iRootIdx);
	}

	// animation, channel 정보 업데이트
	for (auto& pAnim : m_vecAnimations)
	{
		pAnim->Set_MotionBone(iRootIdx);
	}
}

void CModel::Set_Animtion_MotionOffset(_uint iAnimIdx, _float fOffset)
{
	if (iAnimIdx >= Get_AnimationCount())
		return;

	m_vecAnimations[(size_t)iAnimIdx]->Set_MotionOffset(fOffset);
}

_float CModel::Get_Animatioin_MotionOffset(_uint iAnimIdx)
{
	if (iAnimIdx >= Get_AnimationCount())
		return -1.f;

	return m_vecAnimations[(size_t)iAnimIdx]->Get_MotionOffset();
}

void CModel::Set_Animation_Speed(_uint iAnimIdx, _float fSpeed)
{
	if (iAnimIdx >= Get_AnimationCount())
		return;

	m_vecAnimations[(size_t)iAnimIdx]->Set_AnimationSpeed(fSpeed);
}

void CModel::Set_MixAnim(_bool bMix)
{
	m_bMixAnim = bMix;

	// 껐다 킬때, track 정리
	for (auto pMixIdx : m_vecMixAnimIndices)
	{
		if (pMixIdx > 0)
		{
			m_vecAnimations[pMixIdx]->Reset_PrePosition(m_vPreMixPosition, m_vPreQuatMix);
			m_vecAnimations[pMixIdx]->Set_TrackPosition(0.f);
		}
	}
}

void CModel::Set_AdditiveData(_bool bAdditive, _int iRefIdx, _int iPosIdx, _float fMixOffset)
{
	m_bAdditiveAnim			= bAdditive;

	m_iAdditivRef_AnimIdx	= iRefIdx;
	m_iAdditivePos_AnimIdx	= iPosIdx;

	m_fAdditiveOffset		= fMixOffset;
}

void CModel::Set_AdditiveData(_bool bAdditive, _int iPosIdx, _float fMixOffset)
{
	m_bAdditiveAnim			= bAdditive;

	m_iAdditivePos_AnimIdx	= iPosIdx;

	m_fAdditiveOffset		= fMixOffset;
}

void CModel::Make_MixRatio(_uint iAnimIdx, vector<DATA_ANIMIX>& vecAniMixData, CComputeShader* pAnimMixCS)
{
	// 1. 인덱스 방어
	if (iAnimIdx >= Get_AnimationCount())
		return;

	// 넘겨줄 vector를 만들어줌
	vector<_float> vecRatios;
	vecRatios.resize(m_vecBones.size(), 0.f);

	for (auto& pMixData : vecAniMixData)
	{
		// 음수라면 전체 적용
		if (pMixData.iParentIdx < 0)
		{
			for (size_t i = 0; i < m_vecBones.size(); i++)
			{
				vecRatios[i] = pMixData.fRatio;
			}
			continue;
		}

		// 양수라면 해당 인덱스의 자식 계층구조에 포함하는지 검사
		for (size_t i = 0 ; i< m_vecBones.size(); i++)
		{
			// 만약 내가 저 인덱스의 child라면
			if(m_vecBones[i]->Get_IsChildBone(m_vecBones, pMixData.iParentIdx))
				vecRatios[i] = pMixData.fRatio;
		}

		// 내꺼 추가 할래 말래
		if (pMixData.bInClude)
		{
			vecRatios[size_t(pMixData.iParentIdx)] = pMixData.fRatio;
		}
	}

	m_vecAnimations[(size_t)iAnimIdx]->Set_MixRatio(vecRatios, pAnimMixCS);
}

void CModel::Set_MixAnim_ResetSize(_uint iSize)
{
	m_vecMixAnimIndices.resize(size_t(iSize), -1);
}

void CModel::Set_MixAnim_AnimIndex(_uint iVectorIdx, _int iAnimIdx)
{
	size_t idx = size_t(iVectorIdx);


	if (idx < m_vecMixAnimIndices.size())
	{
		// 만약 원래 있던걸 지우는 거면 원래 담겨져 있던건 셋팅 리셋해주고
		if (iAnimIdx < 0 && m_vecMixAnimIndices[idx] >= 0 )
		{
			 m_vecAnimations[m_vecMixAnimIndices[idx]]->Reset_PrePosition(m_vPreMixPosition, m_vPreQuatMix);
			 m_vecAnimations[m_vecMixAnimIndices[idx]]->Set_TrackPosition(0.f);
		}

		// anim idx 바인딩
		m_vecMixAnimIndices[idx] = iAnimIdx;
	}
}

HRESULT CModel::Change_ShaderPassByMseh(_uint iMeshIndex, _uint iPass)
{
	if (iMeshIndex >= m_vecMeshes.size() || iPass < 0)
		return E_FAIL;

	m_vecPasses[iMeshIndex] = iPass;
	return S_OK;
}

HRESULT CModel::Set_DefaultPassByMesh(_uint iMeshIndex)
{
	if (iMeshIndex >= m_vecMeshes.size())
		return E_FAIL;

	m_vecPasses[iMeshIndex] = 0;
	return S_OK;
}

_int CModel::Get_PassByMesh(_uint iMeshIndex)
{
	if (iMeshIndex >= m_vecMeshes.size())
		return -1;

	return m_vecPasses[iMeshIndex];
}

HRESULT CModel::Change_MI(_uint iIndex, EMaterialInstanceType eChangeType)
{
	if (iIndex >= m_vecMaterialInstances.size())
		return E_FAIL;

	CMaterialInstance* pMI = m_pGameInstance->Get_Resource<CMaterialInstance>( Engine_Utils::MI_ToWString(eChangeType) );
	Safe_Release(m_vecMaterialInstances[iIndex]);
	m_vecMaterialInstances[iIndex] = pMI;

	return S_OK;
}

HRESULT CModel::Set_DefaultMI(_uint iIndex)
{
	if (iIndex >= m_vecMaterialInstances.size())
		return E_FAIL;

	CMaterialInstance* pMI = m_pGameInstance->Get_Resource<CMaterialInstance>(L"MaterialInstance_Default");
	Safe_Release(m_vecMaterialInstances[iIndex]);
	m_vecMaterialInstances[iIndex] = pMI;
	return S_OK;
}

HRESULT CModel::Change_Material(_uint iIndex, const wstring& wstrMaterialTag)
{
	if (iIndex >= m_vecMaterials.size())
		return E_FAIL;

	CMaterial* pMaterial = { nullptr };
	if (!(pMaterial = m_pGameInstance->Get_Resource<CMaterial>(wstrMaterialTag)))
		return E_FAIL;
	Safe_Release(m_vecMaterials[iIndex]);
	m_vecMaterials[iIndex] = pMaterial;
	return S_OK;
}

CMaterialInstance* CModel::Get_MaterialInstance(_uint iIndex)
{
	if (iIndex >= m_vecMaterialInstances.size())
		return nullptr;

	return m_vecMaterialInstances[iIndex];
}
CModelAnimation* CModel::Get_Animation(_uint iIndex)
{
	if (iIndex >= m_vecAnimations.size())
		return nullptr;

	return m_vecAnimations[iIndex];
}
CModelAnimation* CModel::Get_CurrentAnimation()
{
	return m_vecAnimations[m_iCurrentAnimIndex];
}
CMesh* CModel::Get_Mesh(_uint iIndex)
{
	if (iIndex >= m_vecMeshes.size())
		return nullptr;

	return m_vecMeshes[iIndex];
}

_int CModel::Get_BoneIndex(const _char* szName)
{
	CBone* pBone = Get_Bone(szName);
	if (!pBone)
		return -1;
	return pBone->Get_Index();
}

CBone* CModel::Get_Bone(const _char* szName)
{
	auto itr = std::find_if(m_vecBones.begin(), m_vecBones.end(),
		[szName](CBone *pBone)->bool
		{
			return pBone->Compare_Name(szName);
		});

	if (itr == m_vecBones.end())
		return nullptr;

	return *itr;
}

CBone* CModel::Get_Bone(_uint iIndex)
{
	if (iIndex >= m_vecBones.size())
		return nullptr;

	return m_vecBones[iIndex];
}

_float CModel::Get_AnimDurationTime() const
{
	return  m_vecAnimations[m_iCurrentAnimIndex]->Get_DurationTime();
}

_float CModel::Get_AnimNormalizedTime() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_NormalizedTime();
}

_float CModel::Get_AnimElpasedTimeSeconds() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_ElpasedTimeSeconds();
}

_float CModel::Get_AnimTickPerSecond() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_TickPerSecond();
}

void CModel::Set_AnimTickPerSecond(_float fValue)
{
	m_vecAnimations[m_iCurrentAnimIndex]->Set_TickPerSecond(fValue);
}

_int CModel::Get_CurrentAnimationIndex() const
{
	return m_iCurrentAnimIndex;
}

wstring CModel::Get_CurrentAnimationName() const
{
	if (m_iCurrentAnimIndex < 0 || m_iCurrentAnimIndex >= m_vecAnimations.size())
		return L"NULL";
		
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_Name();
}

_float CModel::Get_AnimTrackPosition() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_TrackPosition();
}

_bool CModel::Is_AnimTrackPositionBetween(_float fStartRatio, _float fEndRatio)
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionBetween(fStartRatio, fEndRatio);
}

_bool CModel::Is_AnimTrackPositionBetweenRaw(_float fTrackA, _float fTrackB)
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionBetweenRaw(fTrackA, fTrackB);
}

_bool CModel::Is_AnimTrackPositionAt(_float fRatio)
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionAt(fRatio);
}

_bool CModel::Is_AnimTrackPositionAtRaw(_float fTrackPosition)
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionAtRaw(fTrackPosition);
}

_bool CModel::Is_AnimTrackPositionAtHalf() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionAtHalf();
}

_bool CModel::Is_RootMotion_Apply() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Get_ApplyRoot();
}

_int CModel::Get_AnimationIndex(const wstring& wstrName)
{
	if (wstrName.empty())
		return -1;

	auto itr = m_umapAnimationIndexTable.find(wstrName);
	if (itr == m_umapAnimationIndexTable.end())
		return -1;

	return static_cast<_int>(itr->second);
}

wstring CModel::Get_MaterialName(_uint iIndex) const
{
	if (iIndex >= m_vecMaterials.size())
		return L"";

	return wstring(m_vecMaterials[iIndex]->Get_Name());
}

_wstring CModel::Get_AnimationName(_uint iAniIndex) const
{
	if (iAniIndex >= m_vecAnimations.size())
		return L"";

	return wstring(m_vecAnimations[iAniIndex]->Get_Name());
}

void CModel::Set_AnimationPlayRate(_uint iIndex, _float fValue)
{
	if (m_vecAnimations[iIndex])
		m_vecAnimations[iIndex]->Set_PlayRate(fValue);
}

void CModel::Set_CurAnimation_RootApply(_bool bRootApply)
{
	m_vecAnimations[m_iCurrentAnimIndex]->Set_ApplyRootMotion(bRootApply);
}

void CModel::Set_ApplyRootMotionAll(_bool bRootApply)
{
	for (auto& pAnim : m_vecAnimations)
	{
		pAnim->Set_ApplyRootMotion(bRootApply);
	}
}

void CModel::Set_Animtion_MotionOffset_All(_float fOffset)
{
	for (auto& pAnim : m_vecAnimations)
	{
		pAnim->Set_MotionOffset(fOffset);
	}
}

void CModel::Set_Animation_SpeedOffset_All(_float fOffset)
{
	for (auto& pAnim : m_vecAnimations)
	{
		pAnim->Set_AnimationSpeed(fOffset);
	}
}

HRESULT CModel::Set_MI(_uint iIndex, const SHADER_MI_DESC& tDesc)
{
	if (iIndex >= m_vecMaterialInstances.size()) return E_FAIL;

	if (m_vecMaterialInstances[iIndex]->Get_MIType() != EMaterialInstanceType::Free)
	{
		MSG_BOX(" Material Instance Type이 Free가 아니라면 Setting 불가능 ");
	}

	m_vecMaterialInstances[iIndex]->Set_MI(tDesc);

	return S_OK;
}

HRESULT CModel::Set_MI_TintColor(_uint iIndex, const Vec4& vColor)
{
	if (iIndex >= m_vecMaterialInstances.size()) return E_FAIL;

	if (m_vecMaterialInstances[iIndex]->Get_MIType() != EMaterialInstanceType::Free)
	{
		MSG_BOX(" Material Instance Type이 Free가 아니라면 Setting 불가능 ");
	}

	m_vecMaterialInstances[iIndex]->Set_TintColor(vColor);

	return S_OK;
}

HRESULT CModel::Set_MI_EmissiveColor(_uint iIndex, const Vec4& vColor)
{
	if (iIndex >= m_vecMaterialInstances.size()) return E_FAIL;

	if (m_vecMaterialInstances[iIndex]->Get_MIType() != EMaterialInstanceType::Free)
	{
		MSG_BOX(" Material Instance Type이 Free가 아니라면 Setting 불가능 ");
	}

	m_vecMaterialInstances[iIndex]->Set_EmissiveColor(vColor);

	return S_OK;
}

HRESULT CModel::Set_MI_EmissivePower(_uint iIndex, const float& fPower)
{
	if (iIndex >= m_vecMaterialInstances.size()) return E_FAIL;

	if (m_vecMaterialInstances[iIndex]->Get_MIType() != EMaterialInstanceType::Free)
	{
		MSG_BOX(" Material Instance Type이 Free가 아니라면 Setting 불가능 ");
	}

	m_vecMaterialInstances[iIndex]->Set_EmissivePower(fPower);

	return S_OK;
}

void CModel::Set_AnimTrackPosition(_float fValue)
{
	m_vecAnimations[m_iCurrentAnimIndex]->Set_TrackPosition(fValue);
}

HRESULT CModel::Load_StaticModel(const wstring& wstrModelName)
{
	CModelLoader* pModelLoader = CModelLoader::Create(m_pDevice, m_pDeviceContext, wstrModelName.c_str());

	if (FAILED(pModelLoader->Read_Model(m_eType, nullptr, &m_vecMeshes)))
		return E_FAIL;
	if (FAILED(pModelLoader->Read_Material(&m_vecMaterials)))
		return E_FAIL;

	Safe_Release(pModelLoader);
	return S_OK;
}

HRESULT CModel::Load_NonAnimModel(const wstring& wstrModelName)
{
	CModelLoader* pModelLoader = CModelLoader::Create(m_pDevice, m_pDeviceContext, wstrModelName.c_str());

	if (FAILED(pModelLoader->Read_Model(m_eType, &m_vecBones, &m_vecMeshes)))
		return E_FAIL;
	if (FAILED(pModelLoader->Read_Material(&m_vecMaterials)))
		return E_FAIL;
	
	Safe_Release(pModelLoader);
	return S_OK;
}

HRESULT CModel::Load_AnimModel(const wstring& wstrModelName, DATA_ANIMCHANNEL* pData)
{
	CModelLoader* pModelLoader = CModelLoader::Create(m_pDevice, m_pDeviceContext, wstrModelName.c_str());
	
	if (FAILED(pModelLoader->Read_Model(m_eType, &m_vecBones, &m_vecMeshes, pData)))
		return E_FAIL;
	if (FAILED(pModelLoader->Read_Material(&m_vecMaterials)))
		return E_FAIL;
	if (FAILED(pModelLoader->Read_Animation(&m_vecAnimations, pData)))
		return E_FAIL;

	if (pData)
	{
		m_iRootBoneIdx = pData->iRootBoneIndex;
	}

	Make_BoneGroup();		// bone을 그룹별로 만든다

	Safe_Release(pModelLoader);
	return S_OK;
}

HRESULT CModel::Load_CustomPartsModel(const wstring& wstrModelName)
{
	CModelLoader* pModelLoader = CModelLoader::Create(m_pDevice, m_pDeviceContext, wstrModelName.c_str());

	if (FAILED(pModelLoader->Read_Model(EModelType::ANIM, &m_vecBones, &m_vecMeshes)))
		return E_FAIL;
	if (FAILED(pModelLoader->Read_Material(&m_vecMaterials)))
		return E_FAIL;

	Safe_Release(pModelLoader);
	return S_OK;
}

HRESULT CModel::Load_OnlyBone(const wstring& wstrModelName)
{
	CModelLoader* pModelLoader = CModelLoader::Create(m_pDevice, m_pDeviceContext, wstrModelName.c_str());

	if (FAILED(pModelLoader->Read_Model(m_eType, &m_vecBones, nullptr)))
		return E_FAIL;

	Safe_Release(pModelLoader);
	return S_OK;
}

HRESULT CModel::Add_Parts(const wstring& wstrPrototypeTag)
{
	CModel* pPart = { nullptr };
	size_t iMaterialIndexDistance = { m_vecMaterials.size() };
	if (!(pPart = Get_Clone(wstrPrototypeTag)))
		return E_FAIL;

	for (size_t i = 0; i < pPart->m_vecMeshes.size(); ++i)
	{
		CMesh* pClone = static_cast<CMesh*>(pPart->m_vecMeshes[i]->Clone(nullptr));
		pClone->Set_MaterialIndex(pClone->Get_MaterialIndex() + static_cast<_uint>(iMaterialIndexDistance));
		m_vecMeshes.push_back(pClone);
	}

	for (size_t i = 0; i < pPart->m_vecMaterials.size(); ++i)
	{
		m_vecMaterials.push_back(m_pGameInstance->Get_Resource<CMaterial>(pPart->m_vecMaterials[i]->Get_Name()));
	}

	Safe_Release(pPart);
	return S_OK;
}
 
CModel* CModel::Get_Clone(const wstring& wstrPrototypeTag)
{
	return dynamic_cast<CModel*>(m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, 0/* static */, wstrPrototypeTag));
}

HRESULT CModel::Build_AnimationIndexTable()
{
	for (size_t i = 0; i < m_vecAnimations.size(); ++i)
	{
		const wstring& wstrAnimName = m_vecAnimations[i]->Get_Name();
		if (!wstrAnimName.empty())
		{
			auto [itr, inserted] = m_umapAnimationIndexTable.emplace(wstrAnimName, i);
			if (!inserted)
			{
				MSG_BOX("CModel::Build_AnimationIndexTable, Animation name was duplicated");
				return E_FAIL;
			}
		}
		else
		{
			MSG_BOX("CModel::Build_AnimationIndexTable, Animation name invalid");
			return E_FAIL;
		}
	}

	return S_OK;
}

void CModel::Begin_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS, _uint iAnimationIndex, _bool bChannelReset)
{
	switch (eState)
	{
	case Engine::CModel::PLAY:
		Play_Begin(pAnimEvalCS, iAnimationIndex, bChannelReset);
		break;
	case Engine::CModel::BLEND:
		Blend_Begin(iAnimationIndex);
		break;
	}
}

void CModel::Update_AnimationPlayState(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimMixCS, CComputeShader* pAdditive, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	switch (m_eCurrentAnimationState)
	{
	case Engine::CModel::PLAY:
		Play_Update(pBoneComBineCS, pAnimEvalCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, pAnimMixCS, pAdditive, pRagDollCS, pBoneMoveCS);
		break;
	case Engine::CModel::BLEND:
		if(pAnimBlendCS)
			Blend_Update(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, pAnimMixCS, pAdditive, pRagDollCS, pBoneMoveCS);
		break;
	}
}

void CModel::End_AnimationPlayState(AnimationPlayState eState, AnimationPlayState eNextState)
{
	switch (eState)
	{
	case Engine::CModel::PLAY:
		Play_End(eNextState);
		break;
	case Engine::CModel::BLEND:
		Blend_End();
		break;
	}
}

void CModel::Change_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS, _uint iAnimationIndex, _bool bChannelReset)
{
	End_AnimationPlayState(m_eCurrentAnimationState, eState);
	Begin_AnimationPlayState(eState, pAnimEvalCS, iAnimationIndex, bChannelReset);
	m_eCurrentAnimationState = eState;
}

void CModel::Play_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, _float fTimeDelta, CTransform* pOwnerTransform , CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimMixCS, CComputeShader* pAdditiveCS, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	// animation update
	{
		m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, m_bLoopAnimDone, fTimeDelta, m_isAnimLoop, pOwnerTransform, pOwnerPhyCCT, pAnimEvalCS, m_vPreMainPosition,m_vPreQuat);

		m_eAnim_UpdateState = AnimUpdateState::NORMAL;
	}

	// mix
	if (m_bMixAnim && !m_vecMixAnimIndices.empty())
	{
		Mix_Animation(pAnimMixCS, pAnimEvalCS, fTimeDelta);

		m_eAnim_UpdateState = AnimUpdateState::MIX;
	}

	// additive
	if (m_bAdditiveAnim && pAdditiveCS)
	{		
		_bool bAdditiveSuccess = false;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::NORMAL:
			bAdditiveSuccess = Additive_Animation(pAdditiveCS, pAnimEvalCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
			break;

		case  AnimUpdateState::MIX:
			bAdditiveSuccess = Additive_Animation(pAdditiveCS, pAnimMixCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
			break;
		}

		if(bAdditiveSuccess)
			m_eAnim_UpdateState = AnimUpdateState::ADDITIVE;
	}

	// RagDoll
	if(m_bRagDollOn && pRagDollCS)
	{
		StructuredBuffer* pPrevSRT = nullptr;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::NORMAL:
			pPrevSRT = pAnimEvalCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pPrevSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pPrevSRT = pAdditiveCS->Get_Output_Buffer();
			break;
		}

		pRagDollCS->Get_Output_Buffer()->CopyFrom(pPrevSRT);

		// dispatch
		_uint iGroupX = (ENUM_TO_UINT(ERagdollJoint::END) + 31) / 32;
		pRagDollCS->Dispatch(iGroupX, 1, 1);

		m_eAnim_UpdateState = AnimUpdateState::RAGDOLL;
	}

	if (m_bMoveBone && pBoneMoveCS)
	{
		StructuredBuffer* pPrevSRT = nullptr;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::NORMAL:
			pPrevSRT = pAnimEvalCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pPrevSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pPrevSRT = pAdditiveCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::RAGDOLL:
			pPrevSRT = pRagDollCS->Get_Output_Buffer();
			break;
		}

		pBoneMoveCS->Get_Output_Buffer()->CopyFrom(pPrevSRT);
		pBoneMoveCS->Bind_Compute_BoneMoveCB(m_tBoneMoveCB);

		// dispatch
		_uint iGroupX = (Get_BoneCount() + 31) / 32;
		pBoneMoveCS->Dispatch(iGroupX, 1, 1);

		m_eAnim_UpdateState = AnimUpdateState::MYMOVE;
	}

	// combine에 값 바인딩
	{
		StructuredBuffer* pFinalSRT = { nullptr };
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::NORMAL:
			pFinalSRT = pAnimEvalCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pFinalSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pFinalSRT = pAdditiveCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::RAGDOLL:
			pFinalSRT = pRagDollCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MYMOVE:
			pFinalSRT = pBoneMoveCS->Get_Output_Buffer();
			break;
		}

		pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRTS), pBoneComBineCS->Get_SRV("MU_SRTS"), pFinalSRT);
	}

	// bone updatezd
	Update_BoneCombineTransformMatrix(pBoneComBineCS);
}

void CModel::Play_Begin(CComputeShader* pAnimEvalCS, _uint iAnimationIndex, _bool bChannelReset)
{
	if (pAnimEvalCS)
		m_vecAnimations[iAnimationIndex]->Bind_AnimationEData(pAnimEvalCS);

	m_vecAnimations[iAnimationIndex]->Reset_NotifyCursor();

	if (m_eCurrentAnimationState != BLEND || bChannelReset)
		m_vecAnimations[iAnimationIndex]->Reset_PrePosition(m_vPreMainPosition, m_vPreQuat);

	else
		int a = 0;
}

void CModel::Play_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimMixCS, CComputeShader* pAdditive, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	CModelAnimation* pAnimation = m_vecAnimations[m_iCurrentAnimIndex];
	m_fAnimPrevTrackPosition = pAnimation->Get_TrackPosition();
	
	Play_Animation(pBoneComBineCS, pAnimEvalCS, fTimeDelta,  pOwnerTransform ,  pOwnerPhyCCT, pAnimMixCS, pAdditive, pRagDollCS, pBoneMoveCS);

	_float fCurrentPosition = pAnimation->Get_TrackPosition();
	m_bLooped = m_isAnimLoop && (fCurrentPosition < m_fAnimPrevTrackPosition);
	Emit_Notifies(pAnimation, fCurrentPosition, EAnimNotifyPhase::Immediatley);
}

void CModel::Play_End(AnimationPlayState eNextState)
{
	m_bIsAnimFinished = false;

	//if (eNextState != BLEND)
	//	m_vecAnimations[m_iCurrentAnimIndex]->Reset_PrePosition(m_vPreMainPosition);

	//else
	//	int a = 0;
}

void CModel::Blend_Begin(_uint CurAnimationIndex)
{
	m_fBlendedTime = 0.f;



	m_vecAnimations[CurAnimationIndex]->Reset_NotifyCursor();
	//m_vecAnimations[CurAnimationIndex]->Reset_PrePosition(m_vPreBlendPosition); 

	//if (m_iRootBoneIdx >= 0)
	//{
	//	m_vPreMainPosition = m_vecBones[m_iRootBoneIdx]->Get_Transform().Translation();
	//}

	m_vPreBlendPosition = m_vPreMainPosition;
	m_vPreBlendQuat = m_vPreQuat;

	m_vecAnimations[CurAnimationIndex]->Reset_PrePosition(m_vPreMainPosition,m_vPreQuat);
}

void CModel::Blend_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimMixCS, CComputeShader* pAdditive, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	if (m_fBlendDuration <= 0.f)
	{
		Change_AnimationPlayState(PLAY, pAnimEvalCS, m_iCurrentAnimIndex, false);
		return;
	}

	CModelAnimation* pAnimation = m_vecAnimations[m_iCurrentAnimIndex];
	m_fAnimPrevTrackPosition = pAnimation->Get_TrackPosition();

	m_fBlendedTime += fTimeDelta;
	if (m_fBlendedTime < m_fBlendDuration)
	{
		_float fNormalizedTime = std::clamp(m_fBlendedTime / m_fBlendDuration, 0.f, 1.f);
		_float fRatio = fNormalizedTime * fNormalizedTime * (3.0f - 2.0f * fNormalizedTime);

		//if (pOwnerTransform)
			Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, pOwnerTransform, pOwnerPhyCCT, pAnimMixCS, pAdditive, pRagDollCS, pBoneMoveCS);
		//else
			//Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, pOwnerTransform, m_pOwner->Get_Component<CPhysicsCCT>(), pAnimMixCS, pAdditive);
			//Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>(), pAnimMixCS);
	}
	else
		Change_AnimationPlayState(PLAY, pAnimEvalCS, m_iCurrentAnimIndex, false);

	const _float fCurrentPosition = pAnimation->Get_TrackPosition();
	m_bLooped = (m_isAnimLoop && fCurrentPosition < m_fAnimPrevTrackPosition);
	Emit_Notifies(pAnimation, fCurrentPosition, EAnimNotifyPhase::Immediatley);
}

void CModel::Blend_End()
{
	m_fBlendedTime = 0.f;

	switch (m_iBlendRootType)
	{
	case 0: // 둘다 업데이트
		break;

	case 1: // pre만 업데이트
		m_vPreMainPosition = m_vPreBlendPosition;
		m_vPreQuat = m_vPreBlendQuat;
		break;

	case 2: // main만 업데이트
		break;
	}

	//m_vecAnimations[m_iPrevAnimIndex]->Reset_PrePosition(m_vPreBlendPosition);
}

void CModel::Make_BoneGroup()
{
	m_vecBoneGroups.clear();

	// 자신의 깊이 기록용 벡터
	vector<_int> boneDepth(m_vecBones.size(), 0); 

	// bone을 순회
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		_int iParent = m_vecBones[i]->Get_ParentIndex();

		// 부모의 깊이를 통해 내 깊이를 구한다
		{
			if (iParent >= 0)
				boneDepth[i] = boneDepth[iParent] + 1;
			else
				boneDepth[i] = 0;
		}

		// 깊이를 통해 boneGroup에 넣는다
		{
			if (m_vecBoneGroups.size() <= boneDepth[i])
				m_vecBoneGroups.resize(boneDepth[i] + 1);

			m_vecBoneGroups[boneDepth[i]].BoneIndices.push_back((_uint)i);
		}
	}
}

void CModel::Make_GroupBuffers()
{
	// 그룹을 순회
	for (size_t i = 0; i < m_vecBoneGroups.size(); i++)
	{
		_uint iGroupSize = _uint(m_vecBoneGroups[i].BoneIndices.size());

		// SB class 생성

		m_vecBoneGroups[i].pIndexBuffer = nullptr;
		m_vecBoneGroups[i].pIndexBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_MU_BONEIDX), iGroupSize);
	}
}

void CModel::Make_SB()
{
	Safe_Release(m_pPreSB); 
	Safe_Release(m_pCurSB);
	m_pPreSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_SRT), Get_BoneCount());
	m_pCurSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_SRT), Get_BoneCount());
}

void CModel::Make_Staging(MODEL_ORIGIN_DESC* pDesc)
{
	//m_iStageBoneCounts = (_uint)(pDesc->vecStageBoneIndices.size());

	//// 1. staging buffer 생성
	//D3D11_BUFFER_DESC desc = {};
	//desc.ByteWidth = sizeof(CS_OUT_BONE) * m_iStageBoneCounts;
	//desc.Usage = D3D11_USAGE_STAGING;
	//desc.BindFlags = 0;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	//m_pDevice->CreateBuffer(&desc, nullptr, &m_pBoneOuputStagingBuffer[0]);
	//m_pDevice->CreateBuffer(&desc, nullptr, &m_pBoneOuputStagingBuffer[1]);

	//// 2. bone indices 캐스팅 하고 있자
	//m_vecStageBoneIndices.reserve(m_iStageBoneCounts);
	//m_vecStageBoneIndices = pDesc->vecStageBoneIndices;
}

void CModel::Update_BoneCombineTransformMatrix(CComputeShader* pBoneComBineCS)
{
	if (pBoneComBineCS == nullptr)
		return;

	// todo : root motion 적용 여기서
	// 이전 프레임의 root 뼈의 position 과 현재 프레임의 root 뼈의 position을 가져와서

	// bone group 별로 디스패치를 한다
	for (auto& pBoneGroup : m_vecBoneGroups)
	{
		_uint iBoneNums = _uint(pBoneGroup.BoneIndices.size());

		// 가변 데이터 group idx 넘겨줌
		pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_GROUPIDX), pBoneGroup.pInputGroupSB_SRV, pBoneGroup.pIndexBuffer);

		// 가변 데이터 : GroupNum 작성
		CS_MU_GROUPNUMS tMuDesc{};
		tMuDesc.iGroupBoneNums = iBoneNums;
		pBoneComBineCS->Bind_Compute_BoneMuCB(tMuDesc);

		// dispatch
		_uint iGroupX = (iBoneNums + 31) / 32;
		pBoneComBineCS->Dispatch(iGroupX,1,1);
	}

	// cpu update
	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		m_vecBones[i]->Update_CombinedTransformMatrix(m_vecBones, m_matPreTransform);
	}
}

void CModel::Blend_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimMixCS, CComputeShader* pAdditiveCS, CComputeShader* pRagDollCS, CComputeShader* pBoneMoveCS)
{
	//if (pOwnerTransform)
	{
		// 1. 버퍼 빼돌리기
		StructuredBuffer* pOriginSB = pAnimEvalCS->Get_Output_Buffer();

		// 2. pre animation
		{
			// 내 버퍼로 받도록 설정
			pAnimEvalCS->Set_OutputStructuredBuffer(m_pPreSB);

			// channel 업데이트
			m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, nullptr, pOwnerPhyCCT, Get_BoneCount(), pAnimEvalCS, m_vPrePosNon, m_vPreQuatNon);

			// animation 결과 blendCS에 bind
			pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_PRESRT),
				pAnimBlendCS->Get_SRV("MU_PRETRANSFORMS"), m_pPreSB);
		}

		// 3. cur animation
		{
			// 내 버퍼로 받도록 설정
			pAnimEvalCS->Set_OutputStructuredBuffer(m_pCurSB);

			// channel 업데이트
			m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, nullptr, pOwnerPhyCCT, Get_BoneCount(), pAnimEvalCS, m_vPrePosNon, m_vPreQuatNon);

			// animation 결과 blendCS에 bind
			pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_CURSRT),
				pAnimBlendCS->Get_SRV("MU_CURTRANSFORMS"), m_pCurSB);
		}

		// 4. 버퍼 돌려놓기
		pAnimEvalCS->Set_OutputStructuredBuffer(pOriginSB);

		m_eAnim_UpdateState = AnimUpdateState::NORMAL;

	}

	// animation 2개를 lerp
	{
		Lerp_Animation(pAnimBlendCS, fRatio, pOwnerTransform, pOwnerPhyCCT);

		m_eAnim_UpdateState = AnimUpdateState::BLEND;
	}


	// mix
	if (m_bMixAnim && !m_vecMixAnimIndices.empty())
	{
		Mix_Animation(pAnimMixCS, pAnimBlendCS, fTimeDelta);

		m_eAnim_UpdateState = AnimUpdateState::MIX;
	}

	// additive
	if (m_bAdditiveAnim && pAdditiveCS)
	{
		_bool bAdditiveSuccess = false;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::BLEND:
			bAdditiveSuccess = Additive_Animation(pAdditiveCS, pAnimBlendCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
			break;

		case  AnimUpdateState::MIX:
			bAdditiveSuccess = Additive_Animation(pAdditiveCS, pAnimMixCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
			break;
		}

		if (bAdditiveSuccess)
			m_eAnim_UpdateState = AnimUpdateState::ADDITIVE;
	}

	// RagDoll
	if (m_bRagDollOn && pRagDollCS)
	{
		StructuredBuffer* pPrevSRT = nullptr;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::BLEND:
			pPrevSRT = pAnimBlendCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pPrevSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pPrevSRT = pAdditiveCS->Get_Output_Buffer();
			break;
		}

		pRagDollCS->Get_Output_Buffer()->CopyFrom(pPrevSRT);

		// dispatch
		_uint iGroupX = (ENUM_TO_UINT(ERagdollJoint::END) + 31) / 32;
		pRagDollCS->Dispatch(iGroupX, 1, 1);

		m_eAnim_UpdateState = AnimUpdateState::RAGDOLL;
	}

	if (m_bMoveBone && pBoneMoveCS)
	{
		StructuredBuffer* pPrevSRT = nullptr;
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::BLEND:
			pPrevSRT = pAnimBlendCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pPrevSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pPrevSRT = pAdditiveCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::RAGDOLL:
			pPrevSRT = pRagDollCS->Get_Output_Buffer();
			break;
		}

		pBoneMoveCS->Get_Output_Buffer()->CopyFrom(pPrevSRT);
		pBoneMoveCS->Bind_Compute_BoneMoveCB(m_tBoneMoveCB);

		// dispatch
		_uint iGroupX = (Get_BoneCount() + 31) / 32;
		pBoneMoveCS->Dispatch(iGroupX, 1, 1);

		m_eAnim_UpdateState = AnimUpdateState::MYMOVE;
	}

	// combine에 값 바인딩
	{
		StructuredBuffer* pFinalSRT = { nullptr };
		switch (m_eAnim_UpdateState)
		{
		case  AnimUpdateState::BLEND:
			pFinalSRT = pAnimBlendCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MIX:
			pFinalSRT = pAnimMixCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::ADDITIVE:
			pFinalSRT = pAdditiveCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::RAGDOLL:
			pFinalSRT = pRagDollCS->Get_Output_Buffer();
			break;

		case  AnimUpdateState::MYMOVE:
			pFinalSRT = pBoneMoveCS->Get_Output_Buffer();
			break;
		}

		pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRTS), pBoneComBineCS->Get_SRV("MU_SRTS"), pFinalSRT);
	}

	// bone updatezd
	Update_BoneCombineTransformMatrix(pBoneComBineCS);

	//if (m_bStageBones)
	//	DisPatch_BondMatrix(pBoneComBineCS, pAnimMixCS);
}

void CModel::Lerp_Animation(CComputeShader* pAnimBlendCS, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	// 가변 데이터 작성
	CS_MU_ANIMB tMuDesc{};
	tMuDesc.fRatio = fRatio;
	tMuDesc.iRootMotionBoneIndex = m_iRootBoneIdx;
	tMuDesc.iBoneCount = Get_BoneCount();

	pAnimBlendCS->Bind_Compute_BlendMu(tMuDesc);

	// dispatch
	_uint iGroupX = (Get_BoneCount() + 31) / 32;
	pAnimBlendCS->Dispatch(iGroupX, 1, 1);


	// cpu update
	_uint i = {};
	for (auto& pBone : m_vecBones)
	{
		if (pBone->Get_IsUpdateCpu())
		{
			Matrix matTransformation = Matrix::Identity;
			Vec3 vScale = {};
			Quat vQuaternion = {};
			Vec3 vTranslation = {};

			vScale = Vec3::Lerp(m_vecPrevAnimationPose[i].vScale, m_vecCurrAnimationPose[i].vScale, fRatio);
			vQuaternion = Quat::Slerp(m_vecPrevAnimationPose[i].vQuaterion, m_vecCurrAnimationPose[i].vQuaterion, fRatio);
			vTranslation = Vec3::Lerp(m_vecPrevAnimationPose[i].vTranslation, m_vecCurrAnimationPose[i].vTranslation, fRatio);

			//motion bone 일때 trans : zero로 해줌
			if (m_iRootBoneIdx == i)
			{
				if (pOwnerPhyCCT && pOwnerTransform)
				{
					Vec3 vDelta = { Vec3::Zero };
					Quat qDelta = { Quat::Identity };
					_float fMotionOffset = { 1.f };
					_float fYaw = { 0.f };
					if (m_vecAnimations[m_iCurrentAnimIndex]->Get_ApplyRoot() && m_vecAnimations[m_iPrevAnimIndex]->Get_ApplyRoot())
					{
						m_iBlendRootType = 0;

						/* Translation */
						{
							Vec3 vBlendvDelta = m_vPreBlendPosition - m_vecPrevAnimationPose[i].vTranslation;
							Vec3 vMainDelta = m_vPreMainPosition - m_vecCurrAnimationPose[i].vTranslation;
							m_vPreBlendPosition = m_vecPrevAnimationPose[i].vTranslation;
							m_vPreMainPosition = m_vecCurrAnimationPose[i].vTranslation;

							vDelta = Vec3::Lerp(vBlendvDelta, vMainDelta, fRatio);
							fMotionOffset = m_vecAnimations[m_iCurrentAnimIndex]->Get_MotionOffset();
						}


						/* Rotation */
						{
							Quat qBlendInv, qMainInv;
							m_vPreBlendQuat.Inverse(qBlendInv);
							m_vPreQuat.Inverse(qMainInv);

							Quat qBlendqDelta = Quat(m_vecPrevAnimationPose[i].vQuaterion) * qBlendInv;
							Quat qMainqDelta = Quat(m_vecCurrAnimationPose[i].vQuaterion) * qMainInv;
							m_vPreBlendQuat = Quat(m_vecPrevAnimationPose[i].vQuaterion);
							m_vPreQuat = Quat(m_vecCurrAnimationPose[i].vQuaterion);

							qDelta = Quat::Slerp(qBlendqDelta, qMainqDelta, fRatio);
							// Yaw(Y축)만 추출
							Vec3 vEuler = qDelta.ToEuler();   // DirectXMath 기준
							fYaw = vEuler.z;
						}
					}

					else if (m_vecAnimations[m_iPrevAnimIndex]->Get_ApplyRoot())
					{
						m_iBlendRootType = 1;

						/* Translation */
						{
							Vec3 vBlendvDelta = m_vPreBlendPosition - m_vecPrevAnimationPose[i].vTranslation;
							m_vPreBlendPosition = m_vecPrevAnimationPose[i].vTranslation;

							vDelta = vBlendvDelta;
							fMotionOffset = m_vecAnimations[m_iPrevAnimIndex]->Get_MotionOffset();
						}

						/* Rotation */
						{
							Quat qBlendInv;
							m_vPreBlendQuat.Inverse(qBlendInv);

							Quat qBlendqDelta = Quat(m_vecPrevAnimationPose[i].vQuaterion) * qBlendInv;
							m_vPreBlendQuat = Quat(m_vecPrevAnimationPose[i].vQuaterion);

							qDelta = qBlendqDelta;
							// Yaw(Y축)만 추출
							Vec3 vEuler = qDelta.ToEuler();   // DirectXMath 기준
							fYaw = vEuler.z;
						}

					}

					else if (m_vecAnimations[m_iCurrentAnimIndex]->Get_ApplyRoot())
					{
						m_iBlendRootType = 2;

						/* Translation*/
						{
							Vec3 vMainDelta = m_vPreMainPosition - m_vecCurrAnimationPose[i].vTranslation;
							m_vPreMainPosition = m_vecCurrAnimationPose[i].vTranslation;

							vDelta = vMainDelta;
							fMotionOffset = m_vecAnimations[m_iCurrentAnimIndex]->Get_MotionOffset();
						}


						/* Rotation */
						{
							Quat qMainInv;
							m_vPreQuat.Inverse(qMainInv);

							Quat qMainqDelta = Quat(m_vecCurrAnimationPose[i].vQuaterion) * qMainInv;
							m_vPreQuat = Quat(m_vecCurrAnimationPose[i].vQuaterion);

							qDelta = qMainqDelta;
							// Yaw(Y축)만 추출
							Vec3 vEuler = qDelta.ToEuler();   // DirectXMath 기준
							fYaw = vEuler.z;
						}
					}

					//// 3. 두 Delta를 블렌딩 비율에 따라 섞음
					//Vec3 vDelta = m_vPreBlendPosition - vTranslation;
					//m_vPreBlendPosition = vTranslation;

					//if (vDelta.Length() < 1.0f) 
					{
						Vec3 vOwnerRight = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
						Vec3 vOwnerUp = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
						Vec3 vOwnerLook = pOwnerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

						vOwnerRight.Normalize();
						vOwnerUp.Normalize();
						vOwnerLook.Normalize();

						Vec3 moveDistance = vOwnerRight * vDelta.x + vOwnerUp * vDelta.z + vOwnerLook * vDelta.y;

						pOwnerPhyCCT->AddFixedMove(moveDistance * fMotionOffset);

						// 오너 Transform Y축 회전에 누적
						pOwnerTransform->Turn_Radian(Vec3::Up, fYaw * -1.f);
					}

					//m_vPreMainPosition
				}
				vTranslation = Vec3::Zero;
				vQuaternion = Quat::Identity;
			}

			matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
			pBone->Set_TransformationMatrix(matTransformation);
		}

		++i;
	}
}

HRESULT CModel::Ready_ComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, CComputeShader* pAnimMixCS, CComputeShader* pAdditiveCS)
{
	Make_GroupBuffers();	// group buffer만 우선 생성
	Make_SB();

	if (pBoneMeshCS)
	{
		for (auto& pMesh : m_vecMeshes)
		{
			if (FAILED(pMesh->Ready_BindCSBuffer(pBoneMeshCS)))
				return E_FAIL;
		}
	}
	
	if (pBoneComBineCS)
	{
		Bind_BoneImmuData(pBoneComBineCS);
		Bind_BufferSRV(pBoneComBineCS);
	}

	// 애니메이션 buffer binding
	if (pAnimEvalCS)
	{
		for (auto& pAnim : m_vecAnimations)
		{
			if (FAILED(pAnim->Ready_BindBuffers(pAnimEvalCS)))
				return E_FAIL;
		}


		// 1. CHANNEL_OUTPUT 초기화
		CS_SRT* pIniailData = new CS_SRT[Get_BoneCount()];

		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			Matrix matBind = m_vecBones[i]->Get_BindPoseTransformMatrix();
			Vector3 vScale, vTranslation;
			Quat vQuat;
			matBind.Decompose(vScale, vQuat, vTranslation);

			pIniailData[i].vScale = vScale;
			pIniailData[i].vQuat = Vec4{ vQuat.x,vQuat.y ,vQuat.z ,vQuat.w };
			pIniailData[i].vTranslation = vTranslation;
			pIniailData[i].Padding0 = 0.f;
			pIniailData[i].Padding1 = 0.f;
		}

		pAnimEvalCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());

		Ready_SB(pAnimEvalCS); 

		if (pAnimBlendCS)
			pAnimBlendCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());
		else
			int a = 0;

		if (pAnimMixCS)
			pAnimMixCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());

		if (pAdditiveCS)
			pAdditiveCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());

		Safe_Delete_Array(pIniailData);
	}

	//if (pAnimMixCS && m_bStageBones)
	//{
	//	Bind_StagingBuffer(pAnimMixCS);
	//}

	return S_OK;
}

HRESULT CModel::Ready_PartComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBonePartCS, CModel* pParentModel)
{
	if (pBoneMeshCS)
	{
		for (auto& pMesh : m_vecMeshes)
		{
			if (FAILED(pMesh->Ready_BindCSBuffer(pBoneMeshCS)))
				return E_FAIL;
		}
	}

	if (pBonePartCS && pParentModel)
	{
		// bone 불변 데이터 넣어줌
		_uint iBoneNums = _uint(m_vecBones.size());

		// 1. 버퍼 내용 생성
		CS_IMMU_PARTBONE* pInitialData = new CS_IMMU_PARTBONE[iBoneNums];

		for (size_t i = 0; i < m_vecBones.size(); i++)
		{
			for (size_t j = 0; j < pParentModel->Get_BoneCount(); j++)
			{
				if (pParentModel->Get_Bone((_uint)j)->Get_Name() == m_vecBones[i]->Get_Name())
				{
					pInitialData[i].iParentIdx = (_uint)j;
					pInitialData[i].iBoneNums = iBoneNums;
					pInitialData[i].Padding0 = Vec2::Zero;
					continue;
				}
			}
		}

		// 2. 바로 바인딩
		pBonePartCS->Bind_InputStructuredBuffer_Data(ENUM_TO_UINT(CS_PARTBONE_IDX::IMMU_BONE), pInitialData, sizeof(CS_IMMU_PARTBONE), iBoneNums);
		Safe_Delete_Array(pInitialData);
	}

	else
		return E_FAIL;

	return S_OK;
}

void CModel::Get_BoneMatrix(CComputeShader* pAnimMixCS)
{
	//// 2. Gpu -> Cpu
	//{
	//	uint32_t writeIndex = m_iFrameIndex % 2;
	//	uint32_t readIndex = (m_iFrameIndex + 1) % 2;

	//	// copy data
	//	m_pDeviceContext->CopyResource(m_pBoneOuputStagingBuffer[writeIndex], pAnimMixCS->Get_Output_Buffer()->Get_Buffer());

	//	if (m_iFrameIndex == 0)
	//	{
	//		m_iFrameIndex++;
	//		return;
	//	}

	//	// 4. Map / Unmap을 통해 CPU로 데이터 가져오기
	//	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//	if (SUCCEEDED(m_pDeviceContext->Map(m_pBoneOuputStagingBuffer[readIndex], 0, D3D11_MAP_READ, 0, &mappedResource)))
	//	{
	//		// 1. 데이터를 행렬 포인터로 해석
	//		Matrix* pGpuMatrices = reinterpret_cast<Matrix*>(mappedResource.pData);

	//		// 2. 중간 복사 없이 바로 bone에 정보 저장
	//		for (size_t i = 0; i < m_iStageBoneCounts; i++)
	//		{
	//			// pGpuMatrices[i]로 바로 접근 가능
	//			m_vecBones[m_vecStageBoneIndices[i]]->Set_CombinedTranformMatrix(pGpuMatrices[i]);
	//		}

	//		m_pDeviceContext->Unmap(m_pBoneOuputStagingBuffer[readIndex], 0);
	//	}

	//	m_iFrameIndex++;
	//}
}

void CModel::DisPatch_BondMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pAnimMixCS)
{
	{
		// combine 정보 넘겨주기
		pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::GETBONECS_SB_IDX::MU_BONEMATS),
			pAnimMixCS->Get_SRV("MU_COMBINEDBONES"), pBoneComBineCS->Get_Output_Buffer());

		// dispatch
		_uint iGroupX = (m_iStageBoneCounts + 31) / 32;
		pAnimMixCS->Dispatch(iGroupX, 1, 1);
	}
}

void CModel::Mix_Animation(CComputeShader* pAnimMixCS, CComputeShader* pPreAnimCS, const _float fTimeDelta)
{
	_bool bFirst = true;
	// todo_eunbi : 만약 이전 mix animation 값을 넘겨줘야한다면 i 값에 따라 분기 나누기
	for (size_t i = 0; i < m_vecMixAnimIndices.size(); i++)
	{
		if (m_vecMixAnimIndices[i] >= 0)
		{
			m_vecAnimations[m_vecMixAnimIndices[i]]->Update_MixAnimation(m_vecBones, pAnimMixCS, pPreAnimCS,fTimeDelta, Get_BoneCount(), bFirst, m_vPreMixPosition, m_vPreQuatMix);
			bFirst = false;
		}
	}
}

_bool  CModel::Additive_Animation(CComputeShader* pAdditiveCS, CComputeShader* pPreAnimCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	// 인덱스 유효성 검사 후
	if (m_iAdditivRef_AnimIdx >= 0 && (_uint)m_iAdditivRef_AnimIdx < Get_AnimationCount()
		&& m_iAdditivePos_AnimIdx >= 0 && (_uint)m_iAdditivePos_AnimIdx < Get_AnimationCount())
	{
		// ref animation data 바인딩
		m_vecAnimations[m_iAdditivRef_AnimIdx]->Bind_RefAnimaationData(pAdditiveCS);

		m_vecAnimations[m_iAdditivePos_AnimIdx]->Update_AdditiveAnimatoin(m_vecBones, m_vecAnimations[m_iAdditivRef_AnimIdx]->Get_Channels(), pAdditiveCS, pPreAnimCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, m_fAdditiveOffset, m_vPreQuatNon);
		return true;
	}

	return false;
}

void CModel::Bind_BoneImmuData(CComputeShader* pBoneComBineCS)
{
	// bone 불변 데이터 넣어줌
	_uint iBoneNums = _uint(m_vecBones.size());

	// 1. 버퍼 내용 생성
	CS_IMMU_BONE* pInitialData = new CS_IMMU_BONE[iBoneNums];
	
	for (size_t i = 0; i < m_vecBones.size(); i++)
	{
		pInitialData[i].iParentIndex = m_vecBones[i]->Get_ParentIndex();
		pInitialData[i].matPreTransform = m_matPreTransform;
		pInitialData[i].Padding0 = Vector3::Zero;
	}

	// 2. 바로 바인딩
	pBoneComBineCS->Bind_InputStructuredBuffer_Data(ENUM_TO_UINT(CS_SB_IDX::IMMU_BONE), pInitialData, sizeof(CS_IMMU_BONE), iBoneNums);
	Safe_Delete_Array(pInitialData);
}

void CModel::Bind_BufferSRV(CComputeShader* pBoneComBineCS)
{
	if (!pBoneComBineCS)
		return;

	// 그룹을 순회
	for (size_t i = 0; i < m_vecBoneGroups.size(); i++)
	{
		_uint iGroupSize = _uint(m_vecBoneGroups[i].BoneIndices.size());

		// 한 그룹당 하나의 structuredBuffer 필요
		CS_MU_BONEIDX* pIniailData = new CS_MU_BONEIDX[iGroupSize];

		// CS_MU_BONEIDX에 값 바인딩
		for (size_t j = 0; j < m_vecBoneGroups[i].BoneIndices.size(); j++)
		{
			pIniailData[j].iMyIdx = m_vecBoneGroups[i].BoneIndices[j];
			pIniailData[j].Padding0 = Vec3::Zero;
		}

		// SB class에 값 넣어주기
		m_vecBoneGroups[i].pIndexBuffer->Copy_Data(pIniailData, sizeof(CS_MU_BONEIDX), iGroupSize);

		// 4. SRV 연결
		m_vecBoneGroups[i].pInputGroupSB_SRV = pBoneComBineCS->Get_SRV("MU_INDEXES");
		m_vecBoneGroups[i].pInputGroupSB_SRV->SetResource(m_vecBoneGroups[i].pIndexBuffer->Get_SRV());

		// 5. 동적배열 정리
		Safe_Delete_Array(pIniailData);
	}
}

void CModel::Ready_SB(CComputeShader* pAnimEvalCS)
{
	pAnimEvalCS->Get_SRV("BLEND_OUTPUT_SRV")->SetResource(m_pPreSB->Get_SRV());
	pAnimEvalCS->Get_SRV("BLEND_OUTPUT_SRV")->SetResource(m_pCurSB->Get_SRV());
}

HRESULT CModel::Bind_StagingBuffer(CComputeShader* pAnimMixCS)
{
	CS_MU_BONEIDX* pInitailData = new CS_MU_BONEIDX[m_iStageBoneCounts];


	// 2. 버퍼 내용을 쓴다
	for (size_t i = 0; i < m_iStageBoneCounts; i++)
	{
		pInitailData[i].iMyIdx = m_vecStageBoneIndices[i];
		pInitailData[i].Padding0 = Vec3::Zero;
	}

	// 3. 바로 바인딩
	pAnimMixCS->Bind_InputStructuredBuffer_Data(ENUM_TO_UINT(GETBONECS_SB_IDX::IMMU_BONEINDICES), pInitailData, sizeof(CS_MU_BONEIDX), m_iStageBoneCounts);
	Safe_Delete_Array(pInitailData);

	return S_OK;
}

//void CModel::Get_BoneMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pAnimMixCS)
//{
//	// 1. GetBone CS dispatch
//	{
//		// combine 정보 넘겨주기
//		pAnimMixCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::GETBONECS_SB_IDX::MU_BONEMATS),
//			pAnimMixCS->Get_SRV("MU_COMBINEDBONES"), pBoneComBineCS->Get_Output_Buffer());
//
//		// dispatch
//		_uint iGroupX = (m_iStageBoneCounts + 31) / 32;
//		pAnimMixCS->Dispatch(iGroupX, 1, 1);
//	}
//
//	// 2. Gpu -> Cpu
//	{
//		// copy data
//		m_pDeviceContext->CopyResource(m_pBoneOuputStagingBuffer, pAnimMixCS->Get_Output_Buffer()->Get_Buffer());
//
//		vector<Matrix> vecBones;
//		vecBones.resize(m_iStageBoneCounts);
//
//		// 4. Map / Unmap을 통해 CPU로 데이터 가져오기
//		D3D11_MAPPED_SUBRESOURCE mappedResource;
//		if (SUCCEEDED(m_pDeviceContext->Map(m_pBoneOuputStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
//		{
//			// 1. 데이터를 행렬 포인터로 해석
//			Matrix* pGpuMatrices = reinterpret_cast<Matrix*>(mappedResource.pData);
//
//			// 2. 중간 복사 없이 바로 bone에 정보 저장
//			for (size_t i = 0; i < m_iStageBoneCounts; i++)
//			{
//				// pGpuMatrices[i]로 바로 접근 가능
//				m_vecBones[m_vecStageBoneIndices[i]]->Set_CombinedTranformMatrix(pGpuMatrices[i]);
//			}
//
//			m_pDeviceContext->Unmap(m_pBoneOuputStagingBuffer, 0);
//		}
//	}
//}

// Phase로직이 잘 이루어지려면 애니메이션 제어권은 무조건 ActionState에게 있어야하며
// 해당 ActionState의 Update가 이루어지고 난 이후에 Body의 ModelComponent PlayAnimation이 호출되어야한다.
HRESULT CModel::Emit_Notifies(EAnimNotifyPhase ePhase)
{
	if (ePhase >= EAnimNotifyPhase::END)
		return E_FAIL;

	if (m_iCurrentAnimIndex < 0 || m_iCurrentAnimIndex >= m_vecAnimations.size())
		return S_OK;

	Emit_Notifies(m_vecAnimations[m_iCurrentAnimIndex], m_vecAnimations[m_iCurrentAnimIndex]->Get_TrackPosition(), ePhase);
	return S_OK;
}

void CModel::Emit_Notifies(CModelAnimation* pAnimation, _float fCurPos, EAnimNotifyPhase ePhase)
{
	// Animation 이벤트들
	const auto& notifyKeys = pAnimation->Get_Notifies(ePhase);
	if (notifyKeys.empty() == true)
		return;

	// 현재 애니메이션 Duration
	const _float fDuration = pAnimation->Get_DurationTime();

	// Event Cursor ( 실행 될 이벤트의 Index를 가리킨다 )
	_uint iIndex = pAnimation->Get_NotifyCursor(ePhase);

	// 람다 ( From, To 사이에 있는 이벤트 Broadcast )
	auto Emit_Range = [&](_float fFrom, _float fTo)->void
		{
			while (iIndex < notifyKeys.size() &&
				notifyKeys[iIndex].fTrackPosition <= fTo)
			{
				if (notifyKeys[iIndex].fTrackPosition > fFrom)
					OnNotify.Broadcast(notifyKeys[iIndex]);

				pAnimation->Set_NotifyCursor(ePhase, ++iIndex);
			}
		};

	if (m_bLooped == false)
		Emit_Range(m_fAnimPrevTrackPosition, fCurPos);
	// Loop가 처리됬을때 세팅
	else
	{
		Emit_Range(m_fAnimPrevTrackPosition, fDuration);
		pAnimation->Set_NotifyCursor(ePhase, 0);
		iIndex = 0;
		Emit_Range(0.f, fCurPos);
	}
}

void CModel::Mapping_Ragdoll_Bone()
{
	m_arrRagdollBoneDesc[RAGDOLLJOINT::PELVIS]		= Set_Ragdoll_Bone(RAGDOLLJOINT::PELVIS,	RAGDOLLJOINT::END,			RAGDOLLJOINT::SPINE_02);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::SPINE_02]	= Set_Ragdoll_Bone(RAGDOLLJOINT::SPINE_02,	RAGDOLLJOINT::PELVIS,		RAGDOLLJOINT::HEAD);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::HEAD]		= Set_Ragdoll_Bone(RAGDOLLJOINT::HEAD,		RAGDOLLJOINT::SPINE_02,		RAGDOLLJOINT::END);

	m_arrRagdollBoneDesc[RAGDOLLJOINT::UPPERARM_L]	= Set_Ragdoll_Bone(RAGDOLLJOINT::UPPERARM_L, RAGDOLLJOINT::SPINE_02,	RAGDOLLJOINT::LOWERARM_L);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::LOWERARM_L]	= Set_Ragdoll_Bone(RAGDOLLJOINT::LOWERARM_L, RAGDOLLJOINT::UPPERARM_L,	RAGDOLLJOINT::END);

	m_arrRagdollBoneDesc[RAGDOLLJOINT::UPPERARM_R]	= Set_Ragdoll_Bone(RAGDOLLJOINT::UPPERARM_R, RAGDOLLJOINT::SPINE_02,	RAGDOLLJOINT::LOWERARM_R);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::LOWERARM_R]	= Set_Ragdoll_Bone(RAGDOLLJOINT::LOWERARM_R, RAGDOLLJOINT::UPPERARM_R,	RAGDOLLJOINT::END);

	m_arrRagdollBoneDesc[RAGDOLLJOINT::THIGH_L]		= Set_Ragdoll_Bone(RAGDOLLJOINT::THIGH_L,	RAGDOLLJOINT::PELVIS,		RAGDOLLJOINT::CALF_L);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::CALF_L]		= Set_Ragdoll_Bone(RAGDOLLJOINT::CALF_L,	RAGDOLLJOINT::THIGH_L,		RAGDOLLJOINT::FOOT_L);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::FOOT_L]		= Set_Ragdoll_Bone(RAGDOLLJOINT::FOOT_L,	RAGDOLLJOINT::CALF_L,		RAGDOLLJOINT::END);

	m_arrRagdollBoneDesc[RAGDOLLJOINT::THIGH_R]		= Set_Ragdoll_Bone(RAGDOLLJOINT::THIGH_R,	RAGDOLLJOINT::PELVIS,		RAGDOLLJOINT::CALF_R);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::CALF_R]		= Set_Ragdoll_Bone(RAGDOLLJOINT::CALF_R,	RAGDOLLJOINT::THIGH_R,		RAGDOLLJOINT::FOOT_R);
	m_arrRagdollBoneDesc[RAGDOLLJOINT::FOOT_R]		= Set_Ragdoll_Bone(RAGDOLLJOINT::FOOT_R,	RAGDOLLJOINT::CALF_R,		RAGDOLLJOINT::END);
}

RAGDOLLBONEDESC CModel::Set_Ragdoll_Bone(RAGDOLLJOINT::Enum eJoint, RAGDOLLJOINT::Enum eParentJoint, RAGDOLLJOINT::Enum eChildJoint)
{
	CBone* bone = Get_Bone(PhysicsJointNames[eJoint].c_str());
	RAGDOLLBONEDESC desc{};
	desc.eJoint = eJoint;
	desc.eParentJoint = eParentJoint;

	if (bone == nullptr)
		return desc;
	
	desc.iBoneIndex = bone->Get_Index();
	desc.iParentIndex = bone->Get_ParentIndex();
	desc.matLocalTransform = bone->Get_Transform();

	CBone* child = Get_Bone(PhysicsJointNames[eChildJoint].c_str());
	if (child != nullptr)
		desc.fHeight = child->Get_Transform().Translation().Length();
	else
		desc.fHeight = 0.05f;

	switch (eJoint)
	{
	case Engine::ERagdollJoint::PELVIS:
		desc.fRadius = 0.25f;
		desc.fMass = 1.f;
		//desc.fHeight = 0.25f;
		break;
	case Engine::ERagdollJoint::SPINE_02:
		desc.fRadius = 0.05f;
		//desc.fHeight = 0.05f;
		break;
	case Engine::ERagdollJoint::HEAD:
		desc.fRadius = 0.1f;
		desc.fMass = 5.f;
		//desc.fHeight = 0.1f;
		break;
	case Engine::ERagdollJoint::UPPERARM_L:
	case Engine::ERagdollJoint::UPPERARM_R:
		desc.fRadius = 0.1f;
		desc.fMass = 0.1f;
		//desc.fHeight = 0.1f;
		break;
	case Engine::ERagdollJoint::LOWERARM_L:
	case Engine::ERagdollJoint::LOWERARM_R:
		desc.fRadius = 0.1f;
		desc.fMass = 0.1f;
		//desc.fHeight = 0.1f;
		break;
	case Engine::ERagdollJoint::THIGH_L:
	case Engine::ERagdollJoint::THIGH_R:
		desc.fRadius = 0.1f;
		desc.fMass = 0.1f;
		//desc.fHeight = 0.1f;
		break;
	case Engine::ERagdollJoint::CALF_L:
	case Engine::ERagdollJoint::CALF_R:
		desc.fRadius = 0.05f;
		desc.fMass = 0.1f;
		//desc.fHeight = 0.05f;
		break;
	case Engine::ERagdollJoint::FOOT_L:
	case Engine::ERagdollJoint::FOOT_R:
		desc.fRadius = 0.05f;
		desc.fMass = 0.05f;
		break;
	}

	desc.matOffsetTransform = PxTransform(PxVec3(0.f, -desc.fHeight, 0.f), PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
	//desc.matOffsetTransform = PxTransform(PxVec3(0.f, -desc.fHeight * 0.5f, 0.f),
	//	PxQuat(PxHalfPi, PxVec3(0, 0, 1)));

	return desc;
}

CModel::GHOST_TRAIL_DESC CModel::Init_GhostTrailDesc(CModel::GHOST_TRAIL_DESC tDesc)
{
	if (tDesc.fInterval <= 0.f)
		tDesc.fInterval = 0.03f;

	if (tDesc.fLifeTime <= 0.f)
		tDesc.fLifeTime = 0.18f;

	if (tDesc.iMaxCount == 0)
		tDesc.iMaxCount = 1;

	tDesc.vColor.w = (std::max)(0.f, tDesc.vColor.w);
	return tDesc;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CModel* pInstance = new CModel(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("CModel::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CModel::Create, Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	Super::Free();

	Clear_GhostTrail();

	/* Prototype 일떄만 지운다 */
	if (!CComponent::IsClone())
		Safe_Delete_Array(m_pStaticModel_MinMax);


	for (auto& pBone : m_vecBones)
		Safe_Release(pBone);

	for (auto& pMesh : m_vecMeshes)
		Safe_Release(pMesh);

	for (auto& pMaterial : m_vecMaterials)
		Safe_Release(pMaterial);

	for (auto& pMaterialInstance : m_vecMaterialInstances)
		Safe_Release(pMaterialInstance);

	for (auto& pAnimation : m_vecAnimations)
		Safe_Release(pAnimation);


	if (m_eType == EModelType::ANIM)
	{
		for (auto& pBoneGroup : m_vecBoneGroups)
		{
			//if (IsClone())
			{
				Safe_Release(pBoneGroup.pInputGroupSB_SRV);
				Safe_Release(pBoneGroup.pIndexBuffer);
				int i = 0;
			}
		}
		m_vecBoneGroups.clear();

		//if (IsClone())
		{
			Safe_Release(m_pPreSB);
			Safe_Release(m_pCurSB);
		}

		if (m_bStageBones)
		{
			//Safe_Release(m_pBoneOuputStagingBuffer[0]);
			//Safe_Release(m_pBoneOuputStagingBuffer[1]);
		}
	}
	
	OnNotify.Clear();
	m_vecBones.clear();
	m_vecMeshes.clear();
	m_vecMaterials.clear();
	m_vecAnimations.clear();
	m_vecMaterialInstances.clear();
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
