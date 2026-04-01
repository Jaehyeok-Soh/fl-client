#include "pch.h"
#include "CitizenPart.h"
// components
#include "Model.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "GameInstance.h"


CCitizenPart::CCitizenPart(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(EToolObjectType::ANIMATION, pDevice, pDeviceContext), m_pBoneSocket{nullptr}
{
}

CCitizenPart::CCitizenPart(const CCitizenPart& rhs)
	:Super(rhs)
	, m_pBoneSocket{rhs.m_pBoneSocket }

{
}

HRESULT CCitizenPart::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;




	return S_OK;
}

HRESULT CCitizenPart::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CCitizenPart::CITIZENPART_DESC* pDesc = static_cast<CCitizenPart::CITIZENPART_DESC*>(pArg);

	m_pBoneSocket = pDesc->pBoneSocket;
	m_tMIDesc.vTintColor = pDesc->tTintColor;


	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;


	return S_OK;
}


HRESULT CCitizenPart::Ready_Components(CITIZENPART_DESC* pDesc)
{
	if (FAILED(Change_Model(pDesc->wstrPartModelFolderName)))
		return E_FAIL;

	if (FAILED(Add_Component<CShader>(0/*static*/, L"Prototype_Component_Shader_VtxMesh_Tool", nullptr)))
		return E_FAIL;

	return S_OK;
}


HRESULT CCitizenPart::Change_Model(const wstring& wstrModelFolderName)
{
	CGameObject::Remove_Component<CModel>();

	const wstring& wstrModelPrototypeTag = L"Prototype_Component_Model_" + wstrModelFolderName;

	/* Hair Part 전용이긴해..  */
	CModel::MODEL_ORIGIN_DESC tOriginDesc;
	tOriginDesc.iPrototypeLevelIndex	= ENUM_TO_UINT(ELevelType::MAP);
	tOriginDesc.wstrModelFolderName		= wstrModelFolderName;
	tOriginDesc.eType					= EModelType::STATIC;

	if (nullptr == (m_pGameInstance->Find_Prototype(tOriginDesc.iPrototypeLevelIndex, wstrModelPrototypeTag)))
	{
		m_pGameInstance->Add_Prototype(tOriginDesc.iPrototypeLevelIndex, wstrModelPrototypeTag , CModel::Create(m_pDevice, m_pDeviceContext, &tOriginDesc));
	}


	if (FAILED(Add_Component<CModel>(tOriginDesc.iPrototypeLevelIndex, wstrModelPrototypeTag, nullptr)))
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



HRESULT CCitizenPart::Awake(const _uint iCurrentLevelIndex)
{
	if (FAILED(Super::Awake(iCurrentLevelIndex)))
		return E_FAIL;


	return S_OK;
}

void CCitizenPart::Update_Priority(_float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CCitizenPart::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	//Get_Component<CModel>()->Update_PartModel(m_pParentBoneCombineCS, m_pPartBoneCombineCS);
}

void CCitizenPart::Update_Late(_float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CCitizenPart::Ready_Before_Render(_float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	/* m_pBoneSoket */
	Super::Update_CombinedWorldMatrix((*m_pBoneSocket) * (*m_pMatParent));

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

void CCitizenPart::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CCitizenPart::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CCitizenPart::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CCitizenPart::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Get_Parent()->OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CCitizenPart::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Get_Parent()->OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

HRESULT CCitizenPart::Render()
{
	CShader* pShader = Get_Component<CShader>();
	CModel* pModel = Get_Component<CModel>();

	if (pShader == nullptr) return S_OK;
	if (pModel == nullptr) return S_OK;
	_uint	iMeshCount = pModel->Get_MeshCount();

	pShader->Bind_ObjectInfoData(m_tObjectInfoDesc);
	pShader->Bind_TransformData(m_CombineWorldMatrix);

	/* ---------------------------- */


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


void CCitizenPart::Update_PartsModel()
{

	return;
}


CCitizenPart* CCitizenPart::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCitizenPart* pInsatnce = new CCitizenPart(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CCitizenPart::Create, Failed");
		Safe_Release(pInsatnce);
	}
	return pInsatnce;
}

CGameObject* CCitizenPart::Clone(void* pArg)
{
	CCitizenPart* pClone = new CCitizenPart(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CCitizenPart::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CCitizenPart::Free()
{
	Super::Free();

}
