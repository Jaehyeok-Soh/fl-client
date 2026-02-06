#include "pch.h"
#include "InstanceModel.h"
#include "Shader.h"
#include "Model.h"
#include "InstanceMesh.h"
#include "DataDocument_Map.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
#include "Mesh.h"

CInstanceModel::CInstanceModel(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(eType, pDevice, pContext), m_iSelectInstanceID{0}
{
	m_eMapObjectType = EMapObject_Type::INSTANCEMODEL;
}

CInstanceModel::CInstanceModel(const CInstanceModel& rhs)
	: CMapObject(rhs), m_iSelectInstanceID{rhs.m_iSelectInstanceID }
{
	m_eMapObjectType = EMapObject_Type::INSTANCEMODEL;
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
	
	if (m_tData.tUsingModelInfo.vecOverrideMaterial.empty())
		m_iUseOverrideMaterials = false;
	else
	{
		for (auto& OverrideMtl : m_tData.tUsingModelInfo.vecOverrideMaterial)
		{
			if (!OverrideMtl.isNull)
				m_iUseOverrideMaterials = true;
		}
		m_vecOverrideMaterials.resize(m_tData.tUsingModelInfo.vecOverrideMaterial.size());
	}

	if (FAILED(CInstanceModel::Ready_SRTData()))
		return E_FAIL;


	if (FAILED(CInstanceModel::Ready_Component()))
		return E_FAIL;


	if (FAILED(CMapObject::Ready_OverrideMtl(m_tData.tUsingModelInfo)))
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

HRESULT CInstanceModel::Ready_SRTData()
{

	for (auto& SRTData : m_tData.vecSRT)
	{
		SRTData.vScale = SRTData.vScale_Isolated;
		Matrix WorldMatrix = SRTData.Get_World();
		m_tData.vecMatirx.push_back(WorldMatrix);
	}

	m_vecOriginSRTs.resize(m_tData.vecSRT.size());
	memcpy(m_vecOriginSRTs.data(), m_tData.vecSRT.data(), sizeof(SRT_DATA) * m_tData.vecSRT.size());

	return S_OK;
}

HRESULT CInstanceModel::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

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

	CShader*	   pShader		= CGameObject::Get_Component<CShader>();
	CModel*		   pModel		= CGameObject::Get_Component<CModel>();
	CTransform*    pTransform	= CGameObject::Get_Component<CTransform>();
	CInstanceMesh* pInstMesh	= CGameObject::Get_Component<CInstanceMesh>();

	if (pShader == nullptr || pModel == nullptr || pTransform == nullptr ) return E_FAIL;

	_uint iMeshCount		= pModel->Get_MeshCount();
	_uint iInstacnceCount	= pInstMesh->Get_InstanceCount();

	/* 1번 슬롯에 Instance 미리 바인딩 */

	pInstMesh->Bind_Instance(1);
	pShader->Set_Pass(ENUM_TO_UINT(m_eMapObjectState));

	if (!m_iUseOverrideMaterials)
	{
		for (UINT32 i = 0; i < iMeshCount; ++i)
		{
			pModel->Bind_Material(pShader, i);
			pModel->Bind_MaterialInstance(pShader, i);
			pShader->Apply();
			pModel->Render_Instance(i, iInstacnceCount);
		}
	}
	else
	{
		_uint iConnectedIndex{ 0 };
		for (UINT32 i = 0; i < iMeshCount; ++i)
		{
			iConnectedIndex = pModel->Get_Mesh(i)->Get_MaterialIndex();
			if (!m_vecOverrideMaterials[iConnectedIndex])
			{
				pModel->Bind_Material(pShader, i);
				pModel->Bind_MaterialInstance(pShader, i);
			}
			else
			{
				m_vecOverrideMaterials[iConnectedIndex]->Bind_ShaderResource(pShader);
				pModel->Bind_MaterialInstance(pShader, i);
			}

			pShader->Apply();
			pModel->Render_Instance(i  , iInstacnceCount);
		}
	}


	return S_OK;
}

void CInstanceModel::Draw_ImGui()
{
	if (ImGui::TreeNode("Instant Model"))
	{

		ImGui::TreePop();
	}
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


Matrix CInstanceModel::Get_WorldMatrix()
{
	return m_tData.vecMatirx[m_iSelectInstanceID];
}

void CInstanceModel::Set_WorldMatrix(const Matrix& WorldMatrix)
{
	m_tData.vecMatirx[m_iSelectInstanceID] = WorldMatrix;
	SRT_DATA& tSRT_Data = m_tData.vecSRT[m_iSelectInstanceID];
	m_tData.vecMatirx[m_iSelectInstanceID].Decompose(tSRT_Data.vScale , tSRT_Data.vQuat , tSRT_Data.vPosition);
	Get_Component<CInstanceMesh>()->Update_Matrix(WorldMatrix,m_iSelectInstanceID);
}

void CInstanceModel::Reset_OriginTransform(_uint iIndex)
{
	m_tData.vecSRT[iIndex] = m_vecOriginSRTs[iIndex];
	m_tData.vecMatirx[iIndex] = m_tData.vecSRT[iIndex].Get_World();
	Update_InstanceWorldMatrix(iIndex);
	return;
}

/* 내 현재 Index 값에 맞게 Instance버퍼 업데이트함수 */
void CInstanceModel::Update_InstanceWorldMatrix(_uint iIndex)
{
	Get_Component<CInstanceMesh>()->Update_Matrix( m_tData.vecMatirx[iIndex] , iIndex );
}

void CInstanceModel::Update_InstanceWorldMatirx(const SimpleMath::Matrix& WorldMatrix, _uint iIndex)
{
	m_tData.vecMatirx[iIndex] = WorldMatrix;
	m_tData.vecMatirx[iIndex].Decompose(m_tData.vecSRT[iIndex].vScale , m_tData.vecSRT[iIndex].vQuat , m_tData.vecSRT[iIndex].vPosition);
	Get_Component<CInstanceMesh>()->Update_Matrix(WorldMatrix , iIndex);
}

bool  CInstanceModel::IntsersectWithPlane(OUT Vec3& vOut)
{
	CModel* pModel = Get_Component<CModel>();
	if (pModel == nullptr)  return false;

	_uint iMeshCount = pModel->Get_MeshCount();
	for (_uint i = 0; i < iMeshCount; ++i)
	{
		if (pModel->Get_Mesh(i)->IntsersectWithPlane(vOut))
		{
			return true;
		}
	}
	return false;
}


_bool CInstanceModel::Picking(OUT Vec3& vOut)
{
	Matrix InvWorldMatrix{};
	/* Matrix 를 업데이트 시켜준다 */
	_uint iIndex = 0;
	for (auto& World : m_tData.vecMatirx)
	{
		InvWorldMatrix = World.Invert();

		m_pGameInstance->TransformRayToLocalSpace(InvWorldMatrix);

		if (IntsersectWithPlane(vOut))
		{
			vOut = Vec3::Transform(vOut , World);
			m_iSelectInstanceID = iIndex;
			return true;
		}
		iIndex++;
	}

	m_iSelectInstanceID = 0;

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
	{
		// PhysX 충돌체 초기화용 괴도 소재혁
		for (auto& srtData : m_tData.vecSRT)
		{
			srtData.vScale_Isolated = srtData.vScale;
			srtData.vScale = Vec3(1.f, 1.f, 1.f);
		}
	}

	tSave_InstanceModleData.vecSRTData.resize(m_tData.vecSRT.size());
	memcpy(tSave_InstanceModleData.vecSRTData.data(), m_tData.vecSRT.data(), sizeof(SRT_DATA) * m_tData.vecSRT.size());

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
