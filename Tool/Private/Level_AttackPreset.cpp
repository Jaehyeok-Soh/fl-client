#include "pch.h"
#include "Level_AttackPreset.h"
#include "Panel_PresetList.h"
#include "Panel_PresetMain.h"
#include "AttackPreset_DataManager.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CLevel_AttackPreset::CLevel_AttackPreset(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	m_arrayImGuiPanel.fill(nullptr);
}

HRESULT CLevel_AttackPreset::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AttackPreset::Ready_GUI()
{
	if (!(m_arrayImGuiPanel[ENUM_TO_UINT(Elements::PresetList)] = CPanel_PresetList::Create(this, m_pDevice, m_pDeviceContext)))
		return E_FAIL;
	if (!(m_arrayImGuiPanel[ENUM_TO_UINT(Elements::PresetMain)] = CPanel_PresetMain::Create(this, m_pDevice, m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AttackPreset::Awake(const _uint iLevelID)
{
	if(FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_GUI()))
		return E_FAIL;

	return S_OK;
}

void CLevel_AttackPreset::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CLevel_AttackPreset::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	for (auto& Panel : m_arrayImGuiPanel)
		if (Panel)
			Panel->Render(nullptr);

	//////////////////////////
	m_pImGuiManager->Render_End();

	return S_OK;
}

void CLevel_AttackPreset::Clear()
{
	CAttackPreset_DataManager::GetInstance()->Clear();
	Safe_Release(m_pImGuiManager);

	for (auto& Panel : m_arrayImGuiPanel)
		Safe_Release(Panel);

	m_arrayImGuiPanel.fill(nullptr);
}

CLevel_AttackPreset* CLevel_AttackPreset::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_AttackPreset* pInstance = new CLevel_AttackPreset(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_AttackPreset::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_AttackPreset::Free()
{
	Clear();
	Super::Free();
}
