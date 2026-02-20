#include "Engine_pch.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"
#include "Engine_Utils.h"
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
	, m_pPreSB(rhs.m_pPreSB)
	, m_pCurSB(rhs.m_pCurSB)
	, m_bStageBones(rhs.m_bStageBones)
	, m_iStageBoneCounts(rhs.m_iStageBoneCounts)
	, m_pStaticModel_MinMax{rhs.m_pStaticModel_MinMax}
	, m_iFrameIndex(rhs.m_iFrameIndex)
	, m_iCpuBoneCount(rhs.m_iCpuBoneCount)
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
		m_vecBoneGroups.reserve(rhs.m_vecBoneGroups.size());
		for (auto& pBoneGroup : rhs.m_vecBoneGroups)
		{
			m_vecBoneGroups.push_back(pBoneGroup);

		}

		//if (m_bStageBones)
		//{
		//	Safe_AddRef(m_pBoneOuputStagingBuffer[0]);
		//	Safe_AddRef(m_pBoneOuputStagingBuffer[1]);
		//}
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


	for (size_t i = 0; i < m_vecAnimations.size(); i++)
	{
		if (Get_AnimationName((_uint)i) == TEXT("Animation_PlayerMoon_Land_Inplace"))
			m_vecAnimations[i]->Set_ApplyRootMotion(false);
	}

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
			if (FAILED(Change_MI((_uint)i, Engine_Utils::MI_ToWString(EMaterialInstanceType::Default))))
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
				if (FAILED(Change_MI((_uint)i, Engine_Utils::MI_ToWString(pDesc->spanMIs[i]))))
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
			return E_FAIL;
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

HRESULT CModel::Change_Animation(CComputeShader* pAnimEComShader, _uint iAnimationIndex, _bool bBlend, _bool isLoop, _bool bForce)
{
	if (m_iCurrentAnimIndex == iAnimationIndex && bForce == false)
		return S_OK;

	if (bBlend)
	{
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		Change_AnimationPlayState(AnimationPlayState::BLEND);
	}
	else
		Change_AnimationPlayState(AnimationPlayState::PLAY, pAnimEComShader, iAnimationIndex);

	m_iCurrentAnimIndex = iAnimationIndex;
	m_vecAnimations[m_iCurrentAnimIndex]->Clear();
	m_isAnimLoop = isLoop;

	return S_OK;
}

void CModel::Update_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEComShader, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pAnimBlendCS, CComputeShader* pGetBoneCS)
{
	Update_AnimationPlayState(pBoneComBineCS, pAnimEComShader, pAnimBlendCS,  fTimeDelta, pOwnerTransform, pOwnerPhyCCT, pGetBoneCS);
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
	m_vecBones[iRootIdx]->Set_MotionBone(iRootIdx);

	// animation, channel 정보 업데이트
	for (auto& pAnim : m_vecAnimations)
	{
		pAnim->Set_MotionBone(iRootIdx);
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

HRESULT CModel::Change_MI(_uint iIndex, const wstring& wstrMITag)
{
	if (iIndex >= m_vecMaterialInstances.size())
		return E_FAIL;

	CMaterialInstance* pMI = m_pGameInstance->Get_Resource<CMaterialInstance>(wstrMITag);
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

_bool CModel::Is_AnimTrackPositionAt(_float fRatio)
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionAt(fRatio);
}

_bool CModel::Is_AnimTrackPositionAtHalf() const
{
	return m_vecAnimations[m_iCurrentAnimIndex]->Is_TrackPositionAtHalf();
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

HRESULT CModel::Ready_ComputeShaders(CComputeShader* pBoneMeshCS, CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, CComputeShader* pGetBoneCS)
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
			if(FAILED(pAnim->Ready_BindBuffers(pAnimEvalCS)))
				return E_FAIL;
		}

		Ready_SB(pAnimEvalCS);


		// 1. CHANNEL_OUTPUT 초기화
		CS_SRT* pIniailData = new CS_SRT[Get_BoneCount()];

		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			Matrix matBind = m_vecBones[i]->Get_BindPoseTransformMatrix();
			Vector3 vScale, vTranslation;
			Quat vQuat;
			matBind.Decompose(vScale, vQuat, vTranslation);

			pIniailData[i].vScale = vScale;
			pIniailData[i].vQuat= vQuat;
			pIniailData[i].vTranslation = vTranslation;
			pIniailData[i].Padding0 = 0.f;
			pIniailData[i].Padding1 = 0.f;
		}

		pAnimEvalCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());

		if(pAnimBlendCS)
			pAnimBlendCS->Get_Output_Buffer()->Copy_Data(pIniailData, sizeof(CS_SRT), Get_BoneCount());

		Safe_Delete_Array(pIniailData);
	}

	if (pGetBoneCS && m_bStageBones)
	{
		Bind_StagingBuffer(pGetBoneCS);
	}

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

