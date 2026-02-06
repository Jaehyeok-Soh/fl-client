#include "pch.h"
#include "StaticModel.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"

#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

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


	if (FAILED(CMapObject::Ready_OverrideMtl(pDesc->tUsingModelInfo)))
		return E_FAIL;


	Get_Component<CTransform>()->Set_Scale(pDesc->vScale_Isolated); // TEST: 소재혁 임시 추가


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
			else
				m_pGameInstance->RegisterPhysicsMesh(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tUsingModelInfo.wstrName);
		}
		CModel::MODEL_COPY_DESC tModelCopyDesc{};
		CGameObject::Add_Component<CModel>(tModelDesc.iPrototypeLevelIndex, L"Prototype_Component_Model_" + pDesc->tUsingModelInfo.wstrName, &tModelCopyDesc);
	}

	{
		if (FAILED(Ready_PhysicsComponent(pDesc)))
			MSG_BOX("Failed to ready physics component : CStaticModel");
	}

	return S_OK;
}

HRESULT CStaticModel::Ready_PhysicsComponent(STATICMODEL_DESC* pDesc)
{
	HRESULT result{};

	if (FAILED(Ready_PhysicsCollider(pDesc)))
		result = E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody(pDesc)))
		result = E_FAIL;

	return result;
}

HRESULT CStaticModel::Ready_PhysicsCollider(STATICMODEL_DESC* pDesc)
{
	PHYSICSCOLLIDER_DESC pcDesc{};
	pcDesc.wstrModelPrototypeTag = L"Prototype_Component_Model_" + pDesc->tUsingModelInfo.wstrName;
	pcDesc.bIsConvex = false;

	CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
	if (pCollider)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + pDesc->tUsingModelInfo.wstrName, pCollider)))
			Safe_Release(pCollider);
	}
	
	if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + pDesc->tUsingModelInfo.wstrName, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Ready_PhysicsRigidBody(STATICMODEL_DESC* pDesc)
{
	PHYSICSRIGIDBODY_DESC desc{};
	desc.eType = EPhysicsActorType::STATIC;
	desc.detection = EPhysicsCollisionDetection::DISCRETE;
	desc.fDensity = 10.f;
	desc.bUseGravity = false;
	desc.bIsKinematic = false;
	desc.fLinearDamping = 0.f;
	desc.fAngularDamping = 0.f;
	desc.pOwnerMatrices.push_back(Get_Component<CTransform>()->Get_WorldMatrix());
	desc.vScale_Isolated.push_back(pDesc->vScale_Isolated);

	if (FAILED(Add_Component<CPhysicsRigidBody>(0, L"Prototype_Component_Physics_RigidBody", &desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticModel::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

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

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif // _DEBUG
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


	//if (!m_iUseOverrideMaterials)
	//{

	//}
	//else
	//{
	//	_uint iConnectedIndex{ 0 };
	//	for (UINT32 i = 0; i < iMeshCount; ++i)
	//	{
	//		iConnectedIndex = pModel->Get_Mesh(i)->Get_MaterialIndex();
	//		if (!m_vecOverrideMaterials[iConnectedIndex])
	//		{
	//			pModel->Bind_Material(pShader, i);
	//			pModel->Bind_MaterialInstance(pShader, i);
	//		}
	//		else
	//		{
	//			m_vecOverrideMaterials[iConnectedIndex]->Bind_ShaderResource(pShader);
	//			pModel->Bind_MaterialInstance(pShader, i);
	//		}

	//		pShader->Apply();
	//		pModel->Render(i);
	//	}
	//}

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
