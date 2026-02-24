#include "pch.h"
#include "Bush.h"
#include "DataStruct_Map.h"



CBush::CBush(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlants(pDevice ,pDeviceContext)
{
}

CBush::CBush(const CBush& rhs)
	: CPlants(rhs)
{
}


HRESULT CBush::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBush::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBush::Ready_Component(BUSH_DESC* pDesc)
{

	return S_OK;
}

HRESULT CBush::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CBush::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CBush::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);
}

void CBush::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);

}


void CBush::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CBush::Render()
{
	if (m_eMapObjectDrawType == EMapObject_DrawType::Instance)
		CMapObject::Render_Default(DTO::EClientMakePath::);


	return S_OK;
}

CBush* CBush::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBush* pBush = new CBush(pDevice, pDeviceContext);

	if (FAILED(pBush->Initialize_Prototype()))
	{
		Safe_Release(pBush);
		MSG_BOX("Bush is failed to Create");
		return nullptr;
	}
	return pBush;
}

CGameObject* CBush::Clone(void* pArg)
{
	CBush* pBush = new CBush(*this);

	if (FAILED(pBush->Initialize(pArg)))
	{
		Safe_Release(pBush);
		MSG_BOX(" Bush is failed to Clone ");
		return nullptr;
	}

	return pBush;
}

void CBush::Free()
{
	Super::Free();
}
