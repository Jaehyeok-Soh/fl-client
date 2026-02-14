#include "pch.h"
#include "Panel_Model.h"



CPanel_Model::CPanel_Model(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), 
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_Release(m_pGameInstance);
}

HRESULT CPanel_Model::Render(CToolObject* pGo)
{
	return S_OK;
}

void CPanel_Model::Update(const _float fTimeDelta)
{
}

HRESULT CPanel_Model::Render_LoadFile()
{
	return S_OK;
}

HRESULT CPanel_Model::Render_ModelInfo()
{
	return S_OK;
}

HRESULT CPanel_Model::Render_Animations()
{
	return S_OK;
}

HRESULT CPanel_Model::Render_Bones()
{
	return S_OK;
}

HRESULT CPanel_Model::Render_PreTransform()
{
	return S_OK;
}

HRESULT CPanel_Model::Make_Model()
{
	// 선택된 모델로 게임 오브젝트를 만든다
	return S_OK;
}

void CPanel_Model::Free()
{
	Super::Free();

	Safe_Release(m_pGameInstance);
}
