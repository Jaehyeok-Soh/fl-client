#include "pch.h"
#include "Panel_PresetMain.h"
#include "GameInstance.h"

CPanel_PresetMain::CPanel_PresetMain(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super("PresetMain##AttackPreset", pOwner, pDevice, pDeviceContext)
{
}

HRESULT CPanel_PresetMain::Initialize()
{
	return S_OK;
}

void CPanel_PresetMain::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_PresetMain::Render(CToolObject* pGo)
{
	return S_OK;
}

CPanel_PresetMain* CPanel_PresetMain::Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_PresetMain* pInstance = new CPanel_PresetMain(pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CPanel_PresetMain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPanel_PresetMain::Free()
{
	Super::Free();
}