void CModel::Play_Animation(_float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	//if (pOwnerTransform)
	//	m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_isAnimLoop, pOwnerTransform, pOwnerPhyCCT);
	//else
	//	m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_isAnimLoop, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());

	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		m_vecBones[i]->Update_CombinedTransformMatrix(m_vecBones, m_matPreTransform);
	}
}

void CModel::Blend_Animation(_float fTimeDelta, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	if (pOwnerTransform)
	{
		//m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
		//m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
	}
	
	else
	{
		//m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());
		//m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());
	}

	_uint i = {};
	for (auto& pBone : m_vecBones)
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
			vTranslation = Vec3::Zero;
		}
		
		matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
		pBone->Set_TransformationMatrix(matTransformation);
		++i;
	}

	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		m_vecBones[i]->Update_CombinedTransformMatrix(m_vecBones, m_matPreTransform);
	}
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

void CModel::Begin_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS, _uint iAnimationIndex)
{
	switch (eState)
	{
	case Engine::CModel::PLAY:
		Play_Begin(pAnimEvalCS, iAnimationIndex);
		break;
	case Engine::CModel::BLEND:
		Blend_Begin();
		break;
	}
}

void CModel::Update_AnimationPlayState(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pGetBoneCS)
{
	switch (m_eCurrentAnimationState)
	{
	case Engine::CModel::PLAY:
		Play_Update(pBoneComBineCS, pAnimEvalCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, pGetBoneCS);
		break;
	case Engine::CModel::BLEND:
		if(pAnimBlendCS)
			Blend_Update(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, pGetBoneCS);
		break;
	}
}

void CModel::End_AnimationPlayState(AnimationPlayState eState)
{
	switch (eState)
	{
	case Engine::CModel::PLAY:
		Play_End();
		break;
	case Engine::CModel::BLEND:
		Blend_End();
		break;
	}
}

void CModel::Change_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS, _uint iAnimationIndex)
{
	End_AnimationPlayState(m_eCurrentAnimationState);
	Begin_AnimationPlayState(eState, pAnimEvalCS, iAnimationIndex);
	m_eCurrentAnimationState = eState;
}

void CModel::Play_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, _float fTimeDelta, CTransform* pOwnerTransform , CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pGetBoneCS)
{
	// animation update
	m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_isAnimLoop, pOwnerTransform, pOwnerPhyCCT, pAnimEvalCS);

	// animation 결과 blendCS에 bind
	pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRTS),
		pBoneComBineCS->Get_SRV("MU_SRTS"), pAnimEvalCS->Get_Output_Buffer());

	// bone updatezd
	Update_BoneCombineTransformMatrix(pBoneComBineCS);

	//// get bone
	//if (m_bStageBones)
	//	DisPatch_BondMatrix(pBoneComBineCS, pGetBoneCS);
}

void CModel::Play_Begin(CComputeShader* pAnimEvalCS, _uint iAnimationIndex)
{
	if (pAnimEvalCS)
		m_vecAnimations[iAnimationIndex]->Bind_AnimationEData(pAnimEvalCS);
}

void CModel::Play_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pGetBoneCS)
{
	CModelAnimation* pAnimation = m_vecAnimations[m_iCurrentAnimIndex];
	_float fPrevPosition = pAnimation->Get_TrackPosition();

	Play_Animation(pBoneComBineCS, pAnimEvalCS, fTimeDelta,  pOwnerTransform ,  pOwnerPhyCCT, pGetBoneCS);

	_float fCurrentPosition = pAnimation->Get_TrackPosition();
	const _bool bLooped = m_isAnimLoop && (fCurrentPosition < fPrevPosition);
	Emit_Notifies(pAnimation, fPrevPosition, fCurrentPosition, bLooped);
}

void CModel::Play_End()
{
	m_bIsAnimFinished = false;
}

void CModel::Blend_Begin()
{
	m_fBlendedTime = 0.f;
}

void CModel::Blend_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pGetBoneCS)
{
	if (m_fBlendDuration <= 0.f)
	{
		Change_AnimationPlayState(PLAY, pAnimEvalCS, m_iCurrentAnimIndex);
		return;
	}

	CModelAnimation* pAnimation = m_vecAnimations[m_iCurrentAnimIndex];
	_float fPrevPosition = pAnimation->Get_TrackPosition();

	m_fBlendedTime += fTimeDelta;
	if (m_fBlendedTime < m_fBlendDuration)
	{
		_float fNormalizedTime = std::clamp(m_fBlendedTime / m_fBlendDuration, 0.f, 1.f);
		_float fRatio = fNormalizedTime * fNormalizedTime * (3.0f - 2.0f * fNormalizedTime);

		if (pOwnerTransform)
			Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, pOwnerTransform, pOwnerPhyCCT, pGetBoneCS);
		else
			Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>(), pGetBoneCS);
	}
	else
		Change_AnimationPlayState(PLAY, pAnimEvalCS, m_iCurrentAnimIndex);

	const _float fCurrentPosition = pAnimation->Get_TrackPosition();
	const _bool bLooped = (m_isAnimLoop && fCurrentPosition < fPrevPosition);
	Emit_Notifies(pAnimation, fPrevPosition, fCurrentPosition, bLooped);
}

