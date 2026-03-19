#include "pch.h"
#include "Tool_Defines.h"
#include "Tool_Structs.h"
#include <fstream>

unsigned int Tool::g_iWinSizeX = 1850;
unsigned int Tool::g_iWinSizeY = 950;
float Tool::g_fAspectio = 1.f;
HRESULT Tool::Render_ImGui_CSS_EventManifest(std::vector<struct CCS_EVENT_MANIFEST>* _vecCSSEventManifests, bool* pOpen)
{
	if (!ImGui::Begin(" Camera Cinematic Sequence Event Manifest Setting Window ", pOpen))
	{
		ImGui::End();
		return S_OK;
	}

	vector<CCS_EVENT_MANIFEST>& vecCSSEventManifests = *_vecCSSEventManifests;

	static int iSelectedSubscriber = -1;
	static int iSelectedAction = -1;

	static char szNewSubscriber[128] = "";
	static char szNewAction[128] = "";
	static char szNewActionExplain[1024] = "";

	// =========================================================
	// 1. Subscriber (수신자) 리스트 영역
	// =========================================================
	ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "[ Subscriber List ]");
	ImGui::BeginChild("SubscriberList", ImVec2(ImGui::GetWindowWidth() * 0.45f, 200), true);

	for (int i = 0; i < vecCSSEventManifests.size(); ++i)
	{
		bool bSelected = (iSelectedSubscriber == i);
		if (ImGui::Selectable(vecCSSEventManifests[i].strSubscriberName.c_str(), bSelected))
		{
			iSelectedSubscriber = i;
			iSelectedAction = -1;

			strcpy_s(szNewSubscriber, sizeof(szNewSubscriber), vecCSSEventManifests[i].strSubscriberName.c_str());

			memset(szNewAction, 0, sizeof(szNewAction));
			memset(szNewActionExplain, 0, sizeof(szNewActionExplain));
		}
	}
	ImGui::EndChild();

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
	ImGui::InputText("##NewSub", szNewSubscriber, sizeof(szNewSubscriber));
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Add Sub"))
	{
		if (strlen(szNewSubscriber) > 0)
		{
			bool bIsDuplicate = false;
			for (const auto& manifest : vecCSSEventManifests) {
				if (manifest.strSubscriberName == szNewSubscriber) { bIsDuplicate = true; break; }
			}

			if (!bIsDuplicate)
			{
				CCS_EVENT_MANIFEST newManifest;
				newManifest.strSubscriberName = szNewSubscriber;
				vecCSSEventManifests.push_back(newManifest);
				memset(szNewSubscriber, 0, sizeof(szNewSubscriber));
			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Modify Sub") && iSelectedSubscriber != -1)
	{
		if (strlen(szNewSubscriber) > 0) vecCSSEventManifests[iSelectedSubscriber].strSubscriberName = szNewSubscriber;
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Sub") && iSelectedSubscriber != -1)
	{
		vecCSSEventManifests.erase(vecCSSEventManifests.begin() + iSelectedSubscriber);
		iSelectedSubscriber = -1;
		iSelectedAction = -1;
	}

	ImGui::Separator();

	// =========================================================
	// 2. Action (명령) 리스트 영역
	// =========================================================
	ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "[ Action List ]");
	if (iSelectedSubscriber != -1 && iSelectedSubscriber < vecCSSEventManifests.size())
	{
		auto& vecActions = vecCSSEventManifests[iSelectedSubscriber].vecActionNames;

		// [좌측 창]
		ImGui::BeginChild("ActionList", ImVec2(ImGui::GetWindowWidth() * 0.4f, 200), true);
		for (int i = 0; i < vecActions.size(); ++i)
		{
			bool bSelected = (iSelectedAction == i);

			if (ImGui::Selectable(vecActions[i].strNames.c_str(), bSelected))
			{
				iSelectedAction = i;

				// 기존 데이터 불러오기
				strcpy_s(szNewAction, sizeof(szNewAction), vecActions[i].strNames.c_str());
				strcpy_s(szNewActionExplain, sizeof(szNewActionExplain), vecActions[i].strExplain.c_str());
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// [우측 창]
		ImGui::BeginChild("ActionExplainBox", ImVec2(0, 200), true);
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Description:");
		ImGui::Separator();

		if (iSelectedAction != -1 && iSelectedAction < vecActions.size())
		{
			if (vecActions[iSelectedAction].strExplain.empty()) ImGui::TextDisabled("No description provided.");
			else ImGui::TextWrapped("%s", vecActions[iSelectedAction].strExplain.c_str());
		}
		else
		{
			ImGui::TextDisabled("Select an Action to view its description.");
		}
		ImGui::EndChild();

		// ---------------------------------------------------------
		// 하단: Action 데이터 입력 및 상태 제어 UI (★ 완벽하게 개선된 부분)
		// ---------------------------------------------------------
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// ★ [상태 표시 헤더] 현재 어떤 작업 중인지 명확하게 색깔로 알려줍니다!
		if (iSelectedAction == -1)
		{
			ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), ">>> Mode : [ Create New Action ]");
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), ">>> Mode : [ Edit Existing Action ]");
		}

		ImGui::Spacing();

		// 이름(Name) 입력칸
		ImGui::PushItemWidth(250.f);
		ImGui::InputText("Action Name##NewAction", szNewAction, sizeof(szNewAction));
		ImGui::PopItemWidth();

		ImGui::Spacing();

		// 설명(Explain) 입력칸 
		ImGui::Text("Action Explain:");
		ImGui::InputTextMultiline("##NewActionExplain", szNewActionExplain, sizeof(szNewActionExplain), ImVec2(ImGui::GetWindowWidth() * 0.8f, 70));

		ImGui::Spacing();

		// ★ [컨텍스트 기반 버튼 배치] 선택된 상태에 따라 나오는 버튼이 다릅니다.
		if (iSelectedAction == -1)
		{
			// [모드 1 : 추가 모드]
			if (ImGui::Button("Add New Action", ImVec2(130, 30)))
			{
				if (strlen(szNewAction) > 0)
				{
					bool bIsDuplicate = false;
					for (const auto& actionInfo : vecActions) {
						if (actionInfo.strNames == szNewAction) { bIsDuplicate = true; break; }
					}

					if (!bIsDuplicate)
					{
						ActionInfo newAction;
						newAction.strNames = szNewAction;
						newAction.strExplain = szNewActionExplain;
						vecActions.push_back(newAction);

						memset(szNewAction, 0, sizeof(szNewAction));
						memset(szNewActionExplain, 0, sizeof(szNewActionExplain));
					}
				}
			}
			ImGui::SameLine();
			// 쓰던 텍스트만 싹 지워주는 리셋 버튼
			if (ImGui::Button("Clear Text", ImVec2(100, 30)))
			{
				memset(szNewAction, 0, sizeof(szNewAction));
				memset(szNewActionExplain, 0, sizeof(szNewActionExplain));
			}
		}
		else
		{
			// [모드 2 : 수정/편집 모드]
			if (ImGui::Button("Apply Modify", ImVec2(130, 30)))
			{
				if (strlen(szNewAction) > 0)
				{
					vecActions[iSelectedAction].strNames = szNewAction;
					vecActions[iSelectedAction].strExplain = szNewActionExplain;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Action", ImVec2(120, 30)))
			{
				vecActions.erase(vecActions.begin() + iSelectedAction);
				iSelectedAction = -1;

				memset(szNewAction, 0, sizeof(szNewAction));
				memset(szNewActionExplain, 0, sizeof(szNewActionExplain));
			}
			ImGui::SameLine();
			// 수정을 취소하고 선택을 해제하여 다시 "추가 모드"로 돌아가는 버튼
			if (ImGui::Button("Cancel / Deselect", ImVec2(150, 30)))
			{
				iSelectedAction = -1; // 선택 해제!
				memset(szNewAction, 0, sizeof(szNewAction));
				memset(szNewActionExplain, 0, sizeof(szNewActionExplain));
			}
		}
	}
	else
	{
		ImGui::BeginChild("ActionListEmpty", ImVec2(0, 200), true);
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Please select a Subscriber first.");
		ImGui::EndChild();
	}

	ImGui::Separator();

	ImGui::Spacing();
	if (ImGui::Button("Save Manifest to JSON", ImVec2(-1, 40)))
	{
		Save_CCS_EventManifest(&vecCSSEventManifests);
	}

	ImGui::End();

	return S_OK;
}

HRESULT Tool::Load_CCS_EventManifest(OUT vector<struct CCS_EVENT_MANIFEST>* pOutData)
{
	if (pOutData == nullptr) return E_FAIL;
	std::ifstream ifs{ g_wszCameraCinematicSequnceEventManifest_JsonPath };
	if (!ifs.peek()) //엿보기 했는데 데이터가없다면 그냥 반환
		return S_OK;

	json LoadJson{};
	ifs >> LoadJson; //읽기
	*pOutData = LoadJson.get<std::vector<CCS_EVENT_MANIFEST>>();


	return S_OK;
}

HRESULT Tool::Save_CCS_EventManifest(const vector<struct CCS_EVENT_MANIFEST>* pSaveData)
{
	if (pSaveData == nullptr) return E_FAIL;
	std::ofstream ofs{ g_wszCameraCinematicSequnceEventManifest_JsonPath };

	json SaveJson{};
	SaveJson = *pSaveData;

	ofs << SaveJson.dump(4);
	ofs.close();

	return S_OK;
}
