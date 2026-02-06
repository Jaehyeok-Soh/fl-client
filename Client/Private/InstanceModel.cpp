#include "pch.h"
#include "InstanceModel.h"
#include "Model.h"
#include "Shader.h"
#include "InstanceMesh.h"
#include "GameInstance.h"

#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

#include "Mesh.h"

CInstanceModel::CInstanceModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext),m_vecMatrix{}
{
	m_eMapObjectType = EMapObject_Type::INSTANCEMODEL;
}

CInstanceModel::CInstanceModel(const CInstanceModel& rhs)
	: CMapObject(rhs), m_vecMatrix{rhs.m_vecMatrix}
{
}

HRESULT CInstanceModel::Initialize_Prototype()
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
	CInstanceModel::INSTANCEMODEL_DESC copiedDesc = *pDesc;

	Set_Name(pDesc->tData.tUsingModelInfo.wstrName);

	for (auto& SRTData : pDesc->tData.vecSRTData)
	{
		SRTData.vScale = SRTData.vScale_Isolated;
		m_vecMatrix.push_back(SRTData.Get_World());
	}

	

	if (FAILED(CInstanceModel::Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(CMapObject::Ready_OverrideMtl(pDesc->tData.tUsingModelInfo)))
		return E_FAIL;
	
	if (FAILED(CInstanceModel::Ready_PhysicsComponent(&copiedDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Ready_Component(INSTANCEMODEL_DESC* pDesc)
{
	CModel::MODEL_COPY_DESC tDesc{};

	if (FAILED(Add_Component<CShader>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Shader_VtxInstanceMesh", nullptr)))
		return E_FAIL;

	CModel::MODEL_ORIGIN_DESC tModelDesc{};
	tModelDesc.eType = EModelType::STATIC;
	tModelDesc.wstrModelFolderName = pDesc->tData.tUsingModelInfo.wstrPath;
	tModelDesc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
	CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, &tModelDesc);
	if (pModel)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tData.tUsingModelInfo.wstrName, pModel)))
			Safe_Release(pModel);
		else
			m_pGameInstance->RegisterPhysicsMesh(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tData.tUsingModelInfo.wstrName);
	}
	CModel::MODEL_COPY_DESC tModelCopyDesc{};
	CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tData.tUsingModelInfo.wstrName, &tModelCopyDesc);


	CInstanceMesh::INSTANCEMESH_DESC tInstanceMeshDesc{};
	tInstanceMeshDesc.VB_Usage = D3D11_USAGE_DYNAMIC;
	tInstanceMeshDesc.IB_Usage = D3D11_USAGE_DYNAMIC;
	tInstanceMeshDesc.vecInstanceMatrixPointer = &m_vecMatrix;
	if (FAILED(CGameObject::Add_Component<CInstanceMesh>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", &tInstanceMeshDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Ready_PhysicsComponent(INSTANCEMODEL_DESC* pDesc)
{
	HRESULT result{};

	if (FAILED(Ready_PhysicsCollider(pDesc)))
		result = E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody(pDesc)))
		result = E_FAIL;

	return result;
}

HRESULT CInstanceModel::Ready_PhysicsCollider(INSTANCEMODEL_DESC* pDesc)
{
	PHYSICSCOLLIDER_DESC pcDesc{};
	pcDesc.wstrModelPrototypeTag = L"Prototype_Component_Model_" + pDesc->tData.tUsingModelInfo.wstrName;
	pcDesc.bIsConvex = false;

	CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
	if (pCollider)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + pDesc->tData.tUsingModelInfo.wstrName, pCollider)))
			Safe_Release(pCollider);
	}

	if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + pDesc->tData.tUsingModelInfo.wstrName, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Ready_PhysicsRigidBody(INSTANCEMODEL_DESC* pDesc)
{
	PHYSICSRIGIDBODY_DESC desc{};
	desc.eType = EPhysicsActorType::STATIC;
	desc.detection = EPhysicsCollisionDetection::DISCRETE;
	desc.fDensity = 10.f;
	desc.bUseGravity = false;
	desc.bIsKinematic = false;
	desc.fLinearDamping = 0.f;
	desc.fAngularDamping = 0.f;

	for (size_t i = 0; i < pDesc->tData.vecSRTData.size(); i++)
	{
		auto& srtData = pDesc->tData.vecSRTData[i];
		auto instWorld = srtData.Get_World();
		desc.pOwnerMatrices.push_back(instWorld);
		desc.vScale_Isolated.push_back(pDesc->tData.vecSRTData[i].vScale_Isolated);
	}

	if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceModel::Awake(const _uint iCurrentLevelID)
{
	/* Instance Model 위치 값이 들어갈 예정 */

	Get_Component<CPhysicsRigidBody>()->Awake();

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

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND , this);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif // _DEBUG
}

HRESULT CInstanceModel::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	CShader* pShader = CGameObject::Get_Component<CShader>();
	CModel* pModel = CGameObject::Get_Component<CModel>();
	CTransform* pTransform = CGameObject::Get_Component<CTransform>();
	CInstanceMesh* pInstMesh = CGameObject::Get_Component<CInstanceMesh>();

	if (pShader == nullptr || pModel == nullptr || pTransform == nullptr) return E_FAIL;

	_uint iMeshCount = pModel->Get_MeshCount();
	_uint iInstacnceCount = pInstMesh->Get_InstanceCount();

	/* 1번 슬롯에 Instance 미리 바인딩 */
	pInstMesh->Bind_Instance(1);

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
			pModel->Render_Instance(i, iInstacnceCount);
		}
	}

	return S_OK;
}


CInstanceModel* CInstanceModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceModel* pInstanceModel = new CInstanceModel(pDevice, pContext);

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
