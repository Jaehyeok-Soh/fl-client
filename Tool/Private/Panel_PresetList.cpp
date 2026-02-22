#include "pch.h"
#include "Panel_PresetList.h"
#include "GameInstance.h"

CPanel_PresetList::CPanel_PresetList(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super("PresetList##AttackPreset", pOwner, pDevice, pDeviceContext)
{
}

HRESULT CPanel_PresetList::Initialize()
{
	return S_OK;
}

void CPanel_PresetList::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_PresetList::Render(CToolObject* pGo)
{
	return S_OK;
}

CPanel_PresetList* CPanel_PresetList::Create(CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_PresetList* pInstance = new CPanel_PresetList(pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CPanel_PresetList::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPanel_PresetList::Free()
{
	Super::Free();
}
