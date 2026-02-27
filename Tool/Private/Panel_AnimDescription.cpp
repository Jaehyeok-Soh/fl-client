#include "pch.h"
#include "Panel_AnimDescription.h"
#include "AnimTool_Manager.h"

CPanel_AnimDescription::CPanel_AnimDescription(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance()),
	m_tAnimControllInfo(&m_pAnimToolManager->Get_AnimControllInfo()),
	m_tEventInfo(&m_pAnimToolManager->Get_AnimEventInfo())
{
}

HRESULT CPanel_AnimDescription::Initialize()
{
	return S_OK;
}

HRESULT CPanel_AnimDescription::Render(CToolObject* pGo)
{
	Description_TabWindow();

	return S_OK;
}

void CPanel_AnimDescription::Update(const _float fTimeDelta)
{
}

void CPanel_AnimDescription::SetAnimationObject()
{
	m_tAnimControllInfo = &m_pAnimToolManager->Get_AnimControllInfo();
	m_tEventInfo = &m_pAnimToolManager->Get_AnimEventInfo();
}

BONEINFO CPanel_AnimDescription::GetBoneInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return BONEINFO();

	return m_tAnimControllInfo->vecBoneInfo[index];
}

ANIMINFO CPanel_AnimDescription::GetAnimInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return ANIMINFO();

	return m_tAnimControllInfo->vecAnimInfo[index];
}

void CPanel_AnimDescription::ModifyOne(_uint eventIdx, DTO::ATTACKEVENT event)
{
    m_pAnimToolManager->Modify_AttackOverlap(m_tAnimControllInfo->iCurrentAttackEventIndex, event);;
}

