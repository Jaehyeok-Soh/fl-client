#include "pch.h"
#include "StaticObject.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"
#include "LandScape.h"

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

	return S_OK;
}

HRESULT CStaticObject::Ready_Component(STATICOBJECT_DESC* pDesc)
{
	return S_OK;
}

HRESULT CStaticObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

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
