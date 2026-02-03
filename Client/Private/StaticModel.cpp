#include "pch.h"
#include "StaticModel.h"
#include "Model.h"
#include "GameInstance.h"
#include "Shader.h"


CStaticModel::CStaticModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice, pDeviceContext)
{
	m_eMapObjectType = EMapObject_Type::STATICMODEL;
}

CStaticModel::CStaticModel(const CStaticModel& rhs)
	: CMapObject(rhs)
{

}

HRESULT CStaticModel::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	STATICMODEL_DESC* pDesc = static_cast<STATICMODEL_DESC*>(pArg);

	m_strName = Engine_Utils::ToString(pDesc->tUsingModelInfo.wstrName) + std::to_string(m_iObjectID);

	if (FAILED(CStaticModel::Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(CStaticModel::Change_OverrideMtl(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Ready_Component(STATICMODEL_DESC* pDesc)
{
	CModel::MODEL_COPY_DESC tDesc{};

	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
		return E_FAIL;

	/* CStatic_Model Type 이라면 */
	if (m_eMapObjectType == EMapObject_Type::STATICMODEL)
	{
		CModel::MODEL_ORIGIN_DESC tModelDesc{};
		tModelDesc.eType = EModelType::STATIC;
		tModelDesc.wstrModelFolderName   = pDesc->tUsingModelInfo.wstrPath;
		tModelDesc.iPrototypeLevelIndex = pDesc->iLevelIndex;
		CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);

		if (pModel)
		{
			if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tUsingModelInfo.wstrName, pModel)))
				Safe_Release(pModel);
		}
		CModel::MODEL_COPY_DESC tModelCopyDesc{};
		CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tUsingModelInfo.wstrName, &tModelCopyDesc);
	}
	return S_OK;
}

HRESULT CStaticModel::Change_OverrideMtl(STATICMODEL_DESC* pDesc)
{
	if (pDesc == nullptr) return E_FAIL;

	/* 없으면 리턴 */
	if (pDesc->tUsingModelInfo.vecOverrideMaterial.empty()) return S_OK;

	CModel* pModel = CGameObject::Get_Component<CModel>();
	if (pModel == nullptr) return E_FAIL;

	/* 머테리얼 생성하고 Change Material 불릴 예정 */


	return S_OK;
}

HRESULT CStaticModel::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;


	return S_OK;
}

void CStaticModel::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}


void CStaticModel::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CStaticModel::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

}

void CStaticModel::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);
}

HRESULT CStaticModel::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;


	CModel* pModel = CGameObject::Get_Component<CModel>();
	CShader* pShader = CGameObject::Get_Component<CShader>();

	if (!pModel || !pShader) return S_OK;

	pShader->Bind_TransformData(CGameObject::Get_Component<CTransform>()->Get_WorldMatrix());

	UINT32 iMeshCount = pModel->Get_MeshCount();


	for (UINT32 i = 0; i < iMeshCount; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render(i);
	}

	return S_OK;
}



CStaticModel* CStaticModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStaticModel* pStaticModel = new CStaticModel(pDevice, pDeviceContext);

	if (FAILED(pStaticModel->Initialize_Prototype()))
	{
		Safe_Release(pStaticModel);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}
	return pStaticModel;
}

CGameObject* CStaticModel::Clone(void* pArg)
{
	CStaticModel* pStaticModel = new CStaticModel(*this);

	if (FAILED(pStaticModel->Initialize(pArg)))
	{
		Safe_Release(pStaticModel);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}

	return pStaticModel;
}


void CStaticModel::Free()
{
	Super::Free();
}
