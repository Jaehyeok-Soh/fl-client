#include "pch.h"
#include "MapObject.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext), m_eMapObjectType{EMapObject_Type::END}
{
}

CMapObject::CMapObject(const CMapObject& rhs)
	: CGameObject(rhs), m_eMapObjectType(rhs.m_eMapObjectType)
{
}


HRESULT	CMapObject::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CMapObject::Initialize(void* pArg)
{
	if(FAILED(Super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

HRESULT	CMapObject::Ready_Component()
{
	return S_OK;
}

HRESULT	CMapObject::Add_MapToolComponent(CMapObject::COMPONENT eType)
{
	return S_OK;
}

HRESULT	CMapObject::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void	CMapObject::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}
void	CMapObject::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}
void	CMapObject::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);



}
void	CMapObject::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT	CMapObject::Render()
{
	if (Super::Render())
		return E_FAIL;

	return S_OK;
}


void CMapObject::Free()
{

	for (auto& Com : m_arrayMapToolComponent)
	{
		Safe_Release(Com);
	}

	Super::Free();

}
