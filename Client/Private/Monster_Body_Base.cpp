#include "pch.h"
#include "Client_Defines.h"
#include "Monster_Body_Base.h"

#include "Monster_Base.h"

#include "Bone.h"

// components
#include "ActionState.h"
#include "Shader.h"
#include "Model.h"
#include "ComputeShader.h"
#include "PhysicsCCT.h"
#include "EffectHandler.h"

#include "UI_Manager.h"
#include "GameInstance.h"

CMonster_Body_Base::CMonster_Body_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CMonster_Body_Base::CMonster_Body_Base(const CMonster_Body_Base& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Body_Base::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Body_Base::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MONSTERBODY_DESC* pDesc = static_cast<MONSTERBODY_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;

	if (FAILED(Ready_Bones(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_EffectHandler(pDesc)))
		return E_FAIL;

	Set_Flag(OF_Outline, true);
	return S_OK;
}

HRESULT CMonster_Body_Base::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	if (m_pEffectHandler)
		m_pEffectHandler->Awake();

	return S_OK;
}

void CMonster_Body_Base::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Body_Base::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	CComputeShader* pBonCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));
	CComputeShader* pAnimBCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimB")));
	CComputeShader* pAnimMix = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimMix")));

	Get_Component<CModel>()->Update_Animation(pBonCS, pAnimECS, fTimeDelta,
		Get_Parent()->Get_Component<CTransform>(), Get_Parent()->Get_Component<CPhysicsCCT>(), pAnimBCS, pAnimMix);
}

void CMonster_Body_Base::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_pEffectHandler)
		m_pEffectHandler->Update(fTimeDelta);
}

void CMonster_Body_Base::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
	Super::Update_CombinedWorldMatrix(m_pMatParent);
}

void CMonster_Body_Base::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Body_Base::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO &tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Body_Base::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Body_Base::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Body_Base::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Body_Base::On_Hit(const HIT_DESC& hitDesc)
{
	return Get_Parent()->On_Hit(hitDesc);
}

HRESULT CMonster_Body_Base::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBoneCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, pBoneMeshCS, pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

CBone* CMonster_Body_Base::Get_Bone(CMonster_Body_Base::EBone eBone)
{
	_uint iIndex = ENUM_TO_UINT(eBone);
	if (m_vecBoneIndices.size() <= iIndex)
		return nullptr;

	return Get_Component<CModel>()->Get_Bone(m_vecBoneIndices[iIndex]);
}

const Matrix* CMonster_Body_Base::Get_SocketMatrix(const _char* szBoneName)
{
	if (CBone* pReturn = Get_Component<CModel>()->Get_Bone(szBoneName))
	{
		return &pReturn->Get_CombinedTransformMatrix();
	}

	return nullptr;
}

const Matrix* CMonster_Body_Base::Get_SocketMatrix(_uint iIndex)
{
	if (CBone* pReturn = Get_Component<CModel>()->Get_Bone(iIndex))
	{
		return &pReturn->Get_CombinedTransformMatrix();
	}

	return nullptr;
}

HRESULT CMonster_Body_Base::Ready_Components(MONSTERBODY_DESC* pDesc)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeTag, nullptr)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Body_Base::Ready_EffectHandler(MONSTERBODY_DESC* pDesc)
{
	wstring NameTag = pDesc->wstrModelPrototypeTag;
	Engine_Utils::Replace(NameTag, L"Prototype_Component_Model_", L"");

	if (FAILED(Add_Component<CEffectHandler>(/*Static*/0, L"Prototype_Component_EffectHandler_" + NameTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Body_Base::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CMonster_Body_Base::Ready_ComputeShader()
{
	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();
	_uint iGetBoneNums = Get_Component<CModel>()->Get_StageBoneCount();
	// ========   Compute Shader : BoneMesh  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONEFNIMAL_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_BONEDATA";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_BONE);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BONEFNIMAL_TRANSFORMS";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneMesh", L"Prototype_Component_Shader_BoneMesh", &ShaderDesc)))
			return E_FAIL;
	}

	// ========   Compute Shader : BoneCombine  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONECOMBINED_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum = 3;
		// 입력 버퍼
		ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_BONEDATA";
		ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_BONE);
		ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BONECOMBINED_TRANSFORMS";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneCombine", L"Prototype_Component_Shader_BondCombine", &ShaderDesc)))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimE  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "CHANNEL_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = m_tEffectDesc._Effect_MaxParticle;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "CHANNEL_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimE", L"Prototype_Component_Shader_AnimEv", &ShaderDesc)))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimB  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BLEND_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 2;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BLEND_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimB", L"Prototype_Component_Shader_AnimB", &ShaderDesc)))
			return E_FAIL;
	}

	// ========   Compute Shader : AnimMix  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "CHANNEL_OUTPUT_SRV";

		ShaderDesc.InputBufferNum = 4;
		ShaderDesc.bMakeSB = false;
		//// 입력 버퍼
		//ShaderDesc.Input_StructBuffer.sBufferName = "IMMU_EFFECT_PARTICLE";
		//ShaderDesc.Input_StructBuffer.iElementSize = sizeof(EFFECT_PARTICLE_IMMU_ELEMENT);
		//ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "CHANNEL_OUTPUT";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_SRT);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimMix", L"Prototype_Component_Shader_AnimMix", &ShaderDesc)))
			return E_FAIL;
	}

	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBonCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));
	CComputeShader* pAnimBlendCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimB")));
	CComputeShader* pAnimMix = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimMix")));

	if (FAILED(Get_Component<CModel>()->Ready_ComputeShaders(pBoneMeshCS, pBonCombineCS, pAnimECS, pAnimBlendCS, pAnimMix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Body_Base::Ready_Bones(MONSTERBODY_DESC* pDesc)
{
	const auto& boneNames = pDesc->spanBoneNames;
	if (boneNames.size() <= 0)
		return S_OK;

	CModel* pModel = Get_Component <CModel>();
	if (pModel == nullptr)
		return E_FAIL;

	m_vecBoneIndices.resize(ENUM_TO_UINT(EBone::END));
	for (size_t i = 0; i < boneNames.size(); ++i)
	{
		EBone eBone = static_cast<EBone>(boneNames[i].first);
		if (eBone >= EBone::END)
			return E_FAIL;

		_int iBoneIndex = pModel->Get_BoneIndex(boneNames[i].second.c_str());
		if (iBoneIndex == -1)
			return E_FAIL;

		m_vecBoneIndices[ENUM_TO_UINT(eBone)] = iBoneIndex;
	}

	return S_OK;
}

void CMonster_Body_Base::Free()
{
	Super::Free();
}
