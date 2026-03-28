#include "pch.h"
#include "BonePart.h"
// components
#include "Model.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "GameInstance.h"


CBonePart::CBonePart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EToolObjectType::ANIMATION,pDevice, pDeviceContext)
{
}

CBonePart::CBonePart(const CBonePart& rhs)
	:Super(rhs)
{
}

HRESULT CBonePart::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBonePart::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	BONEPART_DESC* pDesc = static_cast<BONEPART_DESC*>(pArg);

	m_FFlags = pDesc->FFlags;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_ComputeShaders(pDesc)))
		return E_FAIL;

	// CascadeBuffer Shader에 연결
	if (FAILED(m_pGameInstance->Set_CascadeShadowConstantBuffer(Get_Component<CShader>())))
		return E_FAIL;

	Set_RenderInfoFlag(OF_Outline, true);
	Set_RenderInfoFlag(OF_Rim, true);

	return S_OK;

}

HRESULT CBonePart::Change_Model(const wstring& wstrModelFolderName, _uint iAddModelPrototypeLevelType, _uint iAddModelType, Matrix PreMatrix,_int iRootBoneIndex)
{
	/* Hair Part 전용이긴해..  */
	CModel::MODEL_ORIGIN_DESC tOriginDesc;
	tOriginDesc.iPrototypeLevelIndex	= iAddModelPrototypeLevelType;
	tOriginDesc.pMatPreTransform		= &PreMatrix;
	tOriginDesc.wstrModelFolderName		= wstrModelFolderName;
	tOriginDesc.eType					= EModelType(iAddModelType);

	CModel::DATA_ANIMCHANNEL tChannelData{};
	if (tOriginDesc.eType == EModelType::ANIM)
	{
		tChannelData.iRootBoneIndex = iRootBoneIndex;
		tOriginDesc.pAniChannelData = &tChannelData;
	}
	tChannelData.iRootBoneIndex = iRootBoneIndex; 

	if (FAILED(m_pGameInstance->Add_Prototype(iAddModelPrototypeLevelType, L"Prototype_Component_Model_" + wstrModelFolderName, CModel::Create(m_pDevice, m_pDeviceContext, &tOriginDesc))))
		return E_FAIL;

	if (FAILED(Add_Component<CModel>(iAddModelPrototypeLevelType, L"Prototype_Component_Model_" + wstrModelFolderName ,nullptr)))
		return E_FAIL;

	CModel* pModel =  Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;

	for (_uint i = 0; i < pModel->Get_MaterialCount(); i++)
	{
		pModel->Change_MI(i, EMaterialInstanceType::Free);
	}


	return S_OK;
}

HRESULT CBonePart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;

	if (Engine_Utils::Has_Flag(m_FFlags, ENUM_TO_UINT(BonePartFlag::VSShakeOn)))
		Get_Component<CShader>()->Set_Pass(3);

	return S_OK;
}

void CBonePart::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBonePart::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Get_Component<CModel>()->Update_PartModel(m_pParentBoneCombineCS, m_pPartBoneCombineCS);
}

void CBonePart::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CBonePart::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	Super::Update_CombinedWorldMatrix((*m_pMatParent));

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

void CBonePart::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CBonePart::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBonePart::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CBonePart::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CBonePart::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CBonePart::Render()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();

	_uint	iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);


	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Set_MI(i,m_tMIDesc);
		pModel->Bind_MaterialInstance(pShader, i);
		pModel->Bind_Material(pShader, i);
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pPartBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

HRESULT CBonePart::Render_Shadow()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = Get_Component<CShader>();
	_uint iPrevPass = pShader->Get_CurrentPass();

	constexpr _uint iShadowPass = 4;

	// Set Shadow Pass
	pShader->Set_Pass(iShadowPass);
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	pShader->Bind_TransformData(m_CombineWorldMatrix);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Bones(pShader, i, m_pBoneMeshCS, m_pPartBoneCombineCS);
		pShader->Apply();
		pModel->Render(i);
	}

	pShader->Set_Pass(iPrevPass);
	return S_OK;
	return S_OK;
}

void CBonePart::Update_PartsModel()
{
	CModel* pModel =  Get_Component<CModel>();

	if(pModel)
		pModel->Update_PartModel(m_pParentBoneCombineCS, m_pPartBoneCombineCS);

	return;
}

HRESULT CBonePart::Ready_Components(BONEPART_DESC* pDesc)
{
	Change_Model(pDesc->wstrModelPrototypeName, ENUM_TO_UINT(pDesc->iAddModelPrototypeLevel),
		ENUM_TO_UINT(pDesc->eAddModelType), pDesc->ModelPreMatrix, pDesc->iAddModelRootBoneIndex);

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_AnimMesh", nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBonePart::Ready_ComputeShaders(BONEPART_DESC* pDesc)
{

	_uint iBoneNums = Get_Component<CModel>()->Get_BoneCount();

	m_pParentBoneCombineCS = pDesc->pParentBoneCombineCS;

	if (m_pParentBoneCombineCS == nullptr)
		return E_FAIL;

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

	// ========   Compute Shader : BoneCombinePart  ========
	{
		CComputeShader::ComShaderCopyDesc ShaderDesc = {};
		ShaderDesc.Output_SRVBuffer_Name = "BONECOMBINED_TRANSFORMS_SRV";

		ShaderDesc.InputBufferNum = 2;
		// 입력 버퍼
		ShaderDesc.Input_StructBuffer.sBufferName = "BONEINDEXES_DATA";
		ShaderDesc.Input_StructBuffer.iElementSize = sizeof(CS_IMMU_PARTBONE);
		ShaderDesc.Input_StructBuffer.iNumElements = iBoneNums;

		// 출력 버퍼
		ShaderDesc.OutPut_StructBuffer.sBufferName = "BONECOMBINED_TRANSFORMS";
		ShaderDesc.OutPut_StructBuffer.iElementSize = sizeof(CS_OUT_BONE);
		ShaderDesc.OutPut_StructBuffer.iNumElements = iBoneNums;

		if (FAILED(Add_Script_Component(L"ComputeShader_BoneCombinePart", L"Prototype_Component_Shader_PartBoneCombine", &ShaderDesc, CAST_VOID_PP(&m_pPartBoneCombineCS))))
			return E_FAIL;
	}

	if (FAILED(Get_Component<CModel>()->Ready_PartComputeShaders(m_pBoneMeshCS, m_pPartBoneCombineCS, pDesc->pParentModel)))
		return E_FAIL;

	return S_OK;
}

CBonePart* CBonePart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBonePart* pInsatnce = new CBonePart(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CBonePart::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CBonePart::Clone(void* pArg)
{
	CBonePart* pClone = new CBonePart(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CBonePart::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CBonePart::Free()
{
	Super::Free();
}