void CPanel_AnimDescription::Description_TabWindow()
{
	ImGui::Begin("Description Editor");

	if (ImGui::BeginTabBar("Descriptions", ImGuiTabBarFlags_Reorderable))
	{
		if (ImGui::BeginTabItem("Hitbox"))
		{
            if (m_tAnimControllInfo->iCurrentAttackEventIndex != -1 && m_tAnimControllInfo->iCurrentAttackEventIndex < m_tEventInfo->vecAttackEvents.size())
                Desc_AttackOverlapWindow();
            else
                ImGui::Text("Select an attack event from the timeline.");

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Effect"))
		{
            if (m_tAnimControllInfo->iCurrentEffectEventIndex != -1 &&
                m_tAnimControllInfo->iCurrentEffectEventIndex < m_tEventInfo->vecVFXEvents.size())
            {
                Desc_EffectWindow(); 
            }
            else
            {
                ImGui::Text("Select an effect event from the timeline.");
            }
            ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Bone"))
		{
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Animation"))
		{
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CPanel_AnimDescription::Desc_AttackOverlapWindow()
{
    static constexpr const _char* s_AnimNotifyPhaseItems[] =
    {
        "Immediatley",
        "Late",
        "PreRender"
    };

    auto pEvent = &m_tEventInfo->vecAttackEvents[m_tAnimControllInfo->iCurrentAttackEventIndex];

    if (pEvent == nullptr)
    {
        ImGui::TextDisabled("No Attack Event Selected");
        return;
    }

    if (ImGui::Button("Delete this"))
        m_pAnimToolManager->Open_ConfirmModal("Delete_Attack_Event");

    if (1 == m_pAnimToolManager->Render_ConfirmModal("Delete_Attack_Event", "realy sure delete this attack event?"))
    {
        _uint idxAcc = { 0 };
        m_tEventInfo->vecAttackEvents.erase(std::remove_if(m_tEventInfo->vecAttackEvents.begin(),
            m_tEventInfo->vecAttackEvents.end(),
            [&idxAcc, &delIdx = m_tAnimControllInfo->iCurrentAttackEventIndex](const auto&) mutable {return idxAcc++ == delIdx; }
        ),
        m_tEventInfo->vecAttackEvents.end());

        m_tAnimControllInfo->iCurrentAttackEventIndex = -1;

        m_pAnimToolManager->Modify_AttackOverlap(m_tEventInfo->vecAttackEvents);;
    }

    // ID 충돌 방지 (포인터 주소 사용)
    ImGui::PushID(pEvent);

    // =========================================================
    // 1. 기본 이벤트 정보 (ANIM_EVENT_BASE1)
    // =========================================================
    if (ImGui::CollapsingHeader("Event Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 시작 시간 (Start Track Position)
        // 틱 단위인지 초 단위인지에 따라 speed 조절 (여기선 0.01f)
        if (ImGui::DragFloat("Start Time", &pEvent->fStartTrackPosition, 0.01f, 0.0f, 1000.0f, "%.2f"))
            ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);

        // 설명 (std::string 수정)
        // ImGui::InputText는 char* 버퍼가 필요하므로 std::string과 연동하려면 약간의 트릭이 필요합니다.
        // 여기서는 편의상 고정 버퍼로 예시를 듭니다.
        static char descBuf[256];
        strcpy_s(descBuf, pEvent->strDescription.c_str());
        if (ImGui::InputText("Description", descBuf, 256))
        {
            pEvent->strDescription = descBuf;
        }

        ImGui::SeparatorText("EventNotifyPhase");
        {
            _int iAnimNotifyPhase = static_cast<_int>(pEvent->ePhase);
            if (ImGui::Combo("EventNotifyPhase##Combo", &iAnimNotifyPhase, s_AnimNotifyPhaseItems, static_cast<_int>(Engine::g_AnimNotifyPhaseTypeCount)))
                pEvent->ePhase = static_cast<Engine::EAnimNotifyPhase>(iAnimNotifyPhase);
        }

        // 애니메이션 태그
        static char tagBuf[256];
        strcpy_s(tagBuf, pEvent->strAnimTag.c_str());
        if (ImGui::InputText("Anim Tag", tagBuf, 256))
        {
            pEvent->strAnimTag = tagBuf;
        }
    }

    // =========================================================
    // 2. 히트박스 기본 설정 (ANIM_EVENT_SCRIPT_BASE1)
    // =========================================================
    if (ImGui::CollapsingHeader("Hitbox Duration", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 지속 시간 (Duration)
        if (ImGui::DragFloat("Duration", &pEvent->tHitboxDesc.fDuration, 0.01f, 0.0f, 100.0f, "%.2f s"))
            ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("How long the hitbox stays active");
    }

    // =========================================================
    // 3. 모양 및 트랜스폼 (Transform & Shape)
    // =========================================================
    if (ImGui::CollapsingHeader("Shape & Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DTO::HITBOX_DESC& desc = pEvent->tHitboxDesc;

        // 오프셋 (위치)
        if (ImGui::DragFloat3("Offset Position", (float*)&desc.vOffset, 0.01f))
            ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);

        // 모양 선택 (Combo Box)
        // 실제 Enum 값(int)으로 캐스팅해서 제어
        int currentType = (int)desc.eType;
        if (ImGui::Combo("Shape Type", &currentType, overlapTypeItems, IM_ARRAYSIZE(overlapTypeItems)))
        {
            desc.eType = (EOverlapType::Enum)currentType;
            ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);
        }

        ImGui::Separator();

        // 모양에 따라 필요한 입력값만 보여주기
        switch (desc.eType)
        {
        case EOverlapType::BOX:
        {
            if (ImGui::DragFloat3("Extents (Half-Size)", (float*)&desc.vExtents, 0.01f, 0.0f, 100.0f))
                ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);
        }
            break;

        case EOverlapType::SPHERE:
        {
            if (ImGui::DragFloat("Radius", &desc.fRadius, 0.01f, 0.0f, 100.0f))
                ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);
        }
            break;

        case EOverlapType::CAPSULE: // 보통 Radius와 Height(또는 Extent) 사용
        {
            if (ImGui::DragFloat("Radius", &desc.fRadius, 0.01f, 0.0f, 100.0f))
                ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);
            
            if (ImGui::DragFloat("Half Height", &desc.fHeight, 0.01f, 0.0f, 100.0f))
                ModifyOne(m_tAnimControllInfo->iCurrentAttackEventIndex, *pEvent);
        }
            break;
        }
    }

    // =========================================================
    // 4. 게임플레이 로직 (Gameplay Logic)
    // =========================================================
    if (ImGui::CollapsingHeader("Gameplay Logic", ImGuiTreeNodeFlags_DefaultOpen))
    {
        DTO::HITBOX_DESC& desc = pEvent->tHitboxDesc;

        // 데미지
        ImGui::DragFloat("Damage", &desc.fDamage, 1.0f, 0.0f, 99999.f);

        // 최대 히트 수
        int maxHit = (int)desc.iMaxHit;
        if (ImGui::DragInt("Max Hit Count", &maxHit, 1, 1, 100))
        {
            desc.iMaxHit = (_uint)maxHit;
        }

        // 틱 타임 (연타 간격)
        ImGui::DragFloat("Tick Interval", &desc.fTickTime, 0.01f, -1.0f, 10.0f);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("-1.0: Hit Once\n> 0.0: Hit every X seconds");
    }

    // =========================================================
    // 5. 물리 필터 (Physics Filter)
    // =========================================================
    if (ImGui::CollapsingHeader("Physics Filter"))
    {
        DTO::HITBOX_DESC& desc = pEvent->tHitboxDesc;

        // 필터 레이어 (누가 때리는 것인가?)
        int currentLayer = 0;
        int tempLayer = (int)desc.eFilterLayer;
        while (tempLayer >>= 1)
            currentLayer++;

        if (ImGui::Combo("Filter Layer", &currentLayer, filterGroupItems, IM_ARRAYSIZE(filterGroupItems)))
            desc.eFilterLayer = static_cast<PHYSICSFILTERGROUP::Enum>(1 << currentLayer);

        // 필터 마스크 (누구를 때릴 것인가?) - 비트마스크 입력은 CheckboxGroup 추천
        ImGui::Text("Filter Mask (Target Layer)");

        ImGui::CheckboxFlags("Player", &desc.iFilterMask, PHYSICSFILTERGROUP::PLAYER);
        ImGui::CheckboxFlags("ATTACK", &desc.iFilterMask, PHYSICSFILTERGROUP::ATTACK);
        ImGui::CheckboxFlags("SKILL", &desc.iFilterMask, PHYSICSFILTERGROUP::SKILL);
        ImGui::CheckboxFlags("ATTACK_PROJECTTILE", &desc.iFilterMask, PHYSICSFILTERGROUP::ATTACK_PROJECTTILE);
        ImGui::CheckboxFlags("SKILL_PROJECTTILE", &desc.iFilterMask, PHYSICSFILTERGROUP::SKILL_PROJECTTILE);
        ImGui::CheckboxFlags("MONSTER", &desc.iFilterMask, PHYSICSFILTERGROUP::MONSTER);
        ImGui::CheckboxFlags("MONSTER_ATTACK", &desc.iFilterMask, PHYSICSFILTERGROUP::MONSTER_ATTACK);
        ImGui::CheckboxFlags("MONSTER_SKILL", &desc.iFilterMask, PHYSICSFILTERGROUP::MONSTER_SKILL);
        ImGui::CheckboxFlags("MONSTER_ATTACK_PROJECTTILE", &desc.iFilterMask, PHYSICSFILTERGROUP::MONSTER_ATTACK_PROJECTTILE);
        ImGui::CheckboxFlags("MONSTER_SKILL_PROJECTTILE", &desc.iFilterMask, PHYSICSFILTERGROUP::MONSTER_SKILL_PROJECTTILE);
        ImGui::CheckboxFlags("MAP", &desc.iFilterMask, PHYSICSFILTERGROUP::MAP);
        ImGui::CheckboxFlags("OBJECT1", &desc.iFilterMask, PHYSICSFILTERGROUP::OBJECT1);
        ImGui::CheckboxFlags("OBJECT2", &desc.iFilterMask, PHYSICSFILTERGROUP::OBJECT2);
        ImGui::CheckboxFlags("TRIGGER_UI", &desc.iFilterMask, PHYSICSFILTERGROUP::TRIGGER_UI);
        ImGui::CheckboxFlags("TRIGGER_QUEST", &desc.iFilterMask, PHYSICSFILTERGROUP::TRIGGER_QUEST);
        ImGui::CheckboxFlags("TRIGGER_SPAWN", &desc.iFilterMask, PHYSICSFILTERGROUP::TRIGGER_SPAWN);
        ImGui::CheckboxFlags("TRIGGER_DIRECTION", &desc.iFilterMask, PHYSICSFILTERGROUP::TRIGGER_DIRECTION);
        ImGui::CheckboxFlags("NONE", &desc.iFilterMask, PHYSICSFILTERGROUP::NONE);
    }

    ImGui::PopID();
}
void CPanel_AnimDescription::Desc_EffectWindow()
{
    static constexpr const _char* s_AnimNotifyPhaseItems[] =
    {
        "Immediatley",
        "Late",
        "PreRender"
    };

    // 현재 선택된 이펙트 데이터 가져오기
    auto& pEvent = m_tEventInfo->vecVFXEvents[m_tAnimControllInfo->iCurrentEffectEventIndex];

    if (ImGui::Button("Delete this Effect"))
    {
        m_tEventInfo->vecVFXEvents.erase(m_tEventInfo->vecVFXEvents.begin() + m_tAnimControllInfo->iCurrentEffectEventIndex);
        m_tAnimControllInfo->iCurrentEffectEventIndex = -1;
        m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
        return;
    }

    ImGui::PushID(&pEvent);

    if (ImGui::CollapsingHeader("Effect Basic Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat("Start Position", &pEvent.fStartTrackPosition, 0.1f, 0.f, (_float)m_tAnimControllInfo->fDuration))
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);

        auto& effectTags = m_pAnimToolManager->Get_LoadedEffectTags();

        ImGui::SeparatorText("EventNotifyPhase");
        {
            _int iAnimNotifyPhase = static_cast<_int>(pEvent.ePhase);
            if (ImGui::Combo("EventNotifyPhase##Combo", &iAnimNotifyPhase, s_AnimNotifyPhaseItems, static_cast<_int>(Engine::g_AnimNotifyPhaseTypeCount)))
                pEvent.ePhase = static_cast<Engine::EAnimNotifyPhase>(iAnimNotifyPhase);
        }

        if (ImGui::BeginCombo("Effect Tag", pEvent.strEffectTag.c_str()))
        {
            for (const auto& tag : effectTags)
            {
                bool is_selected = (pEvent.strEffectTag == tag);
                if (ImGui::Selectable(tag.c_str(), is_selected))
                {
                    pEvent.strEffectTag = tag;
                    m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::DragFloat("Duration", &pEvent.fDuration, 0.05f, 0.f, 100.f))
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
    }

    if (ImGui::CollapsingHeader("AnimNotifyId"))
    {

        vector<string> m_pShapeList = { "CollisionOn", "CollisionOff", "FootStepL"," FootStepR", "ONESHOT", "ATTACH_ON" };

        std::vector<const char*> iTems;
        iTems.reserve(static_cast<int>(m_pShapeList.size()));

        for (auto& str : m_pShapeList)
            iTems.push_back(str.c_str());

        int currentIdx = pEvent.iNotifyId;

        if (ImGui::ListBox("", &currentIdx, iTems.data(), static_cast<int>(m_pShapeList.size()), 6))
        {
            pEvent.iNotifyId = currentIdx;
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
        }

        if ((iTems.size() - 1) < currentIdx)
        {
            ImGui::TreePop();
            return;
        }
        ImGui::Spacing();
        ImGui::Text("Selected Shape: "); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", m_pShapeList[currentIdx].c_str());
        ImGui::Separator();
    }

    if (ImGui::CollapsingHeader("Transform & Attachment", ImGuiTreeNodeFlags_DefaultOpen))
    {
        int currentSim = pEvent.iSimulationType;
        const char* simItems[] = { "LOCAL (0)", "WORLD (1)",  };
        if (ImGui::Combo("Sim Type", &currentSim, simItems, IM_ARRAYSIZE(simItems)))
        {
            pEvent.iSimulationType = currentSim;
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
        }

        int CurrentBoneIndex = pEvent.iBoneIndex;
        if (ImGui::InputInt("Socket Name", &CurrentBoneIndex, 256))
        {
            pEvent.iBoneIndex = CurrentBoneIndex;
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
        }

        ImGui::Checkbox("Follow Bone", &pEvent.bFollowBone);

        if (ImGui::DragFloat3("Offset Position", (float*)&pEvent.vOffset, 0.01f))
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);

        if (ImGui::DragFloat3("Offset Rotation", (float*)&pEvent.vRotation, 0.01f))
            m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);

        if (ImGui::TreeNode("Use Bone Flag ##AnimationEffect"))
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Property Random Flags");

            // 2. 비트 플래그 개별 제어 (정우님이 만든 E_RANDOM_FLAG 연동)
            // 현재 플래그 상태 확인
            bool bBonePos = (pEvent.iBoneFlag & DTO::BONE_POS) != 0;
            bool bBoneScale = (pEvent.iBoneFlag & DTO::BONE_SCALE) != 0;
            bool bBoneRotataion = (pEvent.iBoneFlag & DTO::BONE_ROTATAION) != 0;

            // 위치 랜덤 체크박스
            if (ImGui::Checkbox("Use Bone Position", &bBonePos))
            {
                if (bBonePos) pEvent.iBoneFlag |= DTO::BONE_POS;
                else pEvent.iBoneFlag &= ~DTO::BONE_POS;
                m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
            }

            // 수명 랜덤 체크박스
            if (ImGui::Checkbox("Use Bone Scale", &bBoneScale))
            {
                if (bBoneScale) pEvent.iBoneFlag |= DTO::BONE_SCALE;
                else pEvent.iBoneFlag &= ~DTO::BONE_SCALE;
                m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
            }

            // 크기 랜덤 체크박스
            if (ImGui::Checkbox("Use Bone Rotation", &bBoneRotataion))
            {
                if (bBoneRotataion) pEvent.iBoneFlag |= DTO::BONE_ROTATAION;
                else pEvent.iBoneFlag &= ~DTO::BONE_ROTATAION;
                m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
            }

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Current Flag Value: %d", pEvent.iBoneFlag);

            ImGui::TreePop();
        }
    }

    ImGui::PopID();
}

CPanel_AnimDescription* CPanel_AnimDescription::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_AnimDescription* pInstance = new CPanel_AnimDescription(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_AnimDescription is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_AnimDescription::Free()
{
	Super::Free();
}
