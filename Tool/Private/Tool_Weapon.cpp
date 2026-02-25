#include "pch.h"
#include "Tool_Weapon.h"
#include "Model.h"
#include "Bone.h"
#include "Shader.h"
#include "ComputeShader.h"

#include "GameInstance.h"

CTool_Weapon::CTool_Weapon(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Tool_PartObject(eType, pDevice, pDeviceContext)
{
}

CTool_Weapon::CTool_Weapon(const CTool_Weapon& rhs)
	:Tool_PartObject(rhs)
	, m_matRotation(rhs.m_matRotation)
	, m_tWeaponInfo(rhs.m_tWeaponInfo)
{
}

HRESULT CTool_Weapon::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Weapon::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);
	m_pMatSocket = pDesc->pMatSocket;
	m_eModleType = pDesc->eModel;

	m_tWeaponInfo.iSocketIdx = pDesc->iSocketIdx;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	// model과 desc 정보 다를때를 위한 방어
	if (Get_Component<CModel>()->Get_Type() == EModelType::NONANIM)
		m_eModleType = Weapon_ModelType::STATIC;

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
		if (FAILED(Ready_ComputeShaders()))
			return E_FAIL;

		CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));
		Get_Component<CModel>()->Change_Animation(pAnimECS, 0, false,true,true);
		break;
	}

	m_eState = State::HOLD;

	return S_OK;
}

HRESULT CTool_Weapon::Ready_Component(WEAPON_DESC* pArg)
{
	if (FAILED(Add_Component<CModel>(ENUM_TO_UINT(ELevelType::ANIMATION), pArg->wstrModelPrototypeName, pArg)))
		return E_FAIL;

	if (pArg->eModel == Weapon_ModelType::STATIC)
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", pArg)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_AnimMesh", pArg)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTool_Weapon::Ready_ComputeShaders()
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

	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBonCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	if (FAILED(Get_Component<CModel>()->Ready_ComputeShaders(pBoneMeshCS, pBonCombineCS, pAnimECS)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTool_Weapon::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTool_Weapon::Update_Priority(const _float fDT)
{
}

void CTool_Weapon::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		break;

	case Weapon_ModelType::ANIM:
		Play_Anim(fTimeDelta);
		break;
	}

	// scale이 죽었을때 다시 살리기 위함
	//if (CCollider* pCollider = Get_Component<CCollider>())
	//	pCollider->Update(Matrix::CreateScale(100.f, 100.f, 100.f) * m_matCombinedWorld);
}

void CTool_Weapon::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	

	Matrix matSR = Matrix::CreateScale(m_tWeaponInfo.vScale[0], m_tWeaponInfo.vScale[1], m_tWeaponInfo.vScale[2]) * Matrix::CreateFromYawPitchRoll(XMConvertToRadians(m_tWeaponInfo.vPYR[1]), XMConvertToRadians(m_tWeaponInfo.vPYR[0]), XMConvertToRadians(m_tWeaponInfo.vPYR[2]));
	m_matRotation = matSR * Matrix::CreateTranslation(m_tWeaponInfo.vTranslation[0], m_tWeaponInfo.vTranslation[1], m_tWeaponInfo.vTranslation[2]);
}

void CTool_Weapon::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	// none일때는 그리지 않음
	if (m_eState != State::NONE)
		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);

	// state에 따른 combineworld 업데이트
	switch (m_eState)
	{
	case State::HOLD:
		if(m_pMatSocket)
			Super::Update_CombinedWorldMatrix(m_matRotation * (*m_pMatSocket) * (*m_pMatParent));
		else
			Super::Update_CombinedWorldMatrix(m_matRotation * (*m_pMatParent));
		break;
	default:
		//Super::Update_CombinedWorldMatrix((*m_pMatSocket) * (*m_pMatParent));
		//Update_HoldingPos();
		break;
	}

}

HRESULT CTool_Weapon::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	HRESULT hr = {};
	switch (m_eModleType)
	{
	case Weapon_ModelType::STATIC:
		Render_StaticWeap();
		break;

	case Weapon_ModelType::ANIM:
		Render_AnimWeap();
		break;
	}

	return S_OK;
}

void CTool_Weapon::Play_Anim(const _float fTimeDelta)
{
	CComputeShader* pBonCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));
	CComputeShader* pAnimECS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_AnimE")));

	Get_Component<CModel>()->Update_Animation(pBonCS, pAnimECS, fTimeDelta);
}

HRESULT CTool_Weapon::Render_StaticWeap()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}


HRESULT CTool_Weapon::Render_AnimWeap()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();
	_uint				iMeshCount = pModel->Get_MeshCount();
	CComputeShader* pBoneMeshCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneMesh")));
	CComputeShader* pBoneCombineCS = static_cast<CComputeShader*>(Get_Script_Component(TEXT("ComputeShader_BoneCombine")));

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, pBoneMeshCS, pBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

void CTool_Weapon::Draw_ImGui()
{
}

void CTool_Weapon::Set_Soket(_uint iIdx, _bool bCombine)
{
	CModel* pParentModel = Get_Parent()->Get_Component<CModel>();

	if(bCombine)
		m_pMatSocket = &(pParentModel->Get_Bone(iIdx)->Get_CombinedTransformMatrix());

	else
		m_pMatSocket = &(pParentModel->Get_Bone(iIdx)->Get_BindPoseTransformMatrix());

	m_tWeaponInfo.iSocketIdx = iIdx;
}

void CTool_Weapon::Set_State(_uint iState)
{
	m_eState = static_cast<State>(iState);
}

void CTool_Weapon::Set_SRT(SRT eSRT, Vec3 vValue)
{
	switch (eSRT)
	{
	case SRT::Scale:
		m_tWeaponInfo.vScale[0] = vValue.x;
		m_tWeaponInfo.vScale[1] = vValue.y;
		m_tWeaponInfo.vScale[2] = vValue.z;
		break;

	case SRT::PYR:
		m_tWeaponInfo.vPYR[0] = vValue.x;
		m_tWeaponInfo.vPYR[1] = vValue.y;
		m_tWeaponInfo.vPYR[2] = vValue.z;
		break;

	case SRT::Translation:
		m_tWeaponInfo.vTranslation[0] = vValue.x;
		m_tWeaponInfo.vTranslation[1] = vValue.y;
		m_tWeaponInfo.vTranslation[2] = vValue.z;
		break;
	}
}

CTool_Weapon* CTool_Weapon::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTool_Weapon* pInstance = new CTool_Weapon(eType, pDevice, pDeviceContext);

	if (pInstance == nullptr)
	{
		MSG_BOX("Create to Fail : CTool_Weapon");
		Safe_Release(pInstance);
		return nullptr;
	}

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Initialize to Fail : CTool_Weapon");
		return nullptr;
	}

	return pInstance;
}

CGameObject* CTool_Weapon::Clone(void* pArg)
{
	CTool_Weapon* pClone = new CTool_Weapon(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CTool_Weapon::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}
void CTool_Weapon::Free()
{
	Super::Free();
}