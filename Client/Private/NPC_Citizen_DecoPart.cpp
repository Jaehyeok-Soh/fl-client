#include "pch.h"
#include "NPC_Citizen_DecoPart.h"
#include "CitizenData.h"
// components
#include "Model.h"
#include "Shader.h"
#include "GameInstance.h"


CNPC_Citizen_DecoPart::CNPC_Citizen_DecoPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext), m_pBoneSocket{ nullptr }
	, m_tMIDesc{}
{
}

CNPC_Citizen_DecoPart::CNPC_Citizen_DecoPart(const CNPC_Citizen_DecoPart& rhs)
	:Super(rhs)
	, m_pBoneSocket{ rhs.m_pBoneSocket }
	, m_tMIDesc{rhs.m_tMIDesc }
{
}

HRESULT CNPC_Citizen_DecoPart::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;




	return S_OK;
}

HRESULT CNPC_Citizen_DecoPart::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CNPC_Citizen_DecoPart::NPC_CITIZEN_DECOPARTDESC * pDesc = static_cast<CNPC_Citizen_DecoPart::NPC_CITIZEN_DECOPARTDESC*>(pArg);

	m_pBoneSocket = pDesc->pBoneSocket;
	m_tMIDesc.vTintColor = pDesc->tTintColor;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	// CascadeBuffer Shader에 연결
	if (FAILED(m_pGameInstance->Set_CascadeShadowConstantBuffer(Get_Component<CShader>())))
		return E_FAIL;
	return S_OK;

}

HRESULT CNPC_Citizen_DecoPart::Change_Model(const wstring& wstrModelFolderName)
{
	CGameObject::Remove_Component<CModel>();

	const wstring& wstrCitizenPartTag = L"NPC_Citizen_Parts/";

	/* Hair Part 전용이긴해..  */
	CModel::MODEL_ORIGIN_DESC tOriginDesc;
	tOriginDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
	tOriginDesc.wstrModelFolderName = wstrCitizenPartTag + wstrModelFolderName;
	tOriginDesc.eType = EModelType::STATIC;


	if (nullptr == m_pGameInstance->Find_Prototype(tOriginDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + wstrModelFolderName))
	{
		m_pGameInstance->Add_Prototype(tOriginDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + wstrModelFolderName, CModel::Create(m_pDevice, m_pDeviceContext, &tOriginDesc));
	}

	if (FAILED(Add_Component<CModel>(tOriginDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + wstrModelFolderName, nullptr)))
		return E_FAIL;

	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;

	for (_uint i = 0; i < pModel->Get_MaterialCount(); i++)
	{
		pModel->Change_MI(i, EMaterialInstanceType::Free);
	}


	Vec3 vOffsetPos = DTO::Get_CitizenPartsOffset_ByFolderPath(wstrModelFolderName);

	/* Offset 바로적용 */
	Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, vOffsetPos);

	return S_OK;
}

HRESULT CNPC_Citizen_DecoPart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;


	return S_OK;
}

void CNPC_Citizen_DecoPart::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CNPC_Citizen_DecoPart::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CNPC_Citizen_DecoPart::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CNPC_Citizen_DecoPart::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	/* m_pBoneSoket */
	Super::Update_CombinedWorldMatrix((*m_pBoneSocket) * (*m_pMatParent));

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

void CNPC_Citizen_DecoPart::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Citizen_DecoPart::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Citizen_DecoPart::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CNPC_Citizen_DecoPart::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CNPC_Citizen_DecoPart::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CNPC_Citizen_DecoPart::Render()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();

	if (pShader == nullptr) return S_OK;
	if (pModel == nullptr) return S_OK;
	_uint	iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_matCombinedWorld);

	pShader->Set_Pass((_uint)EMapObjectShaderPass::StaticObject);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pModel->Set_MI(i, m_tMIDesc);
		pModel->Bind_MaterialInstance(pShader, i);
		pModel->Bind_Material(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}

HRESULT CNPC_Citizen_DecoPart::Render_Shadow()
{
	CShader* pShader = Get_Component<CShader>();
	_uint iPrevPass = pShader->Get_CurrentPass();

	_uint iShadowPass = 15;

	// Set Shadow Pass
	pShader->Set_Pass(iShadowPass);
	CModel* pModel = Get_Component<CModel>();
	_uint iMeshCount = pModel->Get_MeshCount();
	pShader->Bind_TransformData(m_matCombinedWorld);
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		pShader->Apply();
		pModel->Render(i);
	}

	pShader->Set_Pass(iPrevPass);
	return S_OK;
}

void CNPC_Citizen_DecoPart::Update_PartsModel()
{

	return;
}

HRESULT CNPC_Citizen_DecoPart::Ready_Components(NPC_CITIZEN_DECOPARTDESC* pDesc)
{
	if (FAILED(Change_Model(pDesc->wstrPartModelFolderName)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh", nullptr)))
		return E_FAIL;

	return S_OK;
}

CNPC_Citizen_DecoPart* CNPC_Citizen_DecoPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPC_Citizen_DecoPart* pInsatnce = new CNPC_Citizen_DecoPart(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CNPC_Citizen_DecoPart::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CNPC_Citizen_DecoPart::Clone(void* pArg)
{
	CNPC_Citizen_DecoPart* pClone = new CNPC_Citizen_DecoPart(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CNPC_Citizen_DecoPart::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CNPC_Citizen_DecoPart::Free()
{
	Super::Free();
}

