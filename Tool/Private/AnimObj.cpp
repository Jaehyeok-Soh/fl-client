#include "pch.h"
#include "AnimObj.h"

#include "Model.h"
#include "Shader.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"

#include "ComputeShader.h"

CAnimObj::CAnimObj(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(eType, pDevice, pDeviceContext)
{
}

CAnimObj::CAnimObj(const CAnimObj& rhs)
	: Super(rhs)
{
}

HRESULT CAnimObj::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	ANIMOBJ_DESC* pDesc = static_cast<ANIMOBJ_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_ComputeShaders()))
		return E_FAIL;

	if (FAILED(Ready_CCT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CAnimObj::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CAnimObj::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CAnimObj::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CAnimObj::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

HRESULT CAnimObj::Render()
{
	CShader* pShader	= Get_Component<CShader>();
	CModel* pModel		= Get_Component<CModel>();
	_uint	iMeshCount	= pModel->Get_MeshCount();

	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBoneCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));

	pShader->Bind_TransformData(Get_Component<CTransform>()->Get_WorldMatrix());
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, pBoneMeshCS, pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

_bool CAnimObj::Picking(OUT Vec3& vOut)
{
	return _bool();
}

void CAnimObj::Draw_ImGui()
{
}

void CAnimObj::Set_Dead(const wstring& wstrLayerTag)
{
	Super::Set_Dead(wstrLayerTag);
}

const CModel* CAnimObj::Get_ModelComPtr()
{
	return Get_Component<CModel>();
}

HRESULT CAnimObj::Change_ModelCom(_wstring wstrModelTag)
{
	Change_Component<CModel>(static_cast<CModel*>(m_pGameInstance->
		Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::ANIMATION), wstrModelTag)));

	/* 만약 model이 없다면 */
	if (Get_Component<CModel>() == nullptr)
	{
		MSG_BOX("Fail to Change Model Component");
		return E_FAIL;
	}

	return S_OK;
}

_uint CAnimObj::Get_PartsNums()
{
	return static_cast<_uint>(Get_PartList().size());
}

HRESULT CAnimObj::Ready_Components(ANIMOBJ_DESC* pDesc)
{
	/* model componenet */
	if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::ANIMATION), pDesc->wstrModelProtoTag, pDesc)))
		return E_FAIL;

	/* Prototype_Component_Shader_AnimMesh */
	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_AnimMesh", nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Ready_CCT()
{
	PHYSICSCCT_DESC desc;
	desc.pOwner = this;
	desc.bIsPlayer = true;
	desc.eType = EPhysicsCCTType::CAPSULE;
	desc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
	desc.fRadius = 0.5f;
	desc.fHeight = 1.f;
	desc.vExtens = { 0.f, 0.f, 0.f };

	PHYSICSMATERIAL_DESC mtrlDesc{};
	mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
	desc.tMaterial = mtrlDesc;

	desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::PLAYER;
	desc.iFilterMask = PHYSICSFILTERGROUP::Enum::NONE;

	if (FAILED(Add_Component<CPhysicsCCT>(0, L"Prototype_Component_Physics_CCT", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimObj::Ready_ComputeShaders()
{
	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();
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

CAnimObj* CAnimObj::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CAnimObj* pInstance = new CAnimObj(eType, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX(" CAnimObj Is Failed To Create ");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CAnimObj::Clone(void* pArg)
{
	CAnimObj* pInstance = new CAnimObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CAnimObj Is Failed To Create ");
		return nullptr;
	}

	return pInstance;
}

void CAnimObj::Free()
{
	Super::Free();
}