#include "pch.h"
#include "Panel_State.h"
#include "AnimTool_Manager.h"

#include "BuilderSystem.h"
#include "Builder_MonsterState.h"

#include "DataDocument_MonsterState.h"
#include "DataStruct_MonsterState.h"

CPanel_State::CPanel_State(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
    m_pGameInstance(CGameInstance::GetInstance()),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance()),
	m_tAnimControllInfo(&m_pAnimToolManager->Get_AnimControllInfo())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CPanel_State::Initialize()
{
    if (FAILED(Ready_Builder()))
        return E_FAIL;

	return S_OK;
}

HRESULT CPanel_State::Ready_Builder()
{
    m_pBuilderSystem = CBuilderSystem::Create();
    if (m_pBuilderSystem == nullptr)  return E_FAIL;

    if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::MONSTER_STATE, CBuilder_MonsterState::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::ANIMATION)))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPanel_State::Render(CToolObject* pGo)
{
    StateEditor();

	return S_OK;
}

void CPanel_State::Update(const _float fTimeDelta)
{
}

void CPanel_State::StateEditor()
{
	ImGui::Begin("State editor");

    if (ImGui::Button("Save State"))
    {
        SyncStateNamesToSet();
        OpenSaveModal();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load State"))
    {
        OpenLoadModal();
    }

    // Load modal
    RenderLoadModal();

    // Save modal
    RenderSaveModal();

    ImGui::Separator();

    // 1. 최상위 정보 (strTag, global setStates)
    DrawTopLevelInfo();

    ImGui::Separator();

    ImGui::Separator();

    // 2. 글로벌 트랜지션
    DrawGlobalStateTransition();

    ImGui::Separator();

    // 좌우 패널 분할 (좌: State 리스트, 우: 상세 정보)
    ImGui::Columns(2, "SplitColumns", true);
    ImGui::SetColumnWidth(0, 250.0f); // 좌측 패널 너비 고정

    DrawStateList();

    ImGui::NextColumn();

    DrawStateDetails();

    ImGui::Columns(1);

	ImGui::End();
}

void CPanel_State::DrawTopLevelInfo()
{
    if (ImGui::CollapsingHeader("Root Monster Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        InputTextString("Monster Tag", m_MonsterData.strTag);

        ImGui::Spacing();
        ImGui::Text("Global State Names (setStates)");

        // setStates 추가 UI
        ImGui::InputText("##NewSetState", m_szGlobalSetBuf, sizeof(m_szGlobalSetBuf));
        ImGui::SameLine();
        if (ImGui::Button("Add State Name") && strlen(m_szGlobalSetBuf) > 0)
        {
            m_MonsterData.setStates.insert(m_szGlobalSetBuf);
            memset(m_szGlobalSetBuf, 0, sizeof(m_szGlobalSetBuf));
        }

        ImGui::SameLine();
        if (ImGui::Button("Auto Sync Names"))
        {
            SyncStateNamesToSet();
        }

        // setStates 목록 출력 및 삭제 UI
        ImGui::Indent();
        std::string eraseTarget = "";
        for (const auto& s : m_MonsterData.setStates)
        {
            ImGui::BulletText("%s", s.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - 60.f);
            ImGui::PushID(s.c_str());
            if (ImGui::Button("Del")) eraseTarget = s;
            ImGui::PopID();
        }
        if (!eraseTarget.empty()) m_MonsterData.setStates.erase(eraseTarget);
        ImGui::Unindent();
    }
}

void CPanel_State::DrawStateList()
{
    ImGui::BeginChild("StateListChild", ImVec2(0, 0), true);

    if (ImGui::Button("Add New State", ImVec2(-1, 0)))
    {
        DTO::MONSTER_STATEBASE_DESC newState;
        newState.strName = "New_State_" + std::to_string(m_MonsterData.vecMonsterStateDesc.size());
        m_MonsterData.vecMonsterStateDesc.push_back(newState);
        m_iSelectedIndex = (int)m_MonsterData.vecMonsterStateDesc.size() - 1;
    }

    ImGui::Separator();

    for (int i = 0; i < (int)m_MonsterData.vecMonsterStateDesc.size(); ++i)
    {
        ImGui::PushID(i);

        std::string displayLabel = m_MonsterData.vecMonsterStateDesc[i].strName;
        if (displayLabel.empty()) displayLabel = "UnNamed";

        bool bSelected = (m_iSelectedIndex == i);
        if (ImGui::Selectable(displayLabel.c_str(), bSelected))
        {
            m_iSelectedIndex = i;
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
}

void CPanel_State::DrawStateDetails()
{
    ImGui::BeginChild("StateDetailsChild", ImVec2(0, 0), true);

    if (m_iSelectedIndex < 0 || m_iSelectedIndex >= m_MonsterData.vecMonsterStateDesc.size())
    {
        ImGui::Text("Select a state to edit.");
        ImGui::EndChild();
        return;
    }

    DTO::MONSTER_STATEBASE_DESC& state = m_MonsterData.vecMonsterStateDesc[m_iSelectedIndex];

    // [삭제 버튼]
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Delete Current State", ImVec2(-1, 0)))
    {
        m_MonsterData.vecMonsterStateDesc.erase(m_MonsterData.vecMonsterStateDesc.begin() + m_iSelectedIndex);
        m_iSelectedIndex = -1;
        ImGui::PopStyleColor();
        ImGui::EndChild();
        return;
    }
    ImGui::PopStyleColor();
    ImGui::Separator();

    // 1. 기본 정보 (Basic Info)
    if (ImGui::CollapsingHeader("State Basic Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DrawStateComboBox("State Name", state.strName);

        ImGui::Checkbox("Is Boss", &state.bIsBoss);
        ImGui::SameLine();
        ImGui::Checkbox("Is Combo", &state.bIsCombo);

        // 플래그 및 옵션
        int aniFlags = state.FAniFlags;
        if (ImGui::InputInt("Anim Flags", &aniFlags)) state.FAniFlags = aniFlags;

        ImGui::Checkbox("Blend", &state.bBlend);
        ImGui::SameLine();
        ImGui::Checkbox("Loop", &state.bLoop);
    }

    // 2. 타이머 (Time Counters)
    if (ImGui::CollapsingHeader("Time Counters"))
    {
        DrawTimeCounter("Life Time", state.tStateLifeTime);
        DrawTimeCounter("CoolDown Time", state.tStateCoolDownTime);
    }

    // ※ 주의: state 내부의 setStates는 파싱 맵핑용이므로 툴에서는 숨김(또는 읽기전용) 처리했습니다.

    // 3. Anim Names
    if (ImGui::CollapsingHeader("Animations"))
    {
        ImGui::Text("Main Anim Names");

        static std::string s_selectedMainAnim = "";
        DrawAnimationComboBox("Main Anim", s_selectedMainAnim);
        ImGui::SameLine();
        if (ImGui::Button("Add Main Anim") && s_selectedMainAnim.size() > 0)
        {
            state.vecMainAnimNames.push_back(s_selectedMainAnim);
        }
        for (size_t i = 0; i < state.vecMainAnimNames.size(); ++i)
        {
            ImGui::PushID((int)i);
            InputTextString("##Anim", state.vecMainAnimNames[i]);
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                state.vecMainAnimNames.erase(state.vecMainAnimNames.begin() + i);
                i--;
            }
            ImGui::PopID();
        }

        ImGui::Separator();
        ImGui::Text("Pre Anim Names (Map)");

        static std::string s_selectedPoolState = "";
        static std::string s_selectedPoolAnim = "";
        DrawStateComboBox("Pre State(Key)", s_selectedPoolState);
        DrawAnimationComboBox("Pre Anim(Value)", s_selectedPoolAnim);

        if (ImGui::Button("Add PreAnim Pair") && s_selectedPoolState.size() > 0)
            state.mapPreAnimNames[s_selectedPoolState] = s_selectedPoolAnim;

        std::string mapEraseTarget = "";
        for (auto& pair : state.mapPreAnimNames)
        {
            ImGui::PushID(pair.first.c_str());
            ImGui::BulletText("%s : %s", pair.first.c_str(), pair.second.c_str());
            ImGui::SameLine(ImGui::GetWindowWidth() - 60.f);
            if (ImGui::Button("Del")) mapEraseTarget = pair.first;
            ImGui::PopID();
        }
        if (!mapEraseTarget.empty()) state.mapPreAnimNames.erase(mapEraseTarget);
    }

    // 4. State Transitions
    if (ImGui::CollapsingHeader("State Transitions"))
    {
        if (ImGui::Button("Add Transition"))
            state.vecStateTransition.push_back(DTO::STATE_TRANSITION());

        for (int i = 0; i < (int)state.vecStateTransition.size(); ++i)
        {
            DrawStateTransition(state.vecStateTransition[i], i);
            ImGui::PushID(i);
            if (ImGui::Button("Remove Transition"))
            {
                state.vecStateTransition.erase(state.vecStateTransition.begin() + i);
                i--;
            }
            ImGui::PopID();
            ImGui::Separator();
        }
    }

    // 5. State Features
    if (ImGui::CollapsingHeader("State Features"))
    {
        if (ImGui::Button("Add Features")) state.vecFeature.push_back("");
        for (size_t c = 0; c < state.vecFeature.size(); ++c)
        {
            ImGui::PushID((int)c);
            InputTextString("##Feat", state.vecFeature[c]);
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                state.vecFeature.erase(state.vecFeature.begin() + c);
                c--;
            }
            ImGui::PopID();
        }
    }

    ImGui::EndChild();
}

void CPanel_State::DrawTimeCounter(const char* label, DTO::MONSTERTIME_COUNTER& counter)
{
    if (ImGui::TreeNode(label))
    {
        ImGui::DragFloat("Acc Time", &counter.fTimeAcc, 0.01f);
        ImGui::DragFloat("Max Time", &counter.fMaxTime, 0.01f);
        ImGui::DragFloat("Min Time", &counter.fMinTime, 0.01f);
        ImGui::Checkbox("Count Time", &counter.bCountTime);
        ImGui::SameLine();
        ImGui::Checkbox("Time Reset", &counter.bTimeReset);
        ImGui::TreePop();
    }
}

void CPanel_State::DrawStateTransition(DTO::STATE_TRANSITION& transition, int index)
{
    ImGui::PushID(index);
    if (ImGui::TreeNode("Transition Node"))
    {
        if (ImGui::TreeNode("Conditions"))
        {
            if (ImGui::Button("Add Condition")) transition.vecCondition.push_back("");
            for (size_t c = 0; c < transition.vecCondition.size(); ++c)
            {
                ImGui::PushID((int)c);
                InputTextString("##Cond", transition.vecCondition[c]);
                ImGui::SameLine();
                if (ImGui::Button("X")) {
                    transition.vecCondition.erase(transition.vecCondition.begin() + c);
                    c--;
                }
                ImGui::PopID();
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Random State Pool"))
        {
            static std::string s_selectedPoolState = "";
            static float s_fWeight = 1.0f;

            DrawStateComboBox("Target State", s_selectedPoolState);
            ImGui::DragFloat("Weight", &s_fWeight, 0.1f);
            if (ImGui::Button("Add to Pool") && !s_selectedPoolState.empty())
            {
                transition.mapRandomStatePool[s_selectedPoolState] = s_fWeight;
                s_selectedPoolState = ""; // 초기화
                s_fWeight = 1.0f;
            }

            std::string poolEraseTarget = "";
            for (auto& pair : transition.mapRandomStatePool)
            {
                ImGui::PushID(pair.first.c_str());
                float weight = pair.second;
                if (ImGui::DragFloat(pair.first.c_str(), &weight, 0.1f)) {
                    transition.mapRandomStatePool[pair.first] = weight;
                }
                ImGui::SameLine();
                if (ImGui::Button("X")) poolEraseTarget = pair.first;
                ImGui::PopID();
            }
            if (!poolEraseTarget.empty()) transition.mapRandomStatePool.erase(poolEraseTarget);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

void CPanel_State::DrawGlobalStateTransition()
{
    // State Transitions
    if (ImGui::CollapsingHeader("State Transitions"))
    {
        if (ImGui::Button("Add Transition"))
            m_vecGlobalStateTransition.push_back(DTO::STATE_TRANSITION());
        ImGui::SameLine();
        if (ImGui::Button("Clear Global Transition"))
            m_vecGlobalStateTransition.clear();

        for (int i = 0; i < (int)m_vecGlobalStateTransition.size(); ++i)
        {
            DrawStateTransition(m_vecGlobalStateTransition[i], i);
            ImGui::PushID(i);
            if (ImGui::Button("Remove Transition"))
            {
                m_vecGlobalStateTransition.erase(m_vecGlobalStateTransition.begin() + i);
                i--;
            }
            ImGui::PopID();
            ImGui::Separator();
        }
    }
}

bool CPanel_State::InputTextString(const char* label, std::string& str)
{
    char buffer[256];
    strcpy_s(buffer, str.c_str());
    bool changed = ImGui::InputText(label, buffer, sizeof(buffer));
    if (changed) str = buffer;
    return changed;
}

void CPanel_State::SyncStateNamesToSet()
{
    //m_MonsterData.setStates.clear();
    //for (const auto& state : m_MonsterData.vecMonsterStateDesc)
    //{
    //    if (!state.strName.empty())
    //    {
    //        m_MonsterData.setStates.insert(state.strName);
    //    }
    //}

    for (auto& state : m_MonsterData.vecMonsterStateDesc)
    {
        state.setStates.clear();
        state.setStates = m_MonsterData.setStates;
    }
}

void CPanel_State::SyncGlobalStateTransition()
{
    for (auto& state : m_MonsterData.vecMonsterStateDesc)
    {
        state.vecGlobalStateTransition.clear();
        state.vecGlobalStateTransition = m_vecGlobalStateTransition;
    }
}

bool CPanel_State::DrawStateComboBox(const char* label, std::string& selectedState)
{
    bool isChanged = false;

    // 콤보박스 시작 (현재 선택된 상태 이름을 표시)
    std::string previewValue = selectedState.empty() ? "None" : selectedState;
    if (ImGui::BeginCombo(label, previewValue.c_str()))
    {
        // 최상위 글로벌 스테이트(setStates) 목록을 순회하며 리스트업
        for (const auto& globalState : m_MonsterData.setStates)
        {
            bool isSelected = (selectedState == globalState);

            // 항목 선택 시
            if (ImGui::Selectable(globalState.c_str(), isSelected))
            {
                selectedState = globalState;
                isChanged = true;
            }

            // 초기 포커스 설정
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return isChanged;
}

bool CPanel_State::DrawAnimationComboBox(const char* label, std::string& selectedAnim)
{
    bool isChanged = false;

    // 콤보박스 시작 (현재 선택된 상태 이름을 표시)
    std::string previewValue = selectedAnim.empty() ? "None" : selectedAnim;
    if (ImGui::BeginCombo(label, previewValue.c_str()))
    {
        // 최상위 글로벌 스테이트(setStates) 목록을 순회하며 리스트업
        for (const auto& animInfo : m_tAnimControllInfo->vecAnimInfo)
        {
            bool isSelected = (selectedAnim == animInfo.strAnimName);

            // 항목 선택 시
            if (ImGui::Selectable(animInfo.strAnimName.c_str(), isSelected))
            {
                selectedAnim = animInfo.strAnimName;
                isChanged = true;
            }

            // 초기 포커스 설정
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return isChanged;
}

HRESULT CPanel_State::Save(fs::path path)
{
    ELevelType eLevelType = ELevelType::ANIMATION;
    DTO::ECategory eCategory = DTO::ECategory::MONSTER_STATE;
    _uint iLevelID = ENUM_TO_UINT(eLevelType);

    if (eCategory != DTO::ECategory::MONSTER_STATE)
        return E_FAIL;

    if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_MonsterState>(iLevelID, eCategory)))
        return E_FAIL;

    CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, path);
    if (pDocument == nullptr)
        return E_FAIL;

    CDataDocument_MonsterState* pMonsterStateDoc = static_cast<CDataDocument_MonsterState*>(pDocument);

    if (pMonsterStateDoc == nullptr)
        return E_FAIL;

    DTO::MONSTERSTATE_DESC tData{};

    if (FAILED(pMonsterStateDoc->Try_Add(m_MonsterData)))
        return E_FAIL;

    m_pGameInstance->Save_File_Json(iLevelID, DTO::ECategory::MONSTER_STATE, path);
    return S_OK;
}

HRESULT CPanel_State::Load(fs::path path)
{
    ELevelType eLevelType = ELevelType::ANIMATION;
    DTO::ECategory eCategory = DTO::ECategory::MONSTER_STATE;
    _uint iLevelID = ENUM_TO_UINT(eLevelType);

    if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_MonsterState>(iLevelID, eCategory)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, path)))
        return E_FAIL;

    CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, path);
    if (pDocument == nullptr)
        return E_FAIL;

    CDataDocument_MonsterState* pMonsterStateDoc = static_cast<CDataDocument_MonsterState*>(pDocument);

    m_MonsterData = static_cast<CDataStruct_MonsterState*>(pMonsterStateDoc->Get_AllList()[0])->Get_Data();

    //m_pBuilderSystem->Build_File(ENUM_TO_UINT(ELevelType::ANIMATION), DTO::ECategory::MONSTER_STATE, path.filename().stem().string());

    return S_OK;
}

void CPanel_State::OpenFileDialog(char* buffer, const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        strcpy_s(buffer, 256, ofn.lpstrFile);
    }
}

void CPanel_State::SaveFileDialog(char* buffer, const char* filter)
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn) == TRUE)
    {
        strcpy_s(buffer, 256, ofn.lpstrFile);
    }
}

void CPanel_State::OpenLoadModal()
{
    m_tLoadOptions.Reset();

    ImGui::OpenPopup("Load Data Manager");
}

void CPanel_State::OpenSaveModal()
{
    m_tLoadOptions.Reset();

    ImGui::OpenPopup("Save Data Manager");
}

void CPanel_State::RenderLoadModal()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Load Data Manager", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300.f);
        ImGui::InputText("Animation Info", m_tLoadOptions.strStatePath, 256, ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("...##AnimBtn"))
        {
            OpenFileDialog(m_tLoadOptions.strStatePath, "JSON Files\0*.json\0All\0*.*\0");
        }

        ImGui::Separator();
        ImGui::Spacing();

        float contentWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(contentWidth - 120.f);

        if (ImGui::Button("OK", ImVec2(50, 0)))
        {
            if (strlen(m_tLoadOptions.strStatePath) > 0)
                Load(m_tLoadOptions.strStatePath);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(50, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CPanel_State::RenderSaveModal()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Save Data Manager", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300.f);
        ImGui::InputText("Animation Info", m_tLoadOptions.strStatePath, 256, ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button("...##AnimBtn"))
        {
            SaveFileDialog(m_tLoadOptions.strStatePath, "JSON Files\0*.json\0All\0*.*\0");
        }

        ImGui::Separator();
        ImGui::Spacing();

        float contentWidth = ImGui::GetContentRegionAvail().x;
        ImGui::SetCursorPosX(contentWidth - 120.f);

        if (ImGui::Button("OK", ImVec2(50, 0)))
        {
            if (strlen(m_tLoadOptions.strStatePath) > 0)
                Save(m_tLoadOptions.strStatePath);

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(50, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CPanel_State::SetAnimationObject()
{
	m_tAnimControllInfo = &m_pAnimToolManager->Get_AnimControllInfo();
}

CPanel_State* CPanel_State::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_State* pInstance = new CPanel_State(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_State is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_State::Free()
{
    Safe_Release(m_pBuilderSystem);
    Safe_Release(m_pGameInstance);

	Super::Free();
}
