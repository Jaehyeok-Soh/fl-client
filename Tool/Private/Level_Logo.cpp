#include "pch.h"
#include "Level_Logo.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
	, m_vecCCS_EventManifest{}
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Logo::Initialize()
{
	m_pGameInstance->Request_CursorMode(ECursorMode::Tool);

	if (FAILED(Ready_CCS_EventManifest()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_CCS_EventManifest()
{
	if (FAILED(Load_CCS_EventManifest(&m_vecCCS_EventManifest)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;




	return S_OK;
}

void CLevel_Logo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();


	if (FAILED(Render_CCS_Setting()))
		return E_FAIL;

	//////////////////////////
	// Element Render

	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();
	return S_OK;
}
HRESULT CLevel_Logo::Render_CCS_Setting()
{
	ImGui::Begin(" Camera Cinematic Sequence Event Manifest Setting Window ");

	// 선택 상태 및 입력 버퍼를 유지하기 위한 static 변수들
	static int iSelectedSubscriber = -1;
	static int iSelectedAction = -1;
	static char szNewSubscriber[128] = "";
	static char szNewAction[128] = "";

	// =========================================================
	// 1. Subscriber (수신자) 리스트 영역
	// =========================================================
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "[ Subscriber List ]");
	ImGui::BeginChild("SubscriberList", ImVec2(ImGui::GetWindowWidth() * 0.45f, 200), true);

	for (int i = 0; i < m_vecCCS_EventManifest.size(); ++i)
	{
		bool bSelected = (iSelectedSubscriber == i);
		if (ImGui::Selectable(m_vecCCS_EventManifest[i].strSubscriberName.c_str(), bSelected))
		{
			iSelectedSubscriber = i;
			iSelectedAction = -1; // 수신자가 바뀌면 액션 선택 초기화
		}
	}
	ImGui::EndChild();

	// Subscriber 추가 및 삭제 UI
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
	ImGui::InputText("##NewSub", szNewSubscriber, sizeof(szNewSubscriber));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("Add Sub"))
	{
		if (strlen(szNewSubscriber) > 0)
		{
			// [중복 검사] 이미 같은 이름의 Subscriber가 있는지 확인
			bool bIsDuplicate = false;
			for (const auto& manifest : m_vecCCS_EventManifest)
			{
				if (manifest.strSubscriberName == szNewSubscriber)
				{
					bIsDuplicate = true;
					break;
				}
			}

			// 중복이 아닐 때만 추가
			if (!bIsDuplicate)
			{
				CCS_EVENT_MANIFEST newManifest;
				newManifest.strSubscriberName = szNewSubscriber;
				m_vecCCS_EventManifest.push_back(newManifest);
				memset(szNewSubscriber, 0, sizeof(szNewSubscriber)); // 입력창 비우기
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Sub") && iSelectedSubscriber != -1)
	{
		m_vecCCS_EventManifest.erase(m_vecCCS_EventManifest.begin() + iSelectedSubscriber);
		iSelectedSubscriber = -1;
		iSelectedAction = -1;
	}

	ImGui::Separator();

	// =========================================================
	// 2. Action (명령) 리스트 영역 (선택된 Subscriber에 종속됨)
	// =========================================================
	ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "[ Action List ]");
	if (iSelectedSubscriber != -1 && iSelectedSubscriber < m_vecCCS_EventManifest.size())
	{
		auto& vecActions = m_vecCCS_EventManifest[iSelectedSubscriber].vecActionNames;

		ImGui::BeginChild("ActionList", ImVec2(ImGui::GetWindowWidth() * 0.45f, 200), true);
		for (int i = 0; i < vecActions.size(); ++i)
		{
			bool bSelected = (iSelectedAction == i);
			if (ImGui::Selectable(vecActions[i].c_str(), bSelected))
			{
				iSelectedAction = i;
			}
		}
		ImGui::EndChild();

		// Action 추가 및 삭제 UI
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		ImGui::InputText("##NewAction", szNewAction, sizeof(szNewAction));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Add Action"))
		{
			if (strlen(szNewAction) > 0)
			{
				// [중복 검사] 현재 Subscriber에 같은 이름의 Action이 있는지 확인
				bool bIsDuplicate = false;
				for (const auto& actionName : vecActions)
				{
					if (actionName == szNewAction)
					{
						bIsDuplicate = true;
						break;
					}
				}

				// 중복이 아닐 때만 추가
				if (!bIsDuplicate)
				{
					vecActions.push_back(szNewAction);
					memset(szNewAction, 0, sizeof(szNewAction)); // 입력창 비우기
				}
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Action") && iSelectedAction != -1)
		{
			vecActions.erase(vecActions.begin() + iSelectedAction);
			iSelectedAction = -1;
		}
	}
	else
	{
		// 수신자를 선택하지 않았을 때의 안내 문구
		ImGui::BeginChild("ActionListEmpty", ImVec2(ImGui::GetWindowWidth() * 0.45f, 200), true);
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Please select a Subscriber first.");
		ImGui::EndChild();
	}

	ImGui::Separator();

	// =========================================================
	// 3. JSON 데이터 저장 (Save to File)
	// =========================================================
	ImGui::Spacing();
	if (ImGui::Button("Save Manifest to JSON", ImVec2(-1, 40))) // 가로를 꽉 채우는 큰 버튼
	{
		Save_CCS_EventManifest(&m_vecCCS_EventManifest);
	}

	ImGui::End();

	return S_OK;
}
CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Logo::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	Safe_Release(m_pImGuiManager);
	Super::Free();
}
