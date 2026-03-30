#include "pch.h"
#include "Panel_State.h"
#include "AnimTool_Manager.h"

#include "StateBase.h"
#include "BuilderSystem.h"
#include "Builder_MonsterState.h"

#include "Registry_State.h"
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
        SyncGlobalStateTransition();
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

        ImGui::Indent();
        std::string eraseTarget = "";

        ImGui::BeginChild("##GlobalSetStatesScroll", ImVec2(0.f, 180.f), true,
            ImGuiWindowFlags_AlwaysVerticalScrollbar);

        for (const auto& s : m_MonsterData.setStates)
        {
            ImGui::BulletText("%s", s.c_str());
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 40.f);
            ImGui::PushID(s.c_str());
            if (ImGui::Button("Del", ImVec2(40.f, 0.f))) eraseTarget = s;
            ImGui::PopID();
        }

        ImGui::EndChild();

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
        ImGui::SameLine();
        ImGui::Checkbox("Is Custom (logic in client)", &state.bIsCustom);
    }

    // 플래그 및 옵션
    if (ImGui::CollapsingHeader("Anim Flags", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::TextDisabled("Current : 0x%04X", state.FAniFlags);

        _bool bHasPreAni = (state.FAniFlags & CStateBase::SA_HasPreAni) != 0;
        _bool bPreNonEvent = (state.FAniFlags & CStateBase::SA_PreNonEvent) != 0;
        _bool bWeaponAni = (state.FAniFlags & CStateBase::SA_WeaponAni) != 0;

        if (ImGui::Checkbox("SA_HasPreAni (0x0001) : Has pre animation", &bHasPreAni))
            state.FAniFlags = bHasPreAni ? (state.FAniFlags | CStateBase::SA_HasPreAni) : (state.FAniFlags & ~CStateBase::SA_HasPreAni);

        if (ImGui::Checkbox("SA_PreNonEvent (0x0002) : No state transition during pre ani", &bPreNonEvent))
            state.FAniFlags = bPreNonEvent ? (state.FAniFlags | CStateBase::SA_PreNonEvent) : (state.FAniFlags & ~CStateBase::SA_PreNonEvent);

        if (ImGui::Checkbox("SA_WeaponAni (0x0008) : Has weapon animation", &bWeaponAni))
            state.FAniFlags = bWeaponAni ? (state.FAniFlags | CStateBase::SA_WeaponAni) : (state.FAniFlags & ~CStateBase::SA_WeaponAni);

        ImGui::Separator();
        if (ImGui::Button("Clear Flags"))
            state.FAniFlags = 0;
        ImGui::SameLine();
        if (ImGui::Button("All Flags"))
            state.FAniFlags = CStateBase::SA_HasPreAni | CStateBase::SA_PreNonEvent | CStateBase::SA_WeaponAni;

        ImGui::Checkbox("Loop", &state.bLoop);
        ImGui::SameLine();
        ImGui::Checkbox("Blend", &state.bBlend);
        ImGui::InputFloat("Anim Speed", &state.fAnimationSpeed);
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
            if (ImGui::Button("X##Anim")) {
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

    // 3-1. Weapon Anim Names
    if (ImGui::CollapsingHeader("Weapon Anim Names"))
    {
        static std::string s_newWeaponAnimName;

        ImGui::SetNextItemWidth(260.f);
        ImGui::InputText("##NewWeaponAnimName", &s_newWeaponAnimName);
        ImGui::SameLine();
        if (ImGui::Button("Add##WeaponAnim") && !s_newWeaponAnimName.empty())
        {
            state.vecWeaponAnimNames.push_back(s_newWeaponAnimName);
            s_newWeaponAnimName.clear();
        }

        for (size_t i = 0; i < state.vecWeaponAnimNames.size(); ++i)
        {
            ImGui::PushID((int)i);
            ImGui::SetNextItemWidth(260.f);
            ImGui::InputText("##WeaponAnimEdit", &state.vecWeaponAnimNames[i]);
            ImGui::SameLine();
            if (ImGui::Button("X##WeaponAnimEdit"))
            {
                state.vecWeaponAnimNames.erase(state.vecWeaponAnimNames.begin() + i);
                ImGui::PopID();
                --i;
                continue;
            }
            ImGui::PopID();
        }
    }


    // 4. State Transitions
    if (ImGui::CollapsingHeader("State Transitions"))
    {
        if (ImGui::Button("Add Transition"))
            state.vecStateTransition.push_back(DTO::STATE_TRANSITION());

        for (int i = 0; i < (int)state.vecStateTransition.size(); ++i)
        {
            DrawStateTransition(state.vecStateTransition[i], i, ETransOwner::Local);

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

    // 로컬 transition 모달 렌더: 루프 밖에서 1번만
    if (m_eCondOwner == ETransOwner::Local &&
        m_iCondTransIndex >= 0 &&
        m_iCondTransIndex < (int)state.vecStateTransition.size())
    {
        if (m_bReqOpenCondPopup == true)
        {
            ImGui::OpenPopup("ConditionEntry Editor");
            m_bReqOpenCondPopup = false;
        }

        RenderConditionEntryModal(state.vecStateTransition[m_iCondTransIndex]);
    }

    // 5. State Features
    if (ImGui::CollapsingHeader("State Features"))
    {
        if (ImGui::Button("Add Feature (Param)", ImVec2(-1, 0)))
        {
            m_tFeatDraft = DTO::FEATURE_ENTRY{};
            m_iFeatEditIndex = -1;
            m_bReqOpenFeatPopup = true;
        }

        ImGui::Separator();

        if (ImGui::BeginTable("##FeatEntryTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_WidthFixed, 40.f);
            ImGui::TableSetupColumn("Feature");
            ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 120.f);
            ImGui::TableHeadersRow();

            int eraseIdx = -1;

            for (int i = 0; i < (int)state.vecFeatureEntry.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", i);

                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(state.vecFeatureEntry[i].strFeature.c_str());

                ImGui::TableSetColumnIndex(2);
                if (ImGui::SmallButton("Edit"))
                {
                    m_tFeatDraft = state.vecFeatureEntry[i];
                    m_iFeatEditIndex = i;
                    m_bReqOpenFeatPopup = true;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Del"))
                    eraseIdx = i;

                ImGui::PopID();
            }

            ImGui::EndTable();

            if (eraseIdx >= 0)
                state.vecFeatureEntry.erase(state.vecFeatureEntry.begin() + eraseIdx);
        }

        if (m_bReqOpenFeatPopup == true)
        {
            ImGui::OpenPopup("FeatureEntry Editor");
            m_bReqOpenFeatPopup = false;
        }
        RenderFeatureEntryModal(state);
    }

    // 6. Condition Features
    RenderConditionFeatureSection(state, "Start Condition Features (OnEnter!)", "##StartCondFeatTable", ECondFeatState::Start);
    RenderConditionFeatureSection(state, "Update Condition Features", "##UpdateCondFeatTable", ECondFeatState::Update);
    RenderConditionFeatureSection(state, "End Condition Features (OnExit)", "##EndCondFeatTable", ECondFeatState::End);

    if (m_bReqOpenCondFeatPopup == true)
    {
        ImGui::OpenPopup("ConditionFeature Editor");
        m_bReqOpenCondFeatPopup = false;
    }

    RenderConditionFeatureModal(state);

    ImGui::EndChild();
}

void CPanel_State::DrawTimeCounter(const char* label, DTO::MONSTERTIME_COUNTER& counter)
{
    if (ImGui::TreeNode(label))
    {
        ImGui::DragFloat("Max Time", &counter.fMaxTime, 0.01f);
        ImGui::DragFloat("Min Time", &counter.fMinTime, 0.01f);
        ImGui::Checkbox("Count Time", &counter.bCountTime);
        ImGui::SameLine();
        ImGui::Checkbox("Time Reset", &counter.bTimeReset);
        ImGui::TreePop();
    }
}

void CPanel_State::DrawStateTransition(DTO::STATE_TRANSITION& transition, int index, ETransOwner eOwner)
{
    ImGui::PushID(index);
    if (ImGui::TreeNode("Transition Node"))
    {
        if (ImGui::TreeNode("Conditions"))
        {
            if (ImGui::Button("Add Condition (Param)"))
            {
                m_tCondDraft = DTO::CONDITION_ENTRY{};
                m_iCondEditIndex = -1;
                m_eCondOwner = eOwner;
                m_iCondTransIndex = index;
                m_bReqOpenCondPopup = true;
            }

            int eraseIdx = -1;
            for (int c = 0; c < (int)transition.vecConditionEntry.size(); ++c)
            {
                ImGui::PushID(c);

                ImGui::BulletText("%s", transition.vecConditionEntry[c].strCondition.c_str());
                ImGui::SameLine();

                if (ImGui::SmallButton("Edit##Cond"))
                {
                    m_tCondDraft = transition.vecConditionEntry[c];
                    m_iCondEditIndex = c;
                    m_eCondOwner = eOwner;
                    m_iCondTransIndex = index;
                    m_bReqOpenCondPopup = true;
                }
                ImGui::SameLine();

                if (ImGui::SmallButton("X##Cond"))
                    eraseIdx = c;

                ImGui::PopID();
            }

            if (eraseIdx >= 0)
                transition.vecConditionEntry.erase(transition.vecConditionEntry.begin() + eraseIdx);

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
                if (ImGui::Button("X##Weight")) poolEraseTarget = pair.first;
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
    if (ImGui::CollapsingHeader("Global State Transitions"))
    {
        if (ImGui::Button("Add Global Transition"))
            m_vecGlobalStateTransition.push_back(DTO::STATE_TRANSITION());
        ImGui::SameLine();
        if (ImGui::Button("Clear Global Transition"))
            m_vecGlobalStateTransition.clear();

        for (int i = 0; i < (int)m_vecGlobalStateTransition.size(); ++i)
        {
            DrawStateTransition(m_vecGlobalStateTransition[i], i, ETransOwner::Global);
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

    if (m_eCondOwner == ETransOwner::Global &&
        m_iCondTransIndex >= 0 &&
        m_iCondTransIndex < (int)m_vecGlobalStateTransition.size())
    {
        if (m_bReqOpenCondPopup == true)
        {
            ImGui::OpenPopup("ConditionEntry Editor");
            m_bReqOpenCondPopup = false;
        }

        RenderConditionEntryModal(m_vecGlobalStateTransition[m_iCondTransIndex]);
    }
}

bool CPanel_State::InputTextString(const char* label, std::string& str)
{
    char buffer[MAX_PATH]{};
    strcpy_s(buffer, MAX_PATH, str.c_str());
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

void CPanel_State::DrawRegistryPicker(const _char* comboID, const _char* const* items, _int iCount, string& strIOvalue, string& strIOfilter)
{
    if (ImGui::BeginCombo(comboID, "Pick..."))
    {
        ImGui::PushID(comboID);

        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##filter", &strIOfilter);
        ImGui::Separator();

        for (_int i = 0; i < iCount; ++i)
        {
            const _char* name = items[i];
            if (PassFilter(name, strIOfilter) == false)
                continue;

            _bool selected = (strIOvalue == name);
            if (ImGui::Selectable(name, selected))
                strIOvalue = name;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::PopID();
        ImGui::EndCombo();
    }
}

_bool CPanel_State::PassFilter(const _char* item, const string& strFilter)
{
    if (strFilter.empty())
        return true;
    return std::string_view(item).find(strFilter) != std::string_view::npos;
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
    
    if (m_MonsterData.vecMonsterStateDesc.size() > 0)
        m_vecGlobalStateTransition = m_MonsterData.vecMonsterStateDesc.front().vecGlobalStateTransition;
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
    ofn.lpstrDefExt = "json";
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
            OpenFileDialog(m_tLoadOptions.strStatePath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
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
            SaveFileDialog(m_tLoadOptions.strStatePath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
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

void CPanel_State::RenderStateParamEditor(DTO::STATE_PARAM& param)
{
    ImGui::PushID(&param);

    ImGui::SeparatorText("Params");
    ImGui::InputInt4("iParam[4]", param.iParam);
    ImGui::InputFloat4("fParam[4]", param.fParam);

    ImGui::Text("bParam[4]");
    ImGui::Checkbox("##b0", &param.bParam[0]); ImGui::SameLine();
    ImGui::Checkbox("##b1", &param.bParam[1]); ImGui::SameLine();
    ImGui::Checkbox("##b2", &param.bParam[2]); ImGui::SameLine();
    ImGui::Checkbox("##b3", &param.bParam[3]);

    ImGui::PopID();
}

void CPanel_State::RenderFeatureEntryModal(DTO::MONSTER_STATEBASE_DESC& state)
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("FeatureEntry Editor", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        const _float wInput = 320.f;

        ImGui::Text("Feature");
        ImGui::Separator();

        ImGui::SetNextItemWidth(wInput);
        ImGui::InputText("##FeatName", &m_tFeatDraft.strFeature);
        ImGui::SameLine();
        DrawRegistryPicker("##FeatPick",
            kFeatureNames, kFeatureCount,
            m_tFeatDraft.strFeature, m_filterFeat);

        const bool valid = IsKnownFeature(m_tFeatDraft.strFeature);
        if (!m_tFeatDraft.strFeature.empty() && !valid)
            ImGui::Text("Unknown feature (not in Registry_State.h)");

        ImGui::Checkbox("Is Once", &m_tFeatDraft.IsOnce);

        RenderStateParamEditor(m_tFeatDraft.tParam);

        ImGui::Separator();

        if (!m_errFeatModal.empty())
            ImGui::Text("%s", m_errFeatModal.c_str());

        if (ImGui::Button("OK", ImVec2(100.f, 0.f)))
        {
            if (!valid && m_bStrictNameCheck)
            {
                m_errFeatModal = "Unknown feature name. Add it to Tool registry.";
            }
            else
            {
                m_errFeatModal.clear();

                if (m_iFeatEditIndex < 0) state.vecFeatureEntry.push_back(m_tFeatDraft);
                else state.vecFeatureEntry[m_iFeatEditIndex] = m_tFeatDraft;

                m_iFeatEditIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.f, 0.f)))
        {
            m_iFeatEditIndex = -1;
            m_errFeatModal.clear();
            m_filterFeat.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CPanel_State::RenderConditionEntryModal(DTO::STATE_TRANSITION& transition)
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("ConditionEntry Editor", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        const _float wInput = 320.f;

        ImGui::Text("Condition");
        ImGui::Separator();

        ImGui::SetNextItemWidth(wInput);
        ImGui::InputText("##CondName", &m_tCondDraft.strCondition);
        ImGui::SameLine();
        DrawRegistryPicker("##CondPick",
            kConditionNames, kConditionCount,
            m_tCondDraft.strCondition, m_filterCond);

        const _bool bValid = IsKnownCondition(m_tCondDraft.strCondition);
        if (!m_tCondDraft.strCondition.empty() && !bValid)
            ImGui::Text("Unknown condition (not in Registry_State.h)");

        RenderStateParamEditor(m_tCondDraft.tParam);

        ImGui::Separator();

        if (!m_errCondModal.empty())
            ImGui::Text("%s", m_errCondModal.c_str());

        if (ImGui::Button("OK", ImVec2(100.f, 0.f)))
        {
            if (!bValid && m_bStrictNameCheck)
            {
                m_errCondModal = "Unknown condition name. Add it to Tool registry.";
            }
            else
            {
                m_errCondModal.clear();

                if (m_iCondEditIndex < 0) transition.vecConditionEntry.push_back(m_tCondDraft);
                else transition.vecConditionEntry[m_iCondEditIndex] = m_tCondDraft;

                m_iCondEditIndex = -1;
                m_iCondTransIndex = -1;
                m_eCondOwner = ETransOwner::None;

                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.f, 0.f)))
        {
            // 리셋
            m_iCondEditIndex = -1;
            m_iCondTransIndex = -1;
            m_eCondOwner = ETransOwner::None;
            m_errCondModal.clear();
            m_filterCond.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CPanel_State::RenderConditionFeatureModal(DTO::MONSTER_STATEBASE_DESC& state)
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("ConditionFeature Editor", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        const _float wInput = 320.f;

        // 현재 편집중인 conditionfeature 표시
        {
            const _char* label{ nullptr };
            {
                switch (m_eCondFeatState)
                {
                case Tool::CPanel_State::ECondFeatState::Start:
                    label = "OnEnter";
                    break;
                case Tool::CPanel_State::ECondFeatState::Update:
                    label = "Update";
                    break;
                case Tool::CPanel_State::ECondFeatState::End:
                default:
                    label = "OnExit";
                    break;
                }
            }

            RenderRedBanner(label);
            ImGui::Separator();
        }

        // ---- Condition side ----
        ImGui::SeparatorText("Condition");
        ImGui::SetNextItemWidth(wInput);
        ImGui::InputText("Condition", &m_tCondFeatDraft.cond.strCondition);
        ImGui::SameLine();
        
        // Registry에 있는것 그림
        {
            DrawRegistryPicker("##CFCondPick",
                kConditionNames, kConditionCount,
                m_tCondFeatDraft.cond.strCondition, m_filterCondFeat_Cond);
        }

        // Registry와 싱크가 맞는가?
        const _bool bCondOk = 
            m_tCondFeatDraft.cond.strCondition.empty()
            ? true
            : IsKnownCondition(m_tCondFeatDraft.cond.strCondition);
        // 경고 텍스트
        {
            if (!m_tCondFeatDraft.cond.strCondition.empty() && !bCondOk)
                ImGui::Text("Unknown condition (not in Registry_State.h)");
            else if (m_tCondFeatDraft.cond.strCondition.empty())
                m_tCondFeatDraft.cond.strCondition = "condition_true_always";
        }

        // Parameter Editor 모달 그림
        RenderStateParamEditor(m_tCondFeatDraft.cond.tParam);

        // ---- Feature side ----
        ImGui::SeparatorText("Feature");
        ImGui::SetNextItemWidth(wInput);
        ImGui::InputText("Feature", &m_tCondFeatDraft.feat.strFeature);
        ImGui::SameLine();

        // Registry에 있는것 그림
        {
            DrawRegistryPicker("##CFFeatPick",
                kFeatureNames, kFeatureCount,
                m_tCondFeatDraft.feat.strFeature, m_filterCondFeat_Feat);
        }

        // Registry와 싱크가 맞는가?
        const _bool bFeatOk = IsKnownFeature(m_tCondFeatDraft.feat.strFeature);
        // 경고 텍스트
        {
            if (!m_tCondFeatDraft.feat.strFeature.empty() && !bFeatOk)
                ImGui::Text("Unknown feature (not in Registry_State.h)");
        }

        ImGui::Checkbox("Is Once", &m_tCondFeatDraft.feat.IsOnce);

        // Parameter Editor 모달 그림
        RenderStateParamEditor(m_tCondFeatDraft.feat.tParam);

        ImGui::Separator();

        // OK
        if (ImGui::Button("OK", ImVec2(100.f, 0.f)))
        {
            // Feature는 필수
            if (m_tCondFeatDraft.feat.strFeature.empty())
            {
                m_errCondFeatModal = "Feature is empty.";
            }
            else if (m_bStrictNameCheck && (!bCondOk || !bFeatOk))
            {
                m_errCondFeatModal = "Unknown condition/feature (not in Registry_State.h)";
            }
            else
            {
                m_errCondFeatModal.clear();

                // 스테이트에 따라 자동으로 저장 벡터 설정
                auto& vecConditionFeatures = Get_ConditionFeatures(state, m_eCondFeatState);

                if (m_iCondFeatEditIndex < 0)
                    vecConditionFeatures.push_back(m_tCondFeatDraft);
                else
                    vecConditionFeatures[m_iCondFeatEditIndex] = m_tCondFeatDraft;

                m_iCondFeatEditIndex = -1;
                m_filterCondFeat_Cond.clear();
                m_filterCondFeat_Feat.clear();
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.f, 0.f)))
        {
            m_iCondFeatEditIndex = -1;
            m_errCondFeatModal.clear();
            m_filterCondFeat_Cond.clear();
            m_filterCondFeat_Feat.clear();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void CPanel_State::RenderConditionFeatureSection(DTO::MONSTER_STATEBASE_DESC& state, const _char* headerLabel, const _char* tableID, ECondFeatState eState)
{
    if (ImGui::CollapsingHeader(headerLabel) == false)
        return;

    // Add 버튼
    {
        ImGui::PushID(headerLabel);

        std::string strButton = "Add ConditionFeature##";
        strButton += headerLabel;

        if (ImGui::Button(strButton.c_str(), ImVec2(-1, 0)))
        {
            m_tCondFeatDraft = DTO::CONDITIONFEATURE_ENTRY{};
            m_iCondFeatEditIndex = -1;
            m_eCondFeatState = eState;
            m_bReqOpenCondFeatPopup = true;
        }

        ImGui::Separator();
        ImGui::PopID();
    }

    auto& vecConditionFeatures = Get_ConditionFeatures(state, eState);

    if (ImGui::BeginTable(tableID, 4,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_WidthFixed, 40.f);
        ImGui::TableSetupColumn("Condition");
        ImGui::TableSetupColumn("Feature");
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 140.f);
        ImGui::TableHeadersRow();

        _int iEraseIndex = -1;

        for (_int i = 0; i < (_int)vecConditionFeatures.size(); ++i)
        {
            auto& coditionFeature = vecConditionFeatures[i];

            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", i);

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(coditionFeature.cond.strCondition.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(coditionFeature.feat.strFeature.c_str());

            ImGui::TableSetColumnIndex(3);
            if (ImGui::SmallButton("Edit"))
            {
                m_tCondFeatDraft = coditionFeature;
                m_iCondFeatEditIndex = i;
                m_eCondFeatState = eState;
                m_bReqOpenCondFeatPopup = true;
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("Del"))
                iEraseIndex = i;

            ImGui::PopID();
        }

        ImGui::EndTable();

        if (iEraseIndex >= 0)
            vecConditionFeatures.erase(vecConditionFeatures.begin() + iEraseIndex);
    }
}

void CPanel_State::RenderRedBanner(const _char* text)
{
    const float w = ImGui::GetContentRegionAvail().x;
    const ImVec2 size(w, 0.f);

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.00f, 0.f, 0.f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));

    ImGui::BeginDisabled(true);
    ImGui::Button(text, size);
    ImGui::EndDisabled();

    ImGui::PopStyleColor(2);
}

vector<DTO::CONDITIONFEATURE_ENTRY>& CPanel_State::Get_ConditionFeatures(DTO::MONSTER_STATEBASE_DESC& state, ECondFeatState eCondFeatState)
{
    switch (eCondFeatState)
    {
    case Tool::CPanel_State::ECondFeatState::Start:
        return state.vecStartConditionFeature;
    case Tool::CPanel_State::ECondFeatState::Update:
        return state.vecConditionFeature;
    case Tool::CPanel_State::ECondFeatState::End:
    default:
        return state.vecEndConditionFeature;
    }
}

const _char* CPanel_State::ConditionFeatureState(ECondFeatState eState)
{
    switch (eState)
    {
    case ECondFeatState::Start:
        return "Start (OnEnter)";
    case ECondFeatState::Update:
        return "Update";
    case ECondFeatState::End:
    default:
        return "End (OnExit)";
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
