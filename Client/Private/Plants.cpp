#include "pch.h"
#include "Plants.h"
#include "Bush.h"

CPlants::CPlants(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMapObject(pDevice , pDeviceContext)
{
}

CPlants::CPlants(const CPlants& rhs)
	: CMapObject(rhs)
{
}

HRESULT CPlants::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlants::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}


HRESULT CPlants::Ready_Component(PLANTS_DESC* pDesc)
{


	return S_OK;
}

HRESULT CPlants::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CPlants::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CPlants::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CPlants::Update_Late(const _float fTimeelta)
{
	Super::Update_Late(fTimeelta);
}

void CPlants::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}

HRESULT CPlants::Render()
{
	return S_OK;
}

void CPlants::Free()
{
	Super::Free();

	return;
}
