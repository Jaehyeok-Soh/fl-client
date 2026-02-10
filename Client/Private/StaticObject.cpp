#include "pch.h"
#include "StaticObject.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CStaticObject::CStaticObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice, pDeviceContext)
{
	m_eMapObjectType = EMapObject_Type::StaticObject;
}

CStaticObject::CStaticObject(const CStaticObject& rhs)
	: CMapObject(rhs)
{

}

HRESULT CStaticObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticObject::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	STATICOBJECT_DESC* pDesc = static_cast<STATICOBJECT_DESC*>(pArg);


	if (FAILED(CStaticObject::Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PhysicsComponent(pDesc)))
		return E_FAIL;

	//Get_Component<CTransform>()->Set_Scale(pDesc->vScale_Isolated); // TEST: 소재혁 임시 추가

	return S_OK;
}

HRESULT CStaticObject::Ready_Component(STATICOBJECT_DESC* pDesc)
{



	return S_OK;
}

HRESULT CStaticObject::Ready_PhysicsComponent(STATICOBJECT_DESC* pDesc)
{
	HRESULT result{};

	if (FAILED(Ready_PhysicsCollider(pDesc)))
		result = E_FAIL;

	if (FAILED(Ready_PhysicsRigidBody(pDesc)))
		result = E_FAIL;

	return result;
}

HRESULT CStaticObject::Ready_PhysicsCollider(STATICOBJECT_DESC* pDesc)
{
	PHYSICSCOLLIDER_DESC pcDesc{};
	wstring wstrModelName = path(pDesc->wstrModelPath).filename().stem().wstring();

	pcDesc.wstrModelPrototypeTag = L"Prototype_Component_Model_" + wstrModelName;
	pcDesc.bIsConvex = false;

	CPhysicsCollider* pCollider = CPhysicsCollider::Create(m_pDevice, m_pDeviceContext, &pcDesc);
	if (pCollider)
	{
		if (FAILED(m_pGameInstance->Add_Prototype(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, pCollider)))
			Safe_Release(pCollider);
	}
	
	if (FAILED(Add_Component<CPhysicsCollider>(pDesc->iLevelIndex, L"Prototype_Component_Physics_Collider_" + wstrModelName, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStaticObject::Ready_PhysicsRigidBody(STATICOBJECT_DESC* pDesc)
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

HRESULT CStaticObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();

	return S_OK;
}

void CStaticObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}


void CStaticObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CStaticObject::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

}

void CStaticObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::NONEBLEND, this);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif // _DEBUG
}

HRESULT CStaticObject::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;


	return S_OK;
}



CStaticObject* CStaticObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStaticObject* pStaticObject = new CStaticObject(pDevice, pDeviceContext);

	if (FAILED(pStaticObject->Initialize_Prototype()))
	{
		Safe_Release(pStaticObject);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}
	return pStaticObject;
}

CGameObject* CStaticObject::Clone(void* pArg)
{
	CStaticObject* pStaticObject = new CStaticObject(*this);

	if (FAILED(pStaticObject->Initialize(pArg)))
	{
		Safe_Release(pStaticObject);
		MSG_BOX(" Static Model Is Failed To Create ");
		return nullptr;
	}

	return pStaticObject;
}


void CStaticObject::Free()
{
	Super::Free();
}
