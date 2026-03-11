#include "pch.h"
#include "SocketObject.h"

//components
#include "Shader.h"
#include "Model.h"
#include "ComputeShader.h"

// managers
#include "Engine_Utils.h"


CSocketObject::CSocketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pDevice, pDeviceContext)
{
}

CSocketObject::CSocketObject(const CSocketObject& rhs)
	:Super(rhs)
	, m_bAnimModel(rhs.m_bAnimModel)
{
}

HRESULT CSocketObject::Initialize_Prototype()
{
	if (FAILED(Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSocketObject::Initialize(void* pArg)
{
	if (FAILED(Initialize(pArg)))
		return E_FAIL;

	SOCEKT_DESC* pDesc = static_cast<SOCEKT_DESC*>(pArg);

	m_FSocketFlags	= pDesc->FSocketFlgas;
	m_pMatSocket	= pDesc->pMatSocket;

	// socket matrix null 검사
	if (m_pMatSocket == nullptr)
		Engine_Utils::RemoveHard_Flag(m_FSocketFlags, ENUM_TO_UINT(SOCEKT_FLAGS::Has_SocketMatrix));

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (m_bAnimModel)
	{
		if (FAILED(Ready_AnimModel(pDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSocketObject::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Awake(iCurrentLevelIndex)))
		return E_FAIL;

	return S_OK;
}

void CSocketObject::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CSocketObject::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_bAnimModel)
	{
		Update_Anim(fTimeDelta);
	}
}

void CSocketObject::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CSocketObject::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	 
	if(Engine_Utils::Has_Flag(m_FSocketFlags, ENUM_TO_UINT(SOCEKT_FLAGS::Has_SocketMatrix)))
		Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));
	else
		Super::Update_CombinedWorldMatrix((*m_pMatParent));
}

void CSocketObject::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	//Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CSocketObject::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	//Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CSocketObject::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CSocketObject::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CSocketObject::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

HRESULT CSocketObject::Render()
{
	if (m_bAnimModel)
	{
		Render_Anim();
	}

	else
	{
		Render_Static();
	}

	return S_OK;
}

HRESULT CSocketObject::Ready_Components(SOCEKT_DESC* pDesc)
{
	if (FAILED(Add_Component<CModel>(0/*static*/, pDesc->wstrModelPrototypeName, nullptr)))
		return E_FAIL;

	// 만약 static이라면
	if (Get_Component<CModel>()->Get_Type() == EModelType::STATIC)
	{
		m_bAnimModel = false;

		
		m_FSocketFlags &= ~ENUM_TO_UINT(SOCEKT_FLAGS::Model_Anim);
		m_FSocketFlags |=ENUM_TO_UINT(SOCEKT_FLAGS::Model_Static);
	}

	if (m_bAnimModel)
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", nullptr)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxAnimMesh", nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSocketObject::Ready_AnimModel(SOCEKT_DESC* pDesc)
{
	CModel* pModel = Get_Component<CModel>();

	_uint iBoneNums			= pModel->Get_BoneCount();
	_uint iGetBoneNums		= pModel->Get_StageBoneCount();
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

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneMesh", L"Prototype_Component_Shader_BoneMesh", &ShaderDesc, CAST_VOID_PP(&m_pBoneMeshCS))))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneCombine", L"Prototype_Component_Shader_BondCombine", &ShaderDesc, CAST_VOID_PP(&m_pBoneCombineCS))))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimE", L"Prototype_Component_Shader_AnimEv", &ShaderDesc, CAST_VOID_PP(&m_pBoneAnimEvaluateCS))))
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

		if (FAILED(Add_Script_Component(L"ComputeShader_AnimB", L"Prototype_Component_Shader_AnimB", &ShaderDesc, CAST_VOID_PP(&m_pBoneAnimBlendCS))))
			return E_FAIL;
	}

	if (FAILED(pModel->Ready_ComputeShaders(m_pBoneMeshCS, m_pBoneCombineCS, m_pBoneAnimEvaluateCS, m_pBoneAnimBlendCS)))
		return E_FAIL;

	if (Engine_Utils::Has_Flag(m_FSocketFlags, ENUM_TO_UINT(SOCEKT_FLAGS::RootMotion_RemoveAll)))
		pModel->Set_ApplyRootMotionAll(false);

	return S_OK;
}

void CSocketObject::Update_Anim(const _float fTimeDelta)
{
	Get_Component<CModel>()->Update_Animation(m_pBoneCombineCS, m_pBoneAnimEvaluateCS, fTimeDelta, nullptr, nullptr, m_pBoneAnimBlendCS);
}

HRESULT CSocketObject::Render_Static()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

HRESULT CSocketObject::Render_Anim()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint				iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

CSocketObject* CSocketObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSocketObject* pInsatnce = new CSocketObject(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CSocketObject::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CSocketObject::Clone(void* pArg)
{
	CSocketObject* pClone = new CSocketObject(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CSocketObject::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CSocketObject::Free()
{
	Super::Free();
}