void CModel::Blend_End()
{
	m_fBlendedTime = 0.f;
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
		m_vecBoneGroups[i].pIndexBuffer = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_MU_BONEIDX), iGroupSize);
	}
}

void CModel::Make_SB()
{
	m_pPreSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_SRT), Get_BoneCount());
	m_pCurSB = StructuredBuffer::Create(m_pDevice, m_pDeviceContext, sizeof(CS_SRT), Get_BoneCount());
}

void CModel::Make_Staging(MODEL_ORIGIN_DESC* pDesc)
{
	m_iStageBoneCounts = (_uint)(pDesc->vecStageBoneIndices.size());

	// 1. staging buffer 생성
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(CS_OUT_BONE) * m_iStageBoneCounts;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	m_pDevice->CreateBuffer(&desc, nullptr, &m_pBoneOuputStagingBuffer[0]);
	m_pDevice->CreateBuffer(&desc, nullptr, &m_pBoneOuputStagingBuffer[1]);

	// 2. bone indices 캐스팅 하고 있자
	m_vecStageBoneIndices.reserve(m_iStageBoneCounts);
	m_vecStageBoneIndices = pDesc->vecStageBoneIndices;
}

void CModel::Update_BoneCombineTransformMatrix(CComputeShader* pBoneComBineCS)
{
	if (pBoneComBineCS == nullptr)
		return;

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

void CModel::Blend_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT, CComputeShader* pGetBoneCS)
{
	if (pOwnerTransform)
	{
		// 1. 버퍼 빼돌리기
		StructuredBuffer* pOriginSB = pAnimEvalCS->Get_Output_Buffer();

		// 2. pre animation
		{
			// 내 버퍼로 받도록 설정
			pAnimEvalCS->Set_OutputStructuredBuffer(m_pPreSB);

			// channel 업데이트
			m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, Get_BoneCount(), pAnimEvalCS);

			// animation 결과 blendCS에 bind
			pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_PRESRT),
				pAnimBlendCS->Get_SRV("MU_PRETRANSFORMS"), m_pPreSB);
		}

		// 3. cur animation
		{
			// 내 버퍼로 받도록 설정
			pAnimEvalCS->Set_OutputStructuredBuffer(m_pCurSB);

			// channel 업데이트
			m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, pOwnerTransform, pOwnerPhyCCT, Get_BoneCount(), pAnimEvalCS);

			// animation 결과 blendCS에 bind
			pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_CURSRT),
				pAnimBlendCS->Get_SRV("MU_CURTRANSFORMS"), m_pCurSB);
		}

		// 4. 버퍼 돌려놓기
		pAnimEvalCS->Set_OutputStructuredBuffer(pOriginSB);

	}

	// animation 2개를 lerp
	Lerp_Animation(pAnimBlendCS, fRatio);

	// animation 결과 bone cs에 bind
	pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::CS_SB_IDX::MU_SRTS),
		pBoneComBineCS->Get_SRV("MU_SRTS"), pAnimBlendCS->Get_Output_Buffer());

	Update_BoneCombineTransformMatrix(pBoneComBineCS);

	//if (m_bStageBones)
	//	DisPatch_BondMatrix(pBoneComBineCS, pGetBoneCS);
}

void CModel::Lerp_Animation(CComputeShader* pAnimBlendCS, _float fRatio)
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
				vTranslation = Vec3::Zero;
			}

			matTransformation = Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuaternion) * Matrix::CreateTranslation(vTranslation);
			pBone->Set_TransformationMatrix(matTransformation);
		}

		++i;
	}
}

