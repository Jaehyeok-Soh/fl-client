#include "pch.h"
#include "Level_Test.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Test::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;



	return S_OK;
}

void CLevel_Test::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Test::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CLevel_Test::Free()
{
	Super::Free();
}