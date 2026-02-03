#include "pch.h"
#include "StaticModel.h"
#include "Mesh.h"
#include "Model.h"
#include "GameInstance.h"
#include "Shader.h"
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"

CStaticModel::CStaticModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(eType, pDevice,pDeviceContext)
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

	m_tData = pDesc->tData;

	m_strModelFileName = Engine_Utils::ToString(m_tData.tUsingModelInfo.wstrPath);
	Set_Name(m_tData.tUsingModelInfo.wstrName);

	if (FAILED(CStaticModel::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Ready_Component()
{
	CModel::MODEL_COPY_DESC tDesc{};

	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxMesh", nullptr)))
		return E_FAIL;

	/* CStatic_Model Type ÀÌ¶ó¸é */
	if (m_eMapObjectType == EMapObject_Type::STATICMODEL)
	{
		CModel::MODEL_ORIGIN_DESC tModelDesc{};
		tModelDesc.eType = EModelType::STATIC;
		tModelDesc.wstrModelFolderName = Engine_Utils::ToWString(m_strModelFileName);
		tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
		CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
		if (pModel)
		{
			if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), pModel)))
				Safe_Release(pModel);
		}
		CModel::MODEL_COPY_DESC tModelCopyDesc{};
		CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + Engine_Utils::ToWString(m_strName), &tModelCopyDesc);
	}

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

void CStaticModel::Draw_ImGui()
{
	Super::Draw_ImGui();

	ImGui::SeparatorText(" Static Model Info ");

	ImGui::NewLine();

	ImGui::Text(" Type : [%s]" , StaticModelType_ToString(m_eType).c_str()) ;
}

void CStaticModel::Set_Dead(const wstring& wstrLayerTag)
{

}

bool CStaticModel::IntsersectWithPlane(OUT Vec3& vOut)
{
	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr)  return false;

	_uint iMeshCount = pModel->Get_MeshCount();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (pModel->Get_Mesh(i)->IntsersectWithPlane(vOut))
			return true;
	}
	return false;
}

_bool CStaticModel::Picking(OUT Vec3& vOut)
{
	const Matrix& matWorld = Get_Component<CTransform>()->Get_WorldMatrix();
	Matrix matLocal = {};

	matLocal = matWorld.Invert();
	m_pGameInstance->TransformRayToLocalSpace(matLocal);

	if (IntsersectWithPlane(vOut))
	{
		vOut = Vec3::Transform(vOut, matWorld);
		return true;
	}
	return false;
}

_bool CStaticModel::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	if (eCategory != DTO::ECategory::MAP || pDocument == nullptr)
		return false;

	if (pDocument->Get_Category() != DTO::ECategory::MAP)
		return false;

	auto* pMapDocument = static_cast<CDataDocument_Map*>(pDocument);

	CTransform* pTs = Get_Component<CTransform>();
	Matrix WorldMatrix = pTs->Get_WorldMatrix();


	DTO::TMap_StaticModelData tSave_StaticModleData{};
	tSave_StaticModleData.strTag = m_strName + std::to_string(m_iObjectID);

	/* SRT */
	WorldMatrix.Decompose(tSave_StaticModleData.tSRTData.vScale, tSave_StaticModleData.tSRTData.vQuat, tSave_StaticModleData.tSRTData.vPosition);


	/* Using Material Info */
	tSave_StaticModleData.tUsingModelInfo.wstrName = m_tData.tUsingModelInfo.wstrName;
	tSave_StaticModleData.tUsingModelInfo.wstrPath = m_tData.tUsingModelInfo.wstrPath;
	tSave_StaticModleData.tUsingModelInfo.wstrMtl_JsonFile_Path;

	for (auto& UsingMaterial : m_tData.tUsingModelInfo.vecMaterialInfo)
	{
		DTO::USING_MATERIAL_INFO tSaveMtl{};
		tSaveMtl.isNull = UsingMaterial.isNull;
		tSaveMtl.wstrOriginMtl_JsonFile_Name = UsingMaterial.wstrOriginMtl_JsonFile_Name;
		tSaveMtl.wstrOriginMtl_JsonFile_Path = UsingMaterial.wstrOriginMtl_JsonFile_Path;
		tSaveMtl.vecUsingTextureInfo = UsingMaterial.vecUsingTextureInfo;
	}


	pMapDocument->Try_Add(tSave_StaticModleData);

	return true;
}

CStaticModel* CStaticModel::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStaticModel* pStaticModel = new CStaticModel(eType,pDevice,pDeviceContext);

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
