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
    wstring      wstrChangeLevelName{ L"" };

    static ELevelType s_eForceRevertTab = ELevelType::END;

	// ? [추가 핵심] 현재 프레임이 "탭을 강제로 되돌리는 중"인지 기억해 둠
	bool bIsRevertingThisFrame = (s_eForceRevertTab != ELevelType::END);

    if (ImGui::BeginTabBar(m_strLabel.c_str()))
    {
        auto RenderTab = [&](const char* szName, ELevelType eType, const wstring& wName)
            {
                ImGuiTabItemFlags flag = (s_eForceRevertTab == eType) ? ImGuiTabItemFlags_SetSelected : 0;

				if (ImGui::BeginTabItem(szName, nullptr, flag))
				{
					wstrChangeLevelName = wName;
					eNowSelected = eType;
					m_isNo = false;
					ImGui::EndTabItem();
				}
			};

		RenderTab("Logo##TabBar", ELevelType::LOGO, L"LOGO");
		RenderTab("Map##TabBar", ELevelType::MAP, L"MAP");
		RenderTab("Animation##TabBar", ELevelType::ANIMATION, L"ANIMATION");
		RenderTab("Effect##TabBar", ELevelType::EFFECT, L"EFFECT");
		RenderTab("Camera##TabBar", ELevelType::CAMERA, L"CAMERA");
		RenderTab("UI##TabBar", ELevelType::UI, L"UI");
		RenderTab("AttackPreset##TabBar", ELevelType::ATTACK_PRESET, L"ATTACK_PRESET");
		RenderTab("AssetConvert##TabBar", ELevelType::ASSET_CONVERT, L"ASSET_CONVERT");

		ImGui::EndTabBar();
	}

	// 탭 플래그를 먹였으니 다음 프레임을 위해 다시 END로 초기화
	s_eForceRevertTab = ELevelType::END;

	// ? [추가 핵심] 방금 '아니요'를 눌러서 탭이 돌아가는 중이라면, 메세지박스 검사 자체를 1프레임 건너뜀!
	if (bIsRevertingThisFrame)
		return S_OK;

	if ((eNowSelected != ELevelType::END) && (eNowSelected != m_eLastSelectedTab))
	{
		wstring Name{ L"[ " + wstrChangeLevelName + L" ] 로 변환됩니다 " };

		if (IDOK == MessageBox(NULL, Name.c_str(), L"경고: 데이터 말소 X", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND))
		{
			if (FAILED(Change_Scene(eNowSelected))) return E_FAIL;
			m_eLastSelectedTab = eNowSelected;
		}
		else
		{
			// 취소 누름 -> 다음 프레임에 원래 탭으로 돌아가라고 플래그 장전!
			s_eForceRevertTab = m_eLastSelectedTab;
		}
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
