#include "pch.h"
#include "Level_Tutorial_Village.h"

CLevel_Tutorial_Village::CLevel_Tutorial_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Tutorial_Village::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tutorial_Village::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tutorial_Village::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Tutorial_Village::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}



void CLevel_Tutorial_Village::Free()
{
	Super::Free();
}

