#include "pch.h"
#include "ImGui_Dockspace_TabBar.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CImGui_Dockspace_TabBar::CImGui_Dockspace_TabBar(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel)
	: Super(pLabel, pDevice, pDeviceContext)
	, m_eLastSelectedTab(eStartLevel)
{
}

HRESULT CImGui_Dockspace_TabBar::Render(CToolObject* pGo)
{
	ELevelType eNowSelected = ELevelType::END;

	if (ImGui::BeginTabBar(m_strLabel.c_str()))
	{
		if (ImGui::BeginTabItem("Map##TabBar"))
		{
			eNowSelected = ELevelType::MAP;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Animation##TabBar"))
		{
			eNowSelected = ELevelType::ANIMATION;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Effect##TabBar"))
		{
			eNowSelected = ELevelType::EFFECT;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera##TabBar"))
		{
			eNowSelected = ELevelType::CAMERA;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("UI##TabBar"))
		{
			eNowSelected = ELevelType::UI;
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("AssetConvert##TabBar"))
		{
			eNowSelected = ELevelType::ASSET_CONVERT;
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if ((eNowSelected != ELevelType::END) && (eNowSelected != m_eLastSelectedTab))
	{
		if (FAILED(Change_Scene(eNowSelected)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CImGui_Dockspace_TabBar::Change_Scene(ELevelType eLevel)
{
	CGameInstance::GetInstance()->Request_ChangeLevel(
		ENUM_TO_UINT(ELevelType::LOADING),
		CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevel));

	m_eLastSelectedTab = eLevel;
	return S_OK;
}

CImGui_Dockspace_TabBar* CImGui_Dockspace_TabBar::Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eStartLevel)
{
	return new CImGui_Dockspace_TabBar(pLabel, pDevice, pDeviceContext, eStartLevel);
}

void CImGui_Dockspace_TabBar::Free()
{
	Super::Free();
}
