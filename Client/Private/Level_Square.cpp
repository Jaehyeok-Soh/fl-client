#include "pch.h"
#include "Level_Square.h"

CLevel_Square::CLevel_Square(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Square::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;



	return S_OK;
}

void CLevel_Square::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Square::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CLevel_Square::Free()
{
	Super::Free();
}