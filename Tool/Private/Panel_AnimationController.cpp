#include "pch.h"
#include "Panel_AnimationController.h"
#include "AnimTool_Manager.h"

CPanel_AnimationController::CPanel_AnimationController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
    m_pAnimToolManager(CAnimTool_Manager::GetInstance()),
    m_tAnimControllInfo(&m_pAnimToolManager->Get_AnimControllInfo()),
    m_tEventInfo(&m_pAnimToolManager->Get_AnimEventInfo())
{
}

HRESULT CPanel_AnimationController::Initialize()
{
	return S_OK;
}

HRESULT CPanel_AnimationController::Render(CToolObject* pGo)
{
	AnimationListWindow();

	BoneListWindow();

	AnimationControllPanelWindow();

    ButtonsWindow();

	return S_OK;
}

void CPanel_AnimationController::Update(const _float fTimeDelta)
{
}

void CPanel_AnimationController::AnimationListWindow()
{
	ImGui::Begin("Animation list");

	if (!m_pAnimToolManager->ValidCheck())
	{
		ImGui::End();
		return;
	}

	ImGuiListClipper clipper;
	clipper.Begin((_int)m_tAnimControllInfo->vecAnimInfo.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            bool is_selected = (m_tAnimControllInfo->iCurrentAnimIndex == i);

            std::string label = std::to_string(i) + ": " + m_tAnimControllInfo->vecAnimInfo[i].strAnimName;

            if (ImGui::Selectable(label.c_str(), is_selected))
            {
                m_tAnimControllInfo->iCurrentAnimIndex = i;

                m_pAnimToolManager->ChangeAnimation(i);
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
    }
	ImGui::End();
}

void CPanel_AnimationController::BoneListWindow()
{
	ImGui::Begin("Bone list");

	if (!m_pAnimToolManager->ValidCheck())
	{
		ImGui::End();
		return;
	}

    ImGuiListClipper clipper;
    clipper.Begin((_int)m_tAnimControllInfo->vecBoneInfo.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            bool is_selected = (m_tAnimControllInfo->iCurrentBoneIndex == i);

            std::string label = std::to_string(i) + ": " + m_tAnimControllInfo->vecBoneInfo[i].strBoneName;

            if (ImGui::Selectable(label.c_str(), is_selected))
            {
                m_tAnimControllInfo->iCurrentBoneIndex = i;
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
    }

	ImGui::End();
}

void CPanel_AnimationController::AnimationControllPanelWindow()
{
	ImGui::Begin("Animation controller");

	if (!m_pAnimToolManager->ValidCheck())
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Animation Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Total Anims: %d", m_tAnimControllInfo->iTotalAnimCount);
		ImGui::Text("Current Index: %d", m_tAnimControllInfo->iCurrentAnimIndex);
		ImGui::Separator();
		ImGui::Text("Duration: %d ", m_tAnimControllInfo->fDuration);
		ImGui::Text("Speed: %.2f TPS", m_tAnimControllInfo->fTickPerSecond);
        //if (ImGui::DragFloat("Play Rate", &m_tAnimControllInfo->fPlayRate, 0.1f, 0.1f, 5.0f))
        //{
        //    if (m_tAnimControllInfo->fTickPerSecond <= 72.f && m_tAnimControllInfo->fTickPerSecond >= 0.01f)
        //    {
        //        if (m_pAnimToolManager->ValidCheck())
        //            m_tAnimControllInfo->pModel->Set_AnimationSpeed(m_tAnimControllInfo->fPlayRate);
        //    }
        //}

        if (ImGui::InputFloat("Play Rate", &m_fTimeScale, 0.01f, 1.0f, "%.2f"))
        {
            // 범위 클램프
            m_fTimeScale =
                std::clamp(m_fTimeScale, 0.1f, 5.0f);

            if (m_tAnimControllInfo->fTickPerSecond <= 72.f &&
                m_tAnimControllInfo->fTickPerSecond >= 0.01f)
            {
                if (m_pAnimToolManager->ValidCheck())
                {
                   m_tAnimControllInfo->pModel->Set_AnimationSpeed(
                        m_fTimeScale);

                    CGameInstance::GetInstance()->Set_GlobalScale(m_fTimeScale);
                }
            }
        }

		_float currentPosition = (_float)m_tAnimControllInfo->fTrackPosition;
        if (ImGui::SliderFloat("Trackposition Seek", &currentPosition, 0, (_float)m_tAnimControllInfo->fDuration))
        {
            m_tAnimControllInfo->fTrackPosition = (_uint)currentPosition;

            if (m_pAnimToolManager->ValidCheck())
            {
                m_tAnimControllInfo->pModel->Set_AnimTrackPosition((_float)m_tAnimControllInfo->fTrackPosition);
                if (!m_tAnimControllInfo->bPlay)
                    m_pAnimToolManager->Update_Animation(0.03f);
            }
        }

        if (ImGui::DragFloat("Transform Scale", &m_tAnimControllInfo->fTranformScale, 0.01f, 0.01f, 5.0f))
        {
            if (m_pAnimToolManager->ValidCheck())
                m_tAnimControllInfo->pCurrentObject->Get_Component<CTransform>()->Set_Scale(m_tAnimControllInfo->fTranformScale, m_tAnimControllInfo->fTranformScale, m_tAnimControllInfo->fTranformScale);
        }
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

    if (ImGui::Button("Add Event"))
        ImGui::OpenPopup("Add Event");

    Render_AddEventModal();

    ImGui::Separator();
    ImGui::Spacing();

    DrawController();

	ImGui::End();
}

void CPanel_AnimationController::ButtonsWindow()
{
    ImGui::Begin("Controll Buttons");

    if (m_tAnimControllInfo->bPlay)
    {
        if (ImGui::Button("|| Pause", ImVec2(60, 0)))
            m_tAnimControllInfo->bPlay = false;
    }
    else
    {
        if (ImGui::Button(">> Play", ImVec2(60, 0)))
            m_tAnimControllInfo->bPlay = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("# Stop", ImVec2(60, 0)))
    {
        m_tAnimControllInfo->bPlay = false;
        m_tAnimControllInfo->fTrackPosition = 0;

        if (m_pAnimToolManager->ValidCheck())
            m_tAnimControllInfo->pModel->Set_AnimTrackPosition((_float)m_tAnimControllInfo->fTrackPosition);
    }

    ImGui::SameLine();

    if (m_tAnimControllInfo->bLoop)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("Loop: ON", ImVec2(70, 0)))
        {
            m_tAnimControllInfo->bLoop = false;
            if (m_tAnimControllInfo->pModel) m_tAnimControllInfo->pModel->Set_LoopState(false);
        }
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Loop: OFF", ImVec2(70, 0)))
        {
            m_tAnimControllInfo->bLoop = true;
            if (m_tAnimControllInfo->pModel) m_tAnimControllInfo->pModel->Set_LoopState(true);
        }
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

//void CPanel_AnimationController::DrawController()
//{
//    // =========================================================
//    // 2. 타임라인 UI (Timeline Section)
//    // =========================================================
//    ImGui::Text("Timeline Editor");
//
//    float footerHeight = 0.0f;
//    ImGui::BeginChild("TimelineScroll", ImVec2(0, -footerHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//    ImDrawList* draw_list = ImGui::GetWindowDrawList();
//    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
//    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
//
//    // ---------------------------------------------------------
//    // A. 상수 및 비율 설정
//    // ---------------------------------------------------------
//    const float LABEL_WIDTH = 120.0f; // 라벨 공간 조금 확보 (이름+번호 표시 위해)
//    const float TRACK_HEIGHT = 30.0f;
//    const float RULER_HEIGHT = 20.0f;
//    const float TIMELINE_WIDTH_MIN = 500.0f;
//
//    float tick_per_second = (m_tAnimControllInfo->fTickPerSecond > 0.f) ? m_tAnimControllInfo->fTickPerSecond : 24.0f;
//    float timeline_area_width = max(canvas_size.x - LABEL_WIDTH, TIMELINE_WIDTH_MIN);
//    float total_duration_ticks = (m_tAnimControllInfo->fDuration > 0.0f) ? m_tAnimControllInfo->fDuration : 100.0f;
//    float px_per_tick = timeline_area_width / total_duration_ticks;
//
//    // ---------------------------------------------------------
//    // B. 입력 처리 (전역 스크러버)
//    // ---------------------------------------------------------
//    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + LABEL_WIDTH, canvas_pos.y));
//    ImGui::InvisibleButton("TimelineScrubber", ImVec2(timeline_area_width, canvas_size.y));
//
//    // 이벤트 박스 드래그 중이 아닐 때만 스크러버 작동
//    bool is_scrubbing_active = ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left);
//    if (is_scrubbing_active)
//    {
//        ImVec2 mouse_pos = ImGui::GetMousePos();
//        float local_x = mouse_pos.x - (canvas_pos.x + LABEL_WIDTH);
//        float new_tick = local_x / px_per_tick;
//
//        if (new_tick < 0.0f) new_tick = 0.0f;
//        if (new_tick > total_duration_ticks) new_tick = total_duration_ticks;
//
//        m_tAnimControllInfo->fTrackPosition = new_tick;
//
//        if (m_pAnimToolManager->ValidCheck())
//        {
//            m_tAnimControllInfo->pModel->Set_AnimTrackPosition(m_tAnimControllInfo->fTrackPosition);
//            if (!m_tAnimControllInfo->bPlay)
//                m_pAnimToolManager->Update_Animation(0.016f);
//        }
//    }
//
//    // ---------------------------------------------------------
//    // C. 렌더링 - 눈금자 (Ruler)
//    // ---------------------------------------------------------
//    float ruler_y_start = canvas_pos.y;
//    float ruler_y_end = ruler_y_start + RULER_HEIGHT;
//
//    // 배경
//    draw_list->AddRectFilled(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(60, 60, 60, 255));
//    draw_list->AddRect(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(100, 100, 100, 255));
//    draw_list->AddText(ImVec2(canvas_pos.x + 5, ruler_y_start + 2), IM_COL32(200, 200, 200, 255), "Timeline");
//
//    draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, ruler_y_end), IM_COL32(40, 40, 40, 255));
//
//    // 눈금
//    for (int t = 0; t <= (int)total_duration_ticks; t += 5)
//    {
//        float x = canvas_pos.x + LABEL_WIDTH + (t * px_per_tick);
//        float line_height = (t % 10 == 0) ? RULER_HEIGHT : RULER_HEIGHT * 0.5f;
//        draw_list->AddLine(ImVec2(x, ruler_y_end - line_height), ImVec2(x, ruler_y_end), IM_COL32(150, 150, 150, 255));
//        if (t % 10 == 0) {
//            char tmps[32]; sprintf_s(tmps, "%d", t);
//            draw_list->AddText(ImVec2(x + 2, ruler_y_start), IM_COL32(200, 200, 200, 255), tmps);
//        }
//    }
//
//    float current_y = ruler_y_end;
//
//    // ---------------------------------------------------------
//    // 2. 단일 이벤트 트랙 그리기 람다 (1 Event = 1 Track)
//    // ---------------------------------------------------------
//    // T: 이벤트 구조체 타입 (ATTACKEVENT 등)
//    // typeIndex: 0=Attack, 1=Effect (선택 로직 구분용)
//    auto DrawSingleEventTrack = [&](const char* label, auto& evt, int eventIndex, EAnimEvent::Enum typeIndex, ImU32 color, float durationSec)
//        {
//            ImGui::PushID(eventIndex + (typeIndex * 1000)); // ID 충돌 방지
//
//            // --- 2-1. 라벨 영역 (클릭 선택) ---
//            ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x, current_y));
//
//            // 해당 이벤트가 선택되었는지 확인
//            bool isSelected = false;
//            if (typeIndex == EAnimEvent::OVERLAP) isSelected = (m_tAnimControllInfo->iCurrentAttackEventIndex == eventIndex); // Attack
//            // else if (typeIndex == EAnimEvent::EFFECT) isSelected = (iSelectedEffectEvent == eventIndex); // Effect
//
//            if (ImGui::Selectable(label, isSelected, 0, ImVec2(LABEL_WIDTH, TRACK_HEIGHT)))
//            {
//                // 라벨 클릭 시 해당 이벤트 선택
//                if (typeIndex == EAnimEvent::OVERLAP) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
//                // else if (typeIndex == EAnimEvent::EFFECT) iSelectedEffectEvent = eventIndex;
//            }
//
//            // 라벨 테두리
//            draw_list->AddRect(ImVec2(canvas_pos.x, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT), IM_COL32(100, 100, 100, 255));
//
//            // --- 2-2. 트랙 배경 (줄무늬 효과 가능) ---
//            ImU32 bg_col = (eventIndex % 2 == 0) ? IM_COL32(50, 50, 50, 255) : IM_COL32(55, 55, 55, 255);
//            draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), bg_col);
//            draw_list->AddLine(ImVec2(canvas_pos.x, current_y + TRACK_HEIGHT), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), IM_COL32(30, 30, 30, 255));
//
//            // --- 2-3. 이벤트 박스 계산 및 그리기 ---
//            float start_tick = evt.fStartTrackPosition;
//            float duration_tick = durationSec * tick_per_second;
//
//            float start_x = canvas_pos.x + LABEL_WIDTH + (start_tick * px_per_tick);
//            float width_px = duration_tick * px_per_tick;
//            if (width_px < 6.0f) width_px = 6.0f;
//            float end_x = start_x + width_px;
//
//            ImVec2 rect_min(start_x, current_y + 4);
//            ImVec2 rect_max(end_x, current_y + TRACK_HEIGHT - 4);
//
//            // [입력 처리] 투명 버튼 (박스 위치)
//            ImGui::SetCursorScreenPos(rect_min);
//            if (ImGui::InvisibleButton("##EvtBtn", ImVec2(width_px, TRACK_HEIGHT - 8)))
//            {
//                if (typeIndex == EAnimEvent::OVERLAP) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
//                // else if (typeIndex == EAnimEvent::EFFECT) iSelectedEffectEvent = eventIndex;
//            }
//
//            // [드래그 처리]
//            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
//            {
//                float delta_tick = ImGui::GetIO().MouseDelta.x / px_per_tick;
//                evt.fStartTrackPosition += delta_tick;
//                if (evt.fStartTrackPosition < 0.0f) evt.fStartTrackPosition = 0.0f;
//
//                // 드래그 중 자동 선택
//                if (typeIndex == EAnimEvent::OVERLAP) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
//            }
//
//            // [박스 렌더링]
//            draw_list->AddRectFilled(rect_min, rect_max, color, 4.0f);
//
//            // 선택 하이라이트
//            if (isSelected)
//                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 0, 255), 4.0f, 0, 2.0f);
//            else
//                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 100), 4.0f);
//
//            // 박스 내부 텍스트
//            if (width_px > 20.0f) {
//                //draw_list->AddText(ImVec2(start_x + 2, current_y + 5), IM_COL32(255, 255, 255, 255), label);
//            }
//
//            ImGui::PopID();
//
//            // 다음 트랙을 위해 Y축 이동
//            current_y += TRACK_HEIGHT;
//        };
//
//    // ---------------------------------------------------------
//    // 3. 실제 데이터 순회 (Attack -> Effect 순서)
//    // ---------------------------------------------------------
//
//    // [Attack Events]
//    int localIdx = 0;
//    for (int i = 0; i < m_tEventInfo->vecAttackEvents.size(); ++i)
//    {
//        auto& evt = m_tEventInfo->vecAttackEvents[i];
//        if (evt.iAnimIndex != m_tAnimControllInfo->iCurrentAnimIndex) continue;
//
//        char labelBuf[32];
//        sprintf_s(labelBuf, "Hitbox %d", localIdx++); // 라벨: Hitbox 0, Hitbox 1...
//
//        DrawSingleEventTrack(
//            labelBuf,
//            evt,
//            i,      // 실제 벡터 인덱스
//            EAnimEvent::OVERLAP,      // 타입 인덱스 (0: Attack)
//            IM_COL32(200, 100, 100, 200),
//            evt.tHitboxDesc.fDuration
//        );
//    }
//
//    // [Effect Events]
//    /*
//    localIdx = 0;
//    for (int i = 0; i < m_tEventInfo->vecVFXEvents.size(); ++i)
//    {
//        auto& evt = m_tEventInfo->vecVFXEvents[i];
//        if (evt.iAnimIndex != m_tAnimControllInfo->iCurrentAnimIndex) continue;
//
//        char labelBuf[32];
//        sprintf_s(labelBuf, "Effect %d", localIdx++);
//
//        DrawSingleEventTrack(
//            labelBuf,
//            evt,
//            i,      // 실제 벡터 인덱스
//            EAnimEvent::EFFECT,      // 타입 인덱스 (1: Effect)
//            IM_COL32(100, 100, 200, 200),
//            evt.fDuration
//        );
//    }
//    */
//
//    // ---------------------------------------------------------
//    // D. 인디케이터 (현재 재생 위치) - 전체 높이만큼 그리기
//    // ---------------------------------------------------------
//    float cursor_x = canvas_pos.x + LABEL_WIDTH + (m_tAnimControllInfo->fTrackPosition * px_per_tick);
//
//    // 타임라인 끝부분(current_y)까지 선을 그림
//    draw_list->AddLine(ImVec2(cursor_x, ruler_y_start), ImVec2(cursor_x, current_y), IM_COL32(255, 200, 0, 255), 2.0f);
//    draw_list->AddTriangleFilled(ImVec2(cursor_x - 6, ruler_y_start), ImVec2(cursor_x + 6, ruler_y_start), ImVec2(cursor_x, ruler_y_start + 10), IM_COL32(255, 200, 0, 255));
//
//    ImGui::EndChild();
//}

void CPanel_AnimationController::DrawController()
{
    // =========================================================
    // 2. 타임라인 UI (Timeline Section)
    // =========================================================
    ImGui::Text("Timeline Editor");

    float footerHeight = 0.0f;
    ImGui::BeginChild("TimelineScroll", ImVec2(0, -footerHeight), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();

    // ---------------------------------------------------------
    // A. 상수 및 비율 설정
    // ---------------------------------------------------------
    const float LABEL_WIDTH = 120.0f;
    const float TRACK_HEIGHT = 30.0f;
    const float RULER_HEIGHT = 20.0f;
    const float TIMELINE_WIDTH_MIN = 500.0f;

    float tick_per_second = (m_tAnimControllInfo->fTickPerSecond > 0.f) ? m_tAnimControllInfo->fTickPerSecond : 24.0f;
    float timeline_area_width = max(canvas_size.x - LABEL_WIDTH, TIMELINE_WIDTH_MIN);
    float total_duration_ticks = (m_tAnimControllInfo->fDuration > 0.0f) ? m_tAnimControllInfo->fDuration : 100.0f;
    float px_per_tick = timeline_area_width / total_duration_ticks;

    // ---------------------------------------------------------
    // B. 입력 처리 (Scrubbing - 눈금자 영역만)
    // ---------------------------------------------------------
    // [수정] 눈금자 높이(RULER_HEIGHT)만큼만 버튼을 생성하여 클릭 감지
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + LABEL_WIDTH, canvas_pos.y));
    ImGui::InvisibleButton("RulerScrubber", ImVec2(timeline_area_width, RULER_HEIGHT));

    // 버튼이 활성화(클릭 유지) 상태일 때만 이동
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float local_x = mouse_pos.x - (canvas_pos.x + LABEL_WIDTH);
        float new_tick = local_x / px_per_tick;

        if (new_tick < 0.0f) new_tick = 0.0f;
        if (new_tick > total_duration_ticks) new_tick = total_duration_ticks;

        m_tAnimControllInfo->fTrackPosition = (_uint)new_tick;

        if (m_pAnimToolManager->ValidCheck())
        {
            m_tAnimControllInfo->pModel->Set_AnimTrackPosition((_float)m_tAnimControllInfo->fTrackPosition);
            if (!m_tAnimControllInfo->bPlay)
                m_pAnimToolManager->Update_Animation(0.016f);
        }
    }

    // ---------------------------------------------------------
    // C. 렌더링 - 눈금자 (Ruler)
    // ---------------------------------------------------------
    float ruler_y_start = canvas_pos.y;
    float ruler_y_end = ruler_y_start + RULER_HEIGHT;

    // 배경
    draw_list->AddRectFilled(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(60, 60, 60, 255));
    draw_list->AddRect(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(100, 100, 100, 255));
    draw_list->AddText(ImVec2(canvas_pos.x + 5, ruler_y_start + 2), IM_COL32(200, 200, 200, 255), "Timeline");

    draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, ruler_y_end), IM_COL32(40, 40, 40, 255));

    // 눈금
    for (int t = 0; t <= (int)total_duration_ticks; t += 5)
    {
        float x = canvas_pos.x + LABEL_WIDTH + (t * px_per_tick);
        float line_height = (t % 10 == 0) ? RULER_HEIGHT : RULER_HEIGHT * 0.5f;
        draw_list->AddLine(ImVec2(x, ruler_y_end - line_height), ImVec2(x, ruler_y_end), IM_COL32(150, 150, 150, 255));
        if (t % 10 == 0) {
            char tmps[32]; sprintf_s(tmps, "%d", t);
            draw_list->AddText(ImVec2(x + 2, ruler_y_start), IM_COL32(200, 200, 200, 255), tmps);
        }
    }

    float current_y = ruler_y_end;

    // ---------------------------------------------------------
    // 2. 단일 이벤트 트랙 그리기 람다 (1 Event = 1 Track)
    // ---------------------------------------------------------
    auto DrawSingleEventTrack = [&](const char* label, auto& evt, int eventIndex, EAnimEvent::Enum typeIndex, ImU32 color, float durationSec)
        {
            // ID 충돌 방지: 이벤트 인덱스와 타입 인덱스를 조합
            ImGui::PushID(eventIndex * 1000 + typeIndex);

            // --- 2-1. 라벨 영역 (클릭 선택) ---
            ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x, current_y));

            bool isSelected = false;
            if (typeIndex == EAnimEvent::OVERLAP) isSelected = (m_tAnimControllInfo->iCurrentAttackEventIndex == eventIndex);
            else if (typeIndex == EAnimEvent::EFFECT) isSelected = (m_tAnimControllInfo->iCurrentEffectEventIndex == eventIndex);

            if (ImGui::Selectable(label, isSelected, 0, ImVec2(LABEL_WIDTH, TRACK_HEIGHT)))
            {
                if (typeIndex == EAnimEvent::OVERLAP)
                    m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;

                else if (typeIndex == EAnimEvent::EFFECT) 
                    m_tAnimControllInfo->iCurrentEffectEventIndex = eventIndex;
            }

            draw_list->AddRect(ImVec2(canvas_pos.x, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT), IM_COL32(100, 100, 100, 255));

            // --- 2-2. 트랙 배경 ---
            ImU32 bg_col = (eventIndex % 2 == 0) ? IM_COL32(50, 50, 50, 255) : IM_COL32(55, 55, 55, 255);
            draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), bg_col);
            draw_list->AddLine(ImVec2(canvas_pos.x, current_y + TRACK_HEIGHT), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), IM_COL32(30, 30, 30, 255));

            // --- 2-3. 이벤트 박스 계산 ---
            float start_tick = evt.fStartTrackPosition;
            float duration_tick = durationSec * tick_per_second;

            float start_x = canvas_pos.x + LABEL_WIDTH + (start_tick * px_per_tick);
            float width_px = duration_tick * px_per_tick;
            if (width_px < 6.0f) width_px = 6.0f;
            float end_x = start_x + width_px;

            ImVec2 rect_min(start_x, current_y + 4);
            ImVec2 rect_max(end_x, current_y + TRACK_HEIGHT - 4);

            // [중요] 입력 처리를 위한 커서 이동 (박스 위치로)
            ImGui::SetCursorScreenPos(rect_min);

            // 버튼 ID 생성 ("##EvtBtn"으로 하면 PushID 덕분에 고유해짐)
            // [드래그 및 클릭 처리]
            // InvisibleButton은 클릭/드래그 모두 감지 가능
            if (ImGui::InvisibleButton("##EvtBtn", ImVec2(width_px, TRACK_HEIGHT - 8)))
            {
                // 단순 클릭 시 선택 처리
                if (typeIndex == EAnimEvent::OVERLAP) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
                 else if (typeIndex == EAnimEvent::EFFECT) m_tAnimControllInfo->iCurrentEffectEventIndex = eventIndex;
            }

            // 버튼이 활성화된 상태에서 드래그 중인지 확인
            if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                // 마우스 델타값(픽셀) 가져오기
                float delta_px = ImGui::GetIO().MouseDelta.x;

                // 픽셀 -> 틱 변환
                float delta_tick = delta_px / px_per_tick;

                // 데이터 갱신
                evt.fStartTrackPosition += delta_tick;

                // 범위 제한 (0 이하 방지)
                if (evt.fStartTrackPosition < 0.0f) evt.fStartTrackPosition = 0.0f;

                // 드래그 중일 때도 선택된 것으로 간주
                if (typeIndex == EAnimEvent::OVERLAP) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
                else if (typeIndex == EAnimEvent::EFFECT) m_tAnimControllInfo->iCurrentAttackEventIndex = eventIndex;
            }

            // [박스 렌더링] (입력 처리 후에 그려도 됨, DrawList 순서 주의)
            draw_list->AddRectFilled(rect_min, rect_max, color, 4.0f);

            if (isSelected)
                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 0, 255), 4.0f, 0, 2.0f);
            else
                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 100), 4.0f);

            // 텍스트 (박스가 충분히 클 때만)
            if (width_px > 30.0f) {
                // draw_list->AddText(ImVec2(start_x + 2, current_y + 5), IM_COL32(255, 255, 255, 255), label);
            }

            ImGui::PopID(); // PushID에 대한 팝

            current_y += TRACK_HEIGHT;
        };

    // ---------------------------------------------------------
    // 3. 실제 데이터 순회 (Attack)
    // ---------------------------------------------------------
    int localIdx = 0;
    for (int i = 0; i < m_tEventInfo->vecAttackEvents.size(); ++i)
    {
        auto& evt = m_tEventInfo->vecAttackEvents[i];
        if (evt.iAnimIndex != m_tAnimControllInfo->iCurrentAnimIndex) continue;

        char labelBuf[32];
        sprintf_s(labelBuf, "Hitbox %d", localIdx++);

        DrawSingleEventTrack(
            labelBuf,
            evt,
            i,
            EAnimEvent::OVERLAP,
            IM_COL32(200, 100, 100, 200),
            evt.tHitboxDesc.fDuration
        );
    }

    // [Effect Events]
   
    localIdx = 0;
    for (int i = 0; i < m_tEventInfo->vecVFXEvents.size(); ++i)
    {
        auto& evt = m_tEventInfo->vecVFXEvents[i];
        if (evt.iAnimIndex != m_tAnimControllInfo->iCurrentAnimIndex) continue;

        char labelBuf[32];
        sprintf_s(labelBuf, "Effect %d", localIdx++);

        DrawSingleEventTrack(
            labelBuf,
            evt,
            i,                      // 실제 벡터 인덱스
            EAnimEvent::EFFECT,     // 타입 인덱스 (VFX 구분)
            IM_COL32(100, 150, 250, 200), // 이펙트는 파란색 계열
            evt.fDuration           // DTO::EFFECTEVENT에 정의된 유지 시간
        );
    }
    

    // ---------------------------------------------------------
    // D. 인디케이터 (현재 재생 위치)
    // ---------------------------------------------------------
    float cursor_x = canvas_pos.x + LABEL_WIDTH + (m_tAnimControllInfo->fTrackPosition * px_per_tick);

    // 타임라인 끝까지 선 그리기
    draw_list->AddLine(ImVec2(cursor_x, ruler_y_start), ImVec2(cursor_x, current_y), IM_COL32(255, 200, 0, 255), 2.0f);
    draw_list->AddTriangleFilled(ImVec2(cursor_x - 6, ruler_y_start), ImVec2(cursor_x + 6, ruler_y_start), ImVec2(cursor_x, ruler_y_start + 10), IM_COL32(255, 200, 0, 255));

    ImGui::EndChild();
}

