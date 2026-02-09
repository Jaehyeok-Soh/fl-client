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
	, m_pMasterMesh(rhs.m_pMasterMesh)
	, m_iRootBoneIdx(rhs.m_iRootBoneIdx)
{
	m_vecPrevAnimationPose.resize(rhs.m_vecPrevAnimationPose.size());
	m_vecCurrAnimationPose.resize(rhs.m_vecCurrAnimationPose.size());

	m_vecBoneGroups.reserve(rhs.m_vecBoneGroups.size());
	for (auto& pBoneGroup : rhs.m_vecBoneGroups)
	{
		m_vecBoneGroups.push_back(pBoneGroup);
		Safe_AddRef(pBoneGroup.pIndexBuffer);
	}

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

	Safe_AddRef(m_pMasterMesh);
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

	Make_BoneGroup(); // bone을 그룹별로 만든다
	Make_GroupBuffers(); // group buffer만 우선 생성

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

HRESULT CModel::Change_Animation(_uint iAnimationIndex, _bool bBlend, _bool isLoop, _bool bForce, CComputeShader* pAnimEvalCS)
{
	if (m_iCurrentAnimIndex == iAnimationIndex && bForce == false)
		return S_OK;

	if (bBlend)
	{
		m_iPrevAnimIndex = m_iCurrentAnimIndex;
		Change_AnimationPlayState(AnimationPlayState::BLEND);
	}
	else
		Change_AnimationPlayState(AnimationPlayState::PLAY, pAnimEvalCS);

	m_iCurrentAnimIndex = iAnimationIndex;
	m_vecAnimations[m_iCurrentAnimIndex]->Clear();
	m_isAnimLoop = isLoop;
	return S_OK;
}

void CModel::Update_Animation(CComputeShader* pBoneComShader, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	Update_AnimationPlayState(pBoneComShader, pAnimEvalCS, pAnimBlendCS,  fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
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

HRESULT CModel::Bind_Bones(CShader* pShader, _uint iMeshIndex, _uint iIndexDistance)
{
	if (iMeshIndex >= m_vecMeshes.size())
		return E_FAIL;

	return m_vecMeshes[iMeshIndex]->Bind_Bones(pShader, m_vecBones, iIndexDistance);
}

HRESULT CModel::Bind_Masterbones(CShader* pShader, _uint iIndexDistance)
{
	if (!m_pMasterMesh)
		return E_FAIL;

	return m_pMasterMesh->Bind_Bones(pShader, m_vecBones, iIndexDistance);
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

HRESULT CModel::Ready_CSs(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS)
{
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
	}

	return S_OK;
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

	Make_BoneGroup();
	
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
	if (pOwnerTransform)
		m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_isAnimLoop, pOwnerTransform, pOwnerPhyCCT);
	else
		m_bIsAnimFinished = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_isAnimLoop, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());

	for (size_t i = 0; i < m_vecBones.size(); ++i)
	{
		m_vecBones[i]->Update_CombinedTransformMatrix(m_vecBones, m_matPreTransform);
	}
}

void CModel::Blend_Animation(_float fTimeDelta, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	if (pOwnerTransform)
	{
		m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, nullptr, pOwnerPhyCCT);
		m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, nullptr, pOwnerPhyCCT);
	}
	
	else
	{
		m_vecAnimations[m_iPrevAnimIndex]->SetUp_PoseDatasForBlending(m_vecPrevAnimationPose, fTimeDelta, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());
		m_vecAnimations[m_iCurrentAnimIndex]->SetUp_PoseDatasForBlending(m_vecCurrAnimationPose, fTimeDelta, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());
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

void CModel::Begin_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS)
{
	switch (eState)
	{
	case Engine::CModel::PLAY:
		Play_Begin(pAnimEvalCS);
		break;
	case Engine::CModel::BLEND:
		Blend_Begin();
		break;
	}
}

void CModel::Update_AnimationPlayState(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	switch (m_eCurrentAnimationState)
	{
	case Engine::CModel::PLAY:
		Play_Update(pBoneComBineCS, pAnimEvalCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
		break;
	case Engine::CModel::BLEND:
		Blend_Update(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, pOwnerTransform, pOwnerPhyCCT);
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

void CModel::Change_AnimationPlayState(AnimationPlayState eState, CComputeShader* pAnimEvalCS)
{
	End_AnimationPlayState(m_eCurrentAnimationState);
	Begin_AnimationPlayState(eState, pAnimEvalCS);
	m_eCurrentAnimationState = eState;
}

void CModel::Play_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, _float fTimeDelta, CTransform* pOwnerTransform , CPhysicsCCT* pOwnerPhyCCT)
{
	// animation update
	m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformMatrices(pAnimEvalCS, fTimeDelta, m_isAnimLoop, pOwnerTransform, pOwnerPhyCCT);

	// animation 결과 blendCS에 bind
	pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CS_SB_IDX::MU_SRTS),
		pBoneComBineCS->Get_SRV("MU_SRTS"), pAnimEvalCS->Get_Output_Buffer());

	// bone updatezd
	Update_BoneCombineTransformMatrix(pBoneComBineCS);
}

