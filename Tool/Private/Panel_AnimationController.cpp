#include "pch.h"
#include "Panel_AnimationController.h"
#include "AnimTool_Manager.h"

CPanel_AnimationController::CPanel_AnimationController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
    m_pAnimToolManager(CAnimTool_Manager::GetInstance()),
    m_tAnimControllInfo(m_pAnimToolManager->Get_AnimControllInfo()),
    m_tEventInfo(m_pAnimToolManager->Get_AnimEventInfo())
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
	clipper.Begin(m_tAnimControllInfo.vecAnimInfo.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            bool is_selected = (m_tAnimControllInfo.iCurrentAnimIndex == i);

            std::string label = std::to_string(i) + ": " + m_tAnimControllInfo.vecAnimInfo[i].strAnimName;

            if (ImGui::Selectable(label.c_str(), is_selected))
            {
                m_tAnimControllInfo.iCurrentAnimIndex = i;

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
    clipper.Begin(m_tAnimControllInfo.vecBoneInfo.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            bool is_selected = (m_tAnimControllInfo.iCurrentBoneIndex == i);

            std::string label = std::to_string(i) + ": " + m_tAnimControllInfo.vecBoneInfo[i].strBoneName;

            if (ImGui::Selectable(label.c_str(), is_selected))
            {
                m_tAnimControllInfo.iCurrentBoneIndex = i;
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
		ImGui::Text("Total Anims: %d", m_tAnimControllInfo.iTotalAnimCount);
		ImGui::Text("Current Index: %d", m_tAnimControllInfo.iCurrentAnimIndex);
		ImGui::Separator();
		ImGui::Text("Duration: %d ", m_tAnimControllInfo.fDuration);
		ImGui::Text("Speed: %.2f TPS", m_tAnimControllInfo.fTickPerSecond);
        if (ImGui::DragFloat("Play Rate", &m_tAnimControllInfo.fPlayRate, 0.1f, 0.1f, 5.0f))
        {
            if (m_tAnimControllInfo.fTickPerSecond <= 72.f && m_tAnimControllInfo.fTickPerSecond >= 0.01f)
            {
                if (m_pAnimToolManager->ValidCheck())
                    m_tAnimControllInfo.pModel->Set_AnimationPlayRate(m_tAnimControllInfo.iCurrentAnimIndex, m_tAnimControllInfo.fPlayRate);
            }
        }

		_float currentPosition = (int)m_tAnimControllInfo.fTrackPosition;
        if (ImGui::SliderFloat("Trackposition Seek", &currentPosition, 0, m_tAnimControllInfo.fDuration))
        {
            m_tAnimControllInfo.fTrackPosition = currentPosition;

            if (m_pAnimToolManager->ValidCheck())
            {
                m_tAnimControllInfo.pModel->Set_AnimTrackPosition(m_tAnimControllInfo.fTrackPosition);
                if (!m_tAnimControllInfo.bPlay)
                    m_pAnimToolManager->Update_Animation(0.03f);
            }
        }

        if (ImGui::DragFloat("Transform Scale", &m_tAnimControllInfo.fTranformScale, 0.01f, 0.01f, 5.0f))
        {
            if (m_pAnimToolManager->ValidCheck())
                m_tAnimControllInfo.pCurrentObject->Get_Component<CTransform>()->Set_Scale(m_tAnimControllInfo.fTranformScale, m_tAnimControllInfo.fTranformScale, m_tAnimControllInfo.fTranformScale);
        }
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

    DrawController();

	ImGui::End();
}

void CPanel_AnimationController::ButtonsWindow()
{
    ImGui::Begin("Controll Buttons");

    if (m_tAnimControllInfo.bPlay)
    {
        if (ImGui::Button("|| Pause", ImVec2(60, 0)))
            m_tAnimControllInfo.bPlay = false;
    }
    else
    {
        if (ImGui::Button(">> Play", ImVec2(60, 0)))
            m_tAnimControllInfo.bPlay = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("# Stop", ImVec2(60, 0)))
    {
        m_tAnimControllInfo.bPlay = false;
        m_tAnimControllInfo.fTrackPosition = 0.0f;

        if (m_pAnimToolManager->ValidCheck())
            m_tAnimControllInfo.pModel->Set_AnimTrackPosition(m_tAnimControllInfo.fTrackPosition);
    }

    ImGui::SameLine();

    if (m_tAnimControllInfo.bLoop)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
        if (ImGui::Button("Loop: ON", ImVec2(70, 0)))
        {
            m_tAnimControllInfo.bLoop = false;
            if (m_tAnimControllInfo.pModel) m_tAnimControllInfo.pModel->Set_LoopState(false);
        }
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Button("Loop: OFF", ImVec2(70, 0)))
        {
            m_tAnimControllInfo.bLoop = true;
            if (m_tAnimControllInfo.pModel) m_tAnimControllInfo.pModel->Set_LoopState(true);
        }
        ImGui::PopStyleColor();
    }

    ImGui::End();
}

#pragma region Legacy_DrawController
//void CPanel_AnimationController::DrawController()
//{
//    // =========================================================
//    // 2. 타임라인 UI (Timeline Section)
//    // =========================================================
//    ImGui::Text("Timeline Editor");
//
//    // 레이아웃 설정
//    float footerHeight = 0.0f; // 하단 여백 필요시 설정
//    ImGui::BeginChild("TimelineScroll", ImVec2(0, -footerHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
//
//    ImDrawList* draw_list = ImGui::GetWindowDrawList();
//    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // 캔버스 시작 좌표 (절대 좌표)
//    ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // 가용 영역 크기
//
//    // ---------------------------------------------------------
//    // A. 상수 설정
//    // ---------------------------------------------------------
//    const float LABEL_WIDTH = 150.0f;       // 좌측 이벤트 이름 영역 너비
//    const float TRACK_HEIGHT = 30.0f;       // 트랙 한 줄의 높이
//    const float RULER_HEIGHT = 20.0f;       // 상단 눈금자 높이
//    const float TIMELINE_WIDTH_MIN = 500.0f;// 타임라인 최소 너비
//
//    // 타임라인 실제 너비 (창 크기에 맞추거나 더 넓게)
//    float timeline_area_width = max(canvas_size.x - LABEL_WIDTH, TIMELINE_WIDTH_MIN);
//
//    // [핵심] 픽셀 <-> 프레임 변환 비율
//    // PixelPerFrame = (전체 픽셀 너비) / (전체 듀레이션)
//    float px_per_frame = timeline_area_width / (float)m_tAnimControllInfo.fDuration;
//
//    // ---------------------------------------------------------
//    // B. 입력 처리 (Scrubbing / Dragging)
//    // ---------------------------------------------------------
//    // 타임라인 전체 영역에 투명 버튼을 깔아서 클릭/드래그 감지
//    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + LABEL_WIDTH, canvas_pos.y));
//    ImGui::InvisibleButton("TimelineScrubber", ImVec2(timeline_area_width, canvas_size.y));
//
//    bool is_hovered = ImGui::IsItemHovered();
//    bool is_active = ImGui::IsItemActive(); // 클릭 중인지 확인
//
//    // 클릭 또는 드래그 중이면 트랙 포지션 업데이트
//    if (is_active && ImGui::IsMouseDown(ImGuiMouseButton_Left))
//    {
//        ImVec2 mouse_pos = ImGui::GetMousePos();
//        float local_x = mouse_pos.x - (canvas_pos.x + LABEL_WIDTH);
//
//        // 픽셀 -> 프레임 변환
//        float new_frame = local_x / px_per_frame;
//
//        // 클램핑 (0 ~ Duration)
//        if (new_frame < 0.0f) new_frame = 0.0f;
//        if (new_frame > m_tAnimControllInfo.fDuration) new_frame = (float)m_tAnimControllInfo.fDuration;
//
//        m_tAnimControllInfo.fTrackPosition = (_uint)new_frame;
//
//        if (m_pAnimToolManager->ValidCheck())
//        {
//            m_tAnimControllInfo.pModel->Set_AnimTrackPosition(m_tAnimControllInfo.fTrackPosition);
//            if (!m_tAnimControllInfo.bPlay)
//                m_pAnimToolManager->Update_Animation(0.03f);
//        }
//    }
//
//    // ---------------------------------------------------------
//    // C. 렌더링 (Custom Drawing)
//    // ---------------------------------------------------------
//
//    // 1. 눈금자 (Ruler) 그리기
//    ImU32 col_text = IM_COL32(200, 200, 200, 255);
//    ImU32 col_line = IM_COL32(100, 100, 100, 255);
//
//    float ruler_y_start = canvas_pos.y;
//    float ruler_y_end = ruler_y_start + RULER_HEIGHT;
//
//    // 눈금자 배경
//    draw_list->AddRectFilled(
//        ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_start),
//        ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, ruler_y_end),
//        IM_COL32(40, 40, 40, 255)
//    );
//
//    // 10프레임 단위로 눈금 표시
//    for (int f = 0; f <= m_tAnimControllInfo.fDuration; f += 5)
//    {
//        float x = canvas_pos.x + LABEL_WIDTH + (f * px_per_frame);
//
//        // 눈금 선
//        draw_list->AddLine(ImVec2(x, ruler_y_start + 10), ImVec2(x, ruler_y_end), col_line);
//
//        // 숫자 텍스트 (10프레임마다)
//        if (f % 10 == 0) {
//            char tmps[32];
//            sprintf_s(tmps, "%d", f);
//            draw_list->AddText(ImVec2(x + 2, ruler_y_start), col_text, tmps);
//        }
//    }
//
//    // 2. 트랙 및 이벤트 그리기
//    float current_y = ruler_y_end;
//
//    vector<DTO::ATTACKEVENT*> vecFoundEvents;
//
//    for (auto& evt : m_tEventInfo.vecAttackEvents)
//    {
//        if (evt.iAnimIndex == m_tAnimControllInfo.iCurrentAnimIndex)
//            vecFoundEvents.push_back(&evt);
//    }
//
//    if (!vecFoundEvents.empty())
//    {
//        for (size_t i = 0; i < vecFoundEvents.size(); ++i)
//        {
//            // 2-1. 좌측 라벨 (이벤트 이름)
//            ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x, current_y));
//            //ImGui::Text("%s", eventTracks[i].label.c_str());
//            ImGui::Text("%s", "attack");
//
//            // 라벨 영역 구분선
//            draw_list->AddLine(
//                ImVec2(canvas_pos.x + LABEL_WIDTH - 1, current_y),
//                ImVec2(canvas_pos.x + LABEL_WIDTH - 1, current_y + TRACK_HEIGHT),
//                col_line
//            );
//
//            // 2-2. 트랙 배경 (줄무늬 효과)
//            ImU32 track_bg_col = (i % 2 == 0) ? IM_COL32(60, 60, 60, 255) : IM_COL32(50, 50, 50, 255);
//            draw_list->AddRectFilled(
//                ImVec2(canvas_pos.x + LABEL_WIDTH, current_y),
//                ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT),
//                track_bg_col
//            );
//
//            // 2-3. 이벤트 박스 (Duration Bar) 그리기
//            for (size_t j = 0; j < vecFoundEvents.size(); j++)
//            {
//                float start_x = canvas_pos.x + LABEL_WIDTH + (vecFoundEvents[j]->fStartTrackPosition * px_per_frame);
//                float end_x = start_x + (vecFoundEvents[j]->tHitboxDesc.fDuration * px_per_frame);
//
//                // 박스 크기
//                ImVec2 rect_min(start_x, current_y + 2);
//                ImVec2 rect_max(end_x, current_y + TRACK_HEIGHT - 2);
//
//                // 박스 색상 (선택 여부에 따라 다르게)
//                ImU32 rect_col = iSelectedAttackEvent == j ? IM_COL32(100, 200, 100, 255) : IM_COL32(100, 150, 200, 200);
//
//                // 박스 렌더링
//                draw_list->AddRectFilled(rect_min, rect_max, rect_col, 4.0f); // 4.0f는 둥근 모서리
//                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 150), 4.0f); // 테두리
//
//                // 박스 안에 텍스트
//                draw_list->AddText(ImVec2(start_x + 5, current_y + 5), IM_COL32(255, 255, 255, 255), "attack");
//
//                // (선택사항) 이벤트 클릭 감지 로직 추가 가능
//                ImGui::SetCursorScreenPos(rect_min);
//                if (ImGui::InvisibleButton(vecFoundEvents[j]->strDescription.c_str(), ImVec2(end_x - start_x, TRACK_HEIGHT - 4))) {
//                    iSelectedAttackEvent = j;
//                }
//            }
//
//            current_y += TRACK_HEIGHT;
//        }
//    }
//
//    // ---------------------------------------------------------
//    // D. 현재 프레임 커서 (빨간 세로선) 그리기
//    // ---------------------------------------------------------
//    // 트랙 포지션 -> X 좌표 변환
//    float cursor_x = canvas_pos.x + LABEL_WIDTH + (m_tAnimControllInfo.fTrackPosition * px_per_frame);
//
//    // 빨간 선 (위에서 아래까지 관통)
//    draw_list->AddLine(
//        ImVec2(cursor_x, ruler_y_start),
//        ImVec2(cursor_x, current_y),
//        IM_COL32(255, 50, 50, 255),
//        2.0f // 선 두께
//    );
//
//    // 커서 헤드 (삼각형 모양 - DS Anim Studio 스타일)
//    draw_list->AddTriangleFilled(
//        ImVec2(cursor_x - 6, ruler_y_start),
//        ImVec2(cursor_x + 6, ruler_y_start),
//        ImVec2(cursor_x, ruler_y_start + 10),
//        IM_COL32(255, 50, 50, 255)
//    );
//
//    // 현재 프레임 텍스트 표시 (커서 옆에)
//    if (is_active || is_hovered) {
//        char frame_text[16];
//        sprintf_s(frame_text, "%.1f", m_tAnimControllInfo.fTrackPosition);
//        draw_list->AddText(ImVec2(cursor_x + 8, ruler_y_start), IM_COL32(255, 100, 100, 255), frame_text);
//    }
//
//    ImGui::EndChild(); // End TimelineScroll
//}
#pragma endregion

#pragma region Legacy_DrawController_2
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
//    const float LABEL_WIDTH = 100.0f;
//    const float TRACK_HEIGHT = 30.0f;
//    const float RULER_HEIGHT = 20.0f;
//    const float TIMELINE_WIDTH_MIN = 500.0f;
//
//    // 타임라인 실제 너비
//    float timeline_area_width = max(canvas_size.x - LABEL_WIDTH, TIMELINE_WIDTH_MIN);
//
//    // [핵심] 픽셀 <-> 프레임 비율
//    // Duration이 0이면 나눗셈 방지
//    float total_duration = (m_tAnimControllInfo.fDuration > 0.0f) ? m_tAnimControllInfo.fDuration : 100.0f;
//    float px_per_frame = timeline_area_width / total_duration;
//
//    // ---------------------------------------------------------
//    // B. 입력 처리 (Scrubbing)
//    // ---------------------------------------------------------
//    // 타임라인 영역(라벨 제외)에 투명 버튼 배치
//    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + LABEL_WIDTH, canvas_pos.y));
//    ImGui::InvisibleButton("TimelineScrubber", ImVec2(timeline_area_width, canvas_size.y));
//
//    bool is_hovered = ImGui::IsItemHovered();
//    bool is_active = ImGui::IsItemActive();
//
//    if (is_active && ImGui::IsMouseDown(ImGuiMouseButton_Left))
//    {
//        ImVec2 mouse_pos = ImGui::GetMousePos();
//        float local_x = mouse_pos.x - (canvas_pos.x + LABEL_WIDTH);
//        float new_frame = local_x / px_per_frame;
//
//        // 클램핑
//        if (new_frame < 0.0f) new_frame = 0.0f;
//        if (new_frame > total_duration) new_frame = total_duration;
//
//        m_tAnimControllInfo.fTrackPosition = new_frame;
//
//        if (m_pAnimToolManager->ValidCheck())
//        {
//            m_tAnimControllInfo.pModel->Set_AnimTrackPosition(m_tAnimControllInfo.fTrackPosition);
//            // 일시정지 상태에서도 프리뷰 갱신
//            if (!m_tAnimControllInfo.bPlay)
//                m_pAnimToolManager->Update_Animation(0.016f); // 약 60프레임 델타값
//        }
//    }
//
//    // ---------------------------------------------------------
//    // C. 렌더링 (Custom Drawing)
//    // ---------------------------------------------------------
//
//    // 1. 눈금자 (Ruler) 배경
//    float ruler_y_start = canvas_pos.y;
//    float ruler_y_end = ruler_y_start + RULER_HEIGHT;
//
//    // (라벨 영역 배경)
//    draw_list->AddRectFilled(
//        ImVec2(canvas_pos.x, ruler_y_start),
//        ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end),
//        IM_COL32(60, 60, 60, 255)
//    );
//    draw_list->AddRect( // 라벨 영역 테두리
//        ImVec2(canvas_pos.x, ruler_y_start),
//        ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end),
//        IM_COL32(100, 100, 100, 255)
//    );
//    draw_list->AddText(ImVec2(canvas_pos.x + 5, ruler_y_start + 2), IM_COL32(200, 200, 200, 255), "Frame");
//
//    // (타임라인 영역 배경)
//    draw_list->AddRectFilled(
//        ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_start),
//        ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, ruler_y_end),
//        IM_COL32(40, 40, 40, 255)
//    );
//
//    // 눈금 그리기 (5프레임 단위)
//    for (int f = 0; f <= (int)total_duration; f += 5)
//    {
//        float x = canvas_pos.x + LABEL_WIDTH + (f * px_per_frame);
//
//        // 눈금 높이 조절 (10단위는 길게, 5단위는 짧게)
//        float line_height = (f % 10 == 0) ? RULER_HEIGHT : RULER_HEIGHT * 0.5f;
//
//        draw_list->AddLine(
//            ImVec2(x, ruler_y_end - line_height),
//            ImVec2(x, ruler_y_end),
//            IM_COL32(150, 150, 150, 255)
//        );
//
//        if (f % 10 == 0) {
//            char tmps[32];
//            sprintf_s(tmps, "%d", f);
//            draw_list->AddText(ImVec2(x + 2, ruler_y_start), IM_COL32(200, 200, 200, 255), tmps);
//        }
//    }
//
//    float current_y = ruler_y_end;
//
//    // ---------------------------------------------------------
//    // 2. 트랙 그리기 (Track Rendering)
//    // ---------------------------------------------------------
//
//    // 람다 함수: 트랙 그리기 중복 제거
//    // T: 이벤트 타입 (ATTACKEVENT, EFFECTEVENT 등)
//    // getDurationFunc: 각 구조체마다 Duration 가져오는 법이 다를 수 있으므로 함수로 받음
//    auto DrawTrack = [&](const char* label, auto& eventList, ImU32 color, auto getDurationFunc)
//        {
//            // 2-1. 트랙 배경 및 라벨
//            // 라벨 영역
//            draw_list->AddRectFilled(
//                ImVec2(canvas_pos.x, current_y),
//                ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT),
//                IM_COL32(70, 70, 70, 255)
//            );
//            draw_list->AddRect( // 구분선
//                ImVec2(canvas_pos.x, current_y),
//                ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT),
//                IM_COL32(100, 100, 100, 255)
//            );
//            draw_list->AddText(ImVec2(canvas_pos.x + 5, current_y + 5), IM_COL32(255, 255, 255, 255), label);
//
//            // 타임라인 영역 배경 (홀짝 색상 구분 없이 통일하거나 변경 가능)
//            draw_list->AddRectFilled(
//                ImVec2(canvas_pos.x + LABEL_WIDTH, current_y),
//                ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT),
//                IM_COL32(50, 50, 50, 255)
//            );
//            draw_list->AddLine( // 아래 구분선
//                ImVec2(canvas_pos.x, current_y + TRACK_HEIGHT),
//                ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT),
//                IM_COL32(30, 30, 30, 255)
//            );
//
//            // 2-2. 이벤트 박스 그리기
//            for (int i = 0; i < eventList.size(); ++i)
//            {
//                auto& evt = eventList[i];
//
//                // 현재 선택된 애니메이션의 이벤트만 그리기
//                if (evt.iAnimIndex != m_tAnimControllInfo.iCurrentAnimIndex)
//                    continue;
//
//                float duration = getDurationFunc(evt);
//
//                // [중요] Duration이 너무 짧아도 최소 2픽셀은 보이게 처리
//                float width_px = duration * px_per_frame;
//                if (width_px < 4.0f) width_px = 4.0f;
//
//                float start_x = canvas_pos.x + LABEL_WIDTH + (evt.fStartTrackPosition * px_per_frame);
//                float end_x = start_x + width_px;
//
//                ImVec2 rect_min(start_x, current_y + 4);
//                ImVec2 rect_max(end_x, current_y + TRACK_HEIGHT - 4);
//
//                // 선택된 이벤트 하이라이트 (인덱스 로직은 상황에 맞춰 수정 필요)
//                // 여기서는 단순히 색상만 적용
//                draw_list->AddRectFilled(rect_min, rect_max, color, 4.0f);
//                draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 150), 4.0f);
//
//                // 이벤트 이름 (공간 있으면)
//                if (width_px > 30.0f) {
//                    // evt.strDescription이 있으면 그것 사용, 없으면 Label
//                    draw_list->AddText(ImVec2(start_x + 2, current_y + 5), IM_COL32(255, 255, 255, 255), label);
//                }
//            }
//            current_y += TRACK_HEIGHT;
//        };
//
//    // ---------------------------------------------------------
//    // 3. 실제 데이터 연결 (Attack & Effect)
//    // ---------------------------------------------------------
//
//    // [Attack Track]
//    // HitboxDesc 안에 Duration이 있다고 가정
//    DrawTrack("Attack", m_tEventInfo.vecAttackEvents, IM_COL32(200, 100, 100, 200),
//        [](auto& e) { return e.tHitboxDesc.fDuration; });
//
//    //// [Effect Track]
//    //// EffectEvent 안에 fDuration이 직접 있다고 가정 (변수명 확인 필요!)
//    //// 만약 Effect도 Desc 구조체 안에 있다면 e.tEffectDesc.fDuration 처럼 수정하세요.
//    //DrawTrack("Effect", m_tEventInfo.vecVFXEvents, IM_COL32(100, 100, 200, 200),
//    //    [](auto& e) { return e.fDuration; /* <-- 변수명 확인 필요 */ });
//
//
//    // ---------------------------------------------------------
//    // D. 현재 프레임 커서 (Indicator)
//    // ---------------------------------------------------------
//    float cursor_x = canvas_pos.x + LABEL_WIDTH + (m_tAnimControllInfo.fTrackPosition * px_per_frame);
//
//    // 전체 높이 계산 (눈금자 ~ 마지막 트랙까지)
//    float total_height = current_y - canvas_pos.y;
//
//    draw_list->AddLine(
//        ImVec2(cursor_x, ruler_y_start),
//        ImVec2(cursor_x, current_y),
//        IM_COL32(255, 200, 0, 255),
//        2.0f
//    );
//
//    draw_list->AddTriangleFilled(
//        ImVec2(cursor_x - 6, ruler_y_start),
//        ImVec2(cursor_x + 6, ruler_y_start),
//        ImVec2(cursor_x, ruler_y_start + 10),
//        IM_COL32(255, 200, 0, 255)
//    );
//
//    // 드래그 중일 때 시간 텍스트 표시
//    if (is_active || is_hovered) {
//        char frame_text[16];
//        sprintf_s(frame_text, "%.2f", m_tAnimControllInfo.fTrackPosition);
//        draw_list->AddText(ImVec2(cursor_x + 8, ruler_y_start), IM_COL32(255, 200, 0, 255), frame_text);
//    }
//
//    ImGui::EndChild();
//}
#pragma endregion

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
    // A. 상수 및 비율 설정 (Tick 기준)
    // ---------------------------------------------------------
    const float LABEL_WIDTH = 100.0f;
    const float TRACK_HEIGHT = 30.0f;
    const float RULER_HEIGHT = 20.0f;
    const float TIMELINE_WIDTH_MIN = 500.0f;

    // [중요] 엔진의 TickRate (Duration 변환용)
    float tick_per_second = (m_tAnimControllInfo.fTickPerSecond > 0.f) ? m_tAnimControllInfo.fTickPerSecond : 24.0f;

    // 타임라인 실제 너비
    float timeline_area_width = max(canvas_size.x - LABEL_WIDTH, TIMELINE_WIDTH_MIN);

    // [핵심 1] 전체 길이(Duration)는 Tick 단위임
    float total_duration_ticks = (m_tAnimControllInfo.fDuration > 0.0f) ? m_tAnimControllInfo.fDuration : 100.0f;

    // [핵심 2] 1 Tick 당 픽셀
    float px_per_tick = timeline_area_width / total_duration_ticks;

    // ---------------------------------------------------------
    // B. 입력 처리 (Scrubbing)
    // ---------------------------------------------------------
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + LABEL_WIDTH, canvas_pos.y));
    ImGui::InvisibleButton("TimelineScrubber", ImVec2(timeline_area_width, canvas_size.y));

    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float local_x = mouse_pos.x - (canvas_pos.x + LABEL_WIDTH);
        float new_tick = local_x / px_per_tick;

        if (new_tick < 0.0f) new_tick = 0.0f;
        if (new_tick > total_duration_ticks) new_tick = total_duration_ticks;

        m_tAnimControllInfo.fTrackPosition = new_tick;

        if (m_pAnimToolManager->ValidCheck())
        {
            m_tAnimControllInfo.pModel->Set_AnimTrackPosition(m_tAnimControllInfo.fTrackPosition);
            if (!m_tAnimControllInfo.bPlay)
                m_pAnimToolManager->Update_Animation(0.016f);
        }
    }

    // ---------------------------------------------------------
    // C. 렌더링
    // ---------------------------------------------------------

    // 1. 눈금자 (Ruler) - Tick 단위
    float ruler_y_start = canvas_pos.y;
    float ruler_y_end = ruler_y_start + RULER_HEIGHT;

    // 라벨 영역 배경 (어둡게)
    draw_list->AddRectFilled(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(60, 60, 60, 255));
    draw_list->AddRect(ImVec2(canvas_pos.x, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_end), IM_COL32(100, 100, 100, 255));
    draw_list->AddText(ImVec2(canvas_pos.x + 5, ruler_y_start + 2), IM_COL32(200, 200, 200, 255), "Tick");

    // 타임라인 영역 배경
    draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, ruler_y_start), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, ruler_y_end), IM_COL32(40, 40, 40, 255));

    // 눈금 그리기 (5 Tick 단위)
    for (int t = 0; t <= (int)total_duration_ticks; t += 5)
    {
        float x = canvas_pos.x + LABEL_WIDTH + (t * px_per_tick);
        float line_height = (t % 10 == 0) ? RULER_HEIGHT : RULER_HEIGHT * 0.5f;

        draw_list->AddLine(ImVec2(x, ruler_y_end - line_height), ImVec2(x, ruler_y_end), IM_COL32(150, 150, 150, 255));

        if (t % 10 == 0) {
            char tmps[32];
            sprintf_s(tmps, "%d", t);
            draw_list->AddText(ImVec2(x + 2, ruler_y_start), IM_COL32(200, 200, 200, 255), tmps);
        }
    }

    float current_y = ruler_y_end;

    // ---------------------------------------------------------
    // 2. 트랙 그리기 함수
    // ---------------------------------------------------------
    auto DrawTrack = [&](const char* label, auto& eventList, ImU32 color, auto getDurationFunc)
        {
            // 2-1. 라벨 영역 (왼쪽)
            draw_list->AddRectFilled(ImVec2(canvas_pos.x, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT), IM_COL32(70, 70, 70, 255));
            draw_list->AddRect(ImVec2(canvas_pos.x, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH, current_y + TRACK_HEIGHT), IM_COL32(100, 100, 100, 255));
            draw_list->AddText(ImVec2(canvas_pos.x + 5, current_y + 5), IM_COL32(255, 255, 255, 255), label);

            // 2-2. 트랙 영역 (오른쪽)
            draw_list->AddRectFilled(ImVec2(canvas_pos.x + LABEL_WIDTH, current_y), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), IM_COL32(50, 50, 50, 255));
            draw_list->AddLine(ImVec2(canvas_pos.x, current_y + TRACK_HEIGHT), ImVec2(canvas_pos.x + LABEL_WIDTH + timeline_area_width, current_y + TRACK_HEIGHT), IM_COL32(30, 30, 30, 255));

            // 2-3. 이벤트 박스 그리기
            for (int i = 0; i < eventList.size(); ++i)
            {
                auto& evt = eventList[i];
                if (evt.iAnimIndex != m_tAnimControllInfo.iCurrentAnimIndex) continue;

                // [핵심] Start는 Tick 그대로, Duration은 초 -> Tick 변환
                float start_tick = evt.fStartTrackPosition;  // 이미 Tick 단위 (1.92)
                float duration_tick = getDurationFunc(evt) * tick_per_second; // 초(0.74) * 24 = 17.76 Tick

                // 픽셀 계산
                float start_x = canvas_pos.x + LABEL_WIDTH + (start_tick * px_per_tick);
                float width_px = duration_tick * px_per_tick;

                if (width_px < 4.0f) width_px = 4.0f; // 최소 크기 보장

                float end_x = start_x + width_px;

                ImVec2 rect_min(start_x, current_y + 4);
                ImVec2 rect_max(end_x, current_y + TRACK_HEIGHT - 4);

                // 박스 그리기
                draw_list->AddRectFilled(rect_min, rect_max, color, 4.0f);

                // 선택된 하이라이트 (라벨 이름으로 타입 구분 예시)
                bool isSelected = false;
                if (strcmp(label, "Attack") == 0 && iSelectedAttackEvent == i) isSelected = true;
                // if (strcmp(label, "Effect") == 0 && iSelectedEffectEvent == i) isSelected = true; // 이펙트 선택 변수 필요 시

                if (isSelected)
                    draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 0, 255), 4.0f, 0, 2.0f);
                else
                    draw_list->AddRect(rect_min, rect_max, IM_COL32(255, 255, 255, 100), 4.0f);

                // 텍스트 (공간 넉넉할 때만)
                if (width_px > 30.0f) {
                    //char buf[32];
                    //sprintf_s(buf, "%.1f", start_tick);
                    draw_list->AddText(ImVec2(start_x + 2, current_y + 5), IM_COL32(255, 255, 255, 255), label);
                }
            }
            current_y += TRACK_HEIGHT;
        };

    // ---------------------------------------------------------
    // 3. 실제 트랙 배치
    // ---------------------------------------------------------

    // [Attack] HitboxDesc.fDuration (초 단위) 사용
    DrawTrack("Attack", m_tEventInfo.vecAttackEvents, IM_COL32(200, 100, 100, 200),
        [](auto& e) { return e.tHitboxDesc.fDuration; });

    //// [Effect] Effect 구조체 안에 fDuration (초 단위) 있다고 가정
    //// m_vecVFXEvents 추가됨
    //DrawTrack("Effect", m_tEventInfo.vecVFXEvents, IM_COL32(100, 100, 200, 200),
    //    [](auto& e) { return e.fDuration; });


    // ---------------------------------------------------------
    // D. 인디케이터 (현재 재생 위치)
    // ---------------------------------------------------------
    float cursor_x = canvas_pos.x + LABEL_WIDTH + (m_tAnimControllInfo.fTrackPosition * px_per_tick);

    draw_list->AddLine(ImVec2(cursor_x, ruler_y_start), ImVec2(cursor_x, current_y), IM_COL32(255, 200, 0, 255), 2.0f);
    draw_list->AddTriangleFilled(ImVec2(cursor_x - 6, ruler_y_start), ImVec2(cursor_x + 6, ruler_y_start), ImVec2(cursor_x, ruler_y_start + 10), IM_COL32(255, 200, 0, 255));

    ImGui::EndChild();
}

void CPanel_AnimationController::SetAnimationObject()
{
    m_tAnimControllInfo = m_pAnimToolManager->Get_AnimControllInfo();
    m_tEventInfo = m_pAnimToolManager->Get_AnimEventInfo();
}

BONEINFO CPanel_AnimationController::GetBoneInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return BONEINFO();

	return m_tAnimControllInfo.vecBoneInfo[index];
}

ANIMINFO CPanel_AnimationController::GetAnimInfo(_uint index)
{
	if (!m_pAnimToolManager->ValidCheck())
		return ANIMINFO();

	return m_tAnimControllInfo.vecAnimInfo[index];
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