void CModel::Get_BoneMatrix(CComputeShader* pGetBoneCS)
{

	// 2. Gpu -> Cpu
	{
		uint32_t writeIndex = m_iFrameIndex % 2;
		uint32_t readIndex = (m_iFrameIndex + 1) % 2;

		// copy data
		m_pDeviceContext->CopyResource(m_pBoneOuputStagingBuffer[writeIndex], pGetBoneCS->Get_Output_Buffer()->Get_Buffer());

		if (m_iFrameIndex == 0)
		{
			m_iFrameIndex++;
			return;
		}

		// 4. Map / Unmap을 통해 CPU로 데이터 가져오기
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(m_pDeviceContext->Map(m_pBoneOuputStagingBuffer[readIndex], 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			// 1. 데이터를 행렬 포인터로 해석
			Matrix* pGpuMatrices = reinterpret_cast<Matrix*>(mappedResource.pData);

			// 2. 중간 복사 없이 바로 bone에 정보 저장
			for (size_t i = 0; i < m_iStageBoneCounts; i++)
			{
				// pGpuMatrices[i]로 바로 접근 가능
				m_vecBones[m_vecStageBoneIndices[i]]->Set_CombinedTranformMatrix(pGpuMatrices[i]);
			}

			m_pDeviceContext->Unmap(m_pBoneOuputStagingBuffer[readIndex], 0);
		}

		m_iFrameIndex++;
	}
}

void CModel::DisPatch_BondMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pGetBoneCS)
{
	{
		// combine 정보 넘겨주기
		pGetBoneCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::GETBONECS_SB_IDX::MU_BONEMATS),
			pGetBoneCS->Get_SRV("MU_COMBINEDBONES"), pBoneComBineCS->Get_Output_Buffer());

		// dispatch
		_uint iGroupX = (m_iStageBoneCounts + 31) / 32;
		pGetBoneCS->Dispatch(iGroupX, 1, 1);
	}
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

HRESULT CModel::Bind_StagingBuffer(CComputeShader* pGetBoneCS)
{
	CS_MU_BONEIDX* pInitailData = new CS_MU_BONEIDX[m_iStageBoneCounts];


	// 2. 버퍼 내용을 쓴다
	for (size_t i = 0; i < m_iStageBoneCounts; i++)
	{
		pInitailData[i].iMyIdx = m_vecStageBoneIndices[i];
		pInitailData[i].Padding0 = Vec3::Zero;
	}

	// 3. 바로 바인딩
	pGetBoneCS->Bind_InputStructuredBuffer_Data(ENUM_TO_UINT(GETBONECS_SB_IDX::IMMU_BONEINDICES), pInitailData, sizeof(CS_MU_BONEIDX), m_iStageBoneCounts);
	Safe_Delete_Array(pInitailData);

	return S_OK;
}

//void CModel::Get_BoneMatrix(CComputeShader* pBoneComBineCS, CComputeShader* pGetBoneCS)
//{
//	// 1. GetBone CS dispatch
//	{
//		// combine 정보 넘겨주기
//		pGetBoneCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::GETBONECS_SB_IDX::MU_BONEMATS),
//			pGetBoneCS->Get_SRV("MU_COMBINEDBONES"), pBoneComBineCS->Get_Output_Buffer());
//
//		// dispatch
//		_uint iGroupX = (m_iStageBoneCounts + 31) / 32;
//		pGetBoneCS->Dispatch(iGroupX, 1, 1);
//	}
//
//	// 2. Gpu -> Cpu
//	{
//		// copy data
//		m_pDeviceContext->CopyResource(m_pBoneOuputStagingBuffer, pGetBoneCS->Get_Output_Buffer()->Get_Buffer());
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

void CModel::Emit_Notifies(CModelAnimation* pAnimation, _float fPrevPos, _float fCurPos, _bool bIsLooped)
{
	// Animation 이벤트들
	const auto& notifyKeys = pAnimation->Get_Notifies();
	if (notifyKeys.empty() == true)
		return;

	// 현재 애니메이션 Duration
	const _float fDuration = pAnimation->Get_DurationTime();

	// Event Cursor ( 실행 될 이벤트의 Index를 가리킨다 )
	_uint iIndex = pAnimation->Get_NotifyCursor();

	// 람다 ( From, To 사이에 있는 이벤트 Broadcast )
	auto Emit_Range = [&](_float fFrom, _float fTo)->void
		{
			while (iIndex < notifyKeys.size() &&
				notifyKeys[iIndex].fTrackPosition <= fTo)
			{
				if (notifyKeys[iIndex].fTrackPosition > fFrom)
					OnNotify.Broadcast(notifyKeys[iIndex]);

				pAnimation->Set_NotifyCursor(++iIndex);
			}
		};

	if (bIsLooped == false)
		Emit_Range(fPrevPos, fCurPos);
	// Loop가 처리됬을때 세팅
	else
	{
		Emit_Range(fPrevPos, fDuration);
		pAnimation->Set_NotifyCursor(0);
		iIndex = 0;
		Emit_Range(0.f, fCurPos);
	}
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
			if (IsClone())
			{
				Safe_Release(pBoneGroup.pInputGroupSB_SRV);
				Safe_Release(pBoneGroup.pIndexBuffer);
			}
		}

		if (IsClone())
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
	Super::Free();
}
