#include "pch.h"
#include "InstanceModel.h"
#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "DataDocument_Map.h"
#include "Engine_Utils.h"
#include "GameInstance.h"

CInstanceModel::CInstanceModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(eType,pDevice,pContext)
{

}

CInstanceModel::CInstanceModel(const CInstanceModel& rhs)
	: CMapObject(rhs)
{
}

HRESULT Tool::CInstanceModel::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CInstanceModel::INSTANCEMODEL_DESC* pDesc = static_cast<CInstanceModel::INSTANCEMODEL_DESC*>(pArg);
	m_tData = pDesc->tData;

	m_strModelFileName = Engine_Utils::ToString(m_tData.tUsingModelInfo.wstrPath);
	Set_Name(m_tData.tUsingModelInfo.wstrName);
	

	for (auto& SRTData : pDesc->tData.vecOriginSRT)
		m_tData.vecMatirx.push_back(SRTData.Get_World());


	if (FAILED(CInstanceModel::Ready_Component()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Ready_Component()
{
	CModel::MODEL_COPY_DESC tDesc{};

	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh", nullptr)))
		return E_FAIL;

	CModel::MODEL_ORIGIN_DESC tModelDesc{};
	tModelDesc.eType = EModelType::STATIC;
	tModelDesc.wstrModelFolderName = m_tData.tUsingModelInfo.wstrPath;
	tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
	if (pModel)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + m_tData.tUsingModelInfo.wstrName , pModel)))
			Safe_Release(pModel);
	}
	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	Engine::CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + m_tData.tUsingModelInfo.wstrName, &tModelCopyDesc);

	CInstanceMesh::INSTANCEMESH_DESC tInstanceMeshDesc{};
	tInstanceMeshDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
	tInstanceMeshDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
	tInstanceMeshDesc.vecInstanceMatrixPointer = &m_tData.vecMatirx;
	if (FAILED(CGameObject::Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tInstanceMeshDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Awake(const _uint iCurrentLevelID)
{
	/* Instance Model 위치 값이 들어갈 예정 */
	/* 처음 생성시 위치값이 이미 들어감 */

	return S_OK;
}

void CInstanceModel::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CInstanceModel::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CInstanceModel::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CInstanceModel::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CInstanceModel::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader			= CGameObject::Get_Component<CShader>();
	CModel* pModel				= CGameObject::Get_Component<CModel>();
	CTransform* pTransform		= CGameObject::Get_Component<CTransform>();
	CInstanceMesh* pInstMesh	= CGameObject::Get_Component<CInstanceMesh>();

	if (pShader == nullptr || pModel == nullptr || pTransform == nullptr ) return E_FAIL;

	_uint iMeshCount		= pModel->Get_MeshCount();
	_uint iInstacnceCount	= pInstMesh->Get_InstanceCount();

	/* 1번 슬롯에 Instance 미리 바인딩 */
	pInstMesh->Bind_Instance(1);

	for (_uint i = 0; i < iMeshCount ; ++i)
	{
		pModel->Bind_Material(pShader, i);
		pModel->Bind_MaterialInstance(pShader, i);
		pShader->Apply();
		pModel->Render_Instance(i , iInstacnceCount);
	}

	pInstMesh->Unbind_Resource(1);


	return S_OK;
}

void CInstanceModel::Draw_ImGui()
{
}

CInstanceModel* CInstanceModel::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(eType, pDevice, pContext);

	if (FAILED(pInstanceModel->Initialize_Prototype()))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX(" Instance Model is Failed To Create ");
		return nullptr;
	}

	return pInstanceModel;
}


CGameObject* CInstanceModel::Clone(void* pArg)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(*this);

	if (FAILED(pInstanceModel->Initialize(pArg)))
	{
		Safe_Release(pInstanceModel);
		MSG_BOX("Instance Model Is Failed To Clone");
		return nullptr;
	}

	return pInstanceModel;
}

void CInstanceModel::Free()
{
	Super::Free();
}

_bool CInstanceModel::Picking(OUT Vec3& vOut)
{
	return false;
}

_bool CInstanceModel::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	if (eCategory != DTO::ECategory::MAP || pDocument == nullptr)
		return false;

	if (pDocument->Get_Category() != DTO::ECategory::MAP)
		return false;

	auto* pMapDocument = static_cast<CDataDocument_Map*>(pDocument);

	/* SRT 데이터들은 내가 들고 있는 SRT 데이터 들이다 */
	/* SRT 데이터들을 계산해서 넘겨준다 */
	/* 내가 지금 현재 들고있는 Matrix가 진짜 데이터? */


	DTO::TMap_InstanceModelData tSave_InstanceModleData{};
	tSave_InstanceModleData.strTag = m_strName + std::to_string(m_iObjectID);

	/* SRT */
	tSave_InstanceModleData.vecSRTData.resize(m_tData.vecOriginSRT.size());
	memcpy(tSave_InstanceModleData.vecSRTData.data(), m_tData.vecOriginSRT.data(), sizeof(SRT_DATA) * m_tData.vecOriginSRT.size());

	/* Using Material Info */
	tSave_InstanceModleData.tUsingModelInfo.wstrName = m_tData.tUsingModelInfo.wstrName;
	tSave_InstanceModleData.tUsingModelInfo.wstrPath = m_tData.tUsingModelInfo.wstrPath;
	tSave_InstanceModleData.tUsingModelInfo.wstrMtl_JsonFile_Path = m_tData.tUsingModelInfo.wstrMtl_JsonFile_Path;

	for (auto& UsingMaterial : m_tData.tUsingModelInfo.vecOverrideMaterial)
	{
		DTO::OVERRIDE_MATERIALS tSaveMtl{};
		tSaveMtl.isNull = UsingMaterial.isNull;
		tSaveMtl.wstrMtl_JsonFile_Name = UsingMaterial.wstrMtl_JsonFile_Name;
		tSaveMtl.wstrMtl_JsonFile_Path = UsingMaterial.wstrMtl_JsonFile_Path;
		tSaveMtl.vecUsingTextureInfo = UsingMaterial.vecUsingTextureInfo;
		tSave_InstanceModleData.tUsingModelInfo.vecOverrideMaterial.push_back(tSaveMtl);
	}

	pMapDocument->Try_Add(tSave_InstanceModleData);

	return true;
}