void CPanel_AnimationController::Render_AddEventModal()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Add Event", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        int currentType = (int)m_eEventTypeCombo;
        if (ImGui::Combo("Shape Type", &currentType, eventTypeItems, IM_ARRAYSIZE(eventTypeItems)))
        {
            m_eEventTypeCombo = (EAnimEvent::Enum)currentType;
        }

        ImGui::Separator();
        ImGui::Spacing();

        _float buttonsWidth = 50.f;

        if (ImGui::Button("OK", ImVec2(50, 0)))
        {
            switch (m_eEventTypeCombo)
            {
            case Engine::EAnimEvent::OVERLAP:
            {
                m_tAnimControllInfo->iCurrentAttackEventIndex = (_int)m_tEventInfo->vecAttackEvents.size();
                DTO::ATTACKEVENT newEvent{};
                newEvent.iAnimIndex = m_tAnimControllInfo->iCurrentAnimIndex;
                m_tEventInfo->vecAttackEvents.push_back(newEvent);
                m_pAnimToolManager->Modify_AttackOverlap(m_tEventInfo->vecAttackEvents);
                break;
            }
            case Engine::EAnimEvent::EFFECT:
            {
                m_tAnimControllInfo->iCurrentEffectEventIndex = (_int)m_tEventInfo->vecVFXEvents.size();

                // 새로운 EffectEvent 객체 생성 및 기본값 새팅
                DTO::EFFECTEVENT newEvent{};
                newEvent.eEventType = Engine::EAnimEvent::EFFECT;
                newEvent.iAnimIndex = m_tAnimControllInfo->iCurrentAnimIndex;
                newEvent.fStartTrackPosition = (_float)m_tAnimControllInfo->fTrackPosition; // 현재 타임라인 위치에 생성
                newEvent.fDuration = 1.0f; // 기본 지속 시간 1초
                newEvent.strEffectTag = "None"; // 초기 태그

                // 벡터에 추가
                m_tEventInfo->vecVFXEvents.push_back(newEvent);

                // 매니저를 통해 모듈 및 컴포넌트 데이터 갱신 (실시간 프리뷰용)
                m_pAnimToolManager->Modify_EffectEvent(m_tEventInfo->vecVFXEvents);
                break;
            }
            case Engine::EAnimEvent::SOUND:
                break;
            default:
                break;
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        _float windowWidth = ImGui::GetWindowContentRegionMax().x;

        ImGui::SetCursorPosX(windowWidth - buttonsWidth);

        if (ImGui::Button("Cancel", ImVec2(buttonsWidth, 0)))
        {
            ImGui::CloseCurrentPopup(); // 그냥 닫기
        }

        ImGui::EndPopup();
    }
}

void CPanel_AnimationController::SetAnimationObject()
{
    m_tAnimControllInfo = &m_pAnimToolManager->Get_AnimControllInfo();
    m_tEventInfo = &m_pAnimToolManager->Get_AnimEventInfo();
}

BONEINFO CPanel_AnimationController::GetBoneInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return BONEINFO();

	return m_tAnimControllInfo->vecBoneInfo[index];
}

ANIMINFO CPanel_AnimationController::GetAnimInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return ANIMINFO();

	return m_tAnimControllInfo->vecAnimInfo[index];
}

CPanel_AnimationController* CPanel_AnimationController::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_AnimationController* pInstance = new CPanel_AnimationController(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_AnimationController is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_AnimationController::Free()
{
	Super::Free();
}