void CModel::Play_Begin(CComputeShader* pAnimEvalCS)
{
	if(pAnimEvalCS)
		m_vecAnimations[m_iCurrentAnimIndex]->Bind_AnimationEData(pAnimEvalCS);
}

void CModel::Play_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	Play_Animation(pBoneComBineCS, pAnimEvalCS, fTimeDelta,  pOwnerTransform ,  pOwnerPhyCCT);
}

void CModel::Play_End()
{
	m_bIsAnimFinished = false;
}

void CModel::Blend_Begin()
{
	m_fBlendedTime = 0.f;
}

void CModel::Blend_Update(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, const _float fTimeDelta, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	if(m_fBlendDuration <= 0.f)
		Change_AnimationPlayState(PLAY, pAnimEvalCS);

	m_fBlendedTime += fTimeDelta;
	if (m_fBlendedTime < m_fBlendDuration)
	{
		_float fNormalizedTime = std::clamp(m_fBlendedTime / m_fBlendDuration, 0.f, 1.f);
		_float fRatio = fNormalizedTime * fNormalizedTime * (3.0f - 2.0f * fNormalizedTime);

		if (pOwnerTransform)
			Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, pOwnerTransform, pOwnerPhyCCT);
		else
			Blend_Animation(pBoneComBineCS, pAnimEvalCS, pAnimBlendCS, fTimeDelta, fRatio, m_pOwner->Get_Component<CTransform>(), m_pOwner->Get_Component<CPhysicsCCT>());
	}
	else
		Change_AnimationPlayState(PLAY, pAnimEvalCS);
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
}

void CModel::Blend_Animation(CComputeShader* pBoneComBineCS, CComputeShader* pAnimEvalCS, CComputeShader* pAnimBlendCS, _float fTimeDelta, _float fRatio, CTransform* pOwnerTransform, CPhysicsCCT* pOwnerPhyCCT)
{
	if (pOwnerTransform)
	{
		m_vecAnimations[m_iPrevAnimIndex]->Update_BlendAnimation(pAnimEvalCS, fTimeDelta, nullptr, pOwnerPhyCCT);

		// animation 결과 blendCS에 bind
		pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_CURSRT),
			pAnimBlendCS->Get_SRV("MU_PRETRANSFORMS"), pAnimEvalCS->Get_Output_Buffer());
		
		m_vecAnimations[m_iCurrentAnimIndex]->Update_BlendAnimation(pAnimEvalCS, fTimeDelta, nullptr, pOwnerPhyCCT);

		// animation 결과 blendCS에 bind
		pAnimBlendCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(BLENDCS_SB_IDX::MU_CURSRT),
			pAnimBlendCS->Get_SRV("MU_CURTRANSFORMS"), pAnimEvalCS->Get_Output_Buffer());
	}

	Lerp_Animation(pAnimBlendCS, fRatio);

	// animation 결과 bone cs에 bind
	pBoneComBineCS->Bind_InputStructuredBuffer(ENUM_TO_UINT(CModel::CS_SB_IDX::MU_SRTS),
		pBoneComBineCS->Get_SRV("MU_SRTS"), pAnimBlendCS->Get_Output_Buffer());

	Update_BoneCombineTransformMatrix(pBoneComBineCS);
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
	_uint iGroupX = ((_uint)m_vecBones.size() + 31) / 32;
	pAnimBlendCS->Dispatch(iGroupX, 1, 1);
}

void CModel::Bind_BoneImmuData(CComputeShader* pBoneComBineCS)
{
	// bone 불변 데이터 넣어줌
	_uint iBoneNums = _uint(m_vecBones.size());

	CS_IMMU_BONE* pInitialData = new CS_IMMU_BONE[iBoneNums];
	
	for (size_t i = 0; i < m_vecBones.size(); i++)
	{
		pInitialData[i].iParentIndex = m_vecBones[i]->Get_ParentIndex();
		pInitialData[i].matPreTransform = m_matPreTransform;
		pInitialData[i].Padding0 = Vector3::Zero;
	}
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

	for (auto& pBoneGroup : m_vecBoneGroups)
	{
		Safe_Release(pBoneGroup.pIndexBuffer);

		if(!IsClone())
			Safe_Release(pBoneGroup.pInputGroupSB_SRV);
	}
	
	Safe_Release(m_pMasterMesh);

	m_vecBones.clear();
	m_vecMeshes.clear();
	m_vecMaterials.clear();
	m_vecAnimations.clear();
	m_vecMaterialInstances.clear();
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
