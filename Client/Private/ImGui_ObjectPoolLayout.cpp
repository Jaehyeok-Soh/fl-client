#include "pch.h"
#include "ImGui_ObjectPoolLayout.h"
#include "GameInstance.h"

CImGui_ObjectPoolLayout::CImGui_ObjectPoolLayout()
    :Super("ObjectPool")
{
}

HRESULT CImGui_ObjectPoolLayout::Initialize()
{
    return S_OK;
}

void CImGui_ObjectPoolLayout::DrawYAxisTicks_3Multiple(const ImVec2& pMin, const ImVec2& pMax, _int iCapacity)
{
    if (iCapacity <= 0) return;

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float h = pMax.y - pMin.y;

    _int step = 3;
    while (iCapacity / step > 10) step += 3; // 너무 촘촘하면 step 키움(3의 배수 유지)

    for (_int v = 0; v <= iCapacity; v += step)
    {
        const float t = (_float)v / (_float)iCapacity;
        const float y = pMax.y - t * h;

        char buf[32]{};
        sprintf_s(buf, "%d", v);

        dl->AddLine(ImVec2(pMin.x, y), ImVec2(pMin.x + 6.f, y), IM_COL32(160, 160, 160, 255));
        dl->AddText(ImVec2(pMin.x - 34.f, y - 6.f), IM_COL32(200, 200, 200, 255), buf);
    }

    // iCapacity 표시(맨 위)
    {
        char buf[32]{};
        sprintf_s(buf, "%d", iCapacity);
        dl->AddText(ImVec2(pMin.x - 34.f, pMin.y - 10.f), IM_COL32(220, 220, 220, 255), buf);
    }
}

void CImGui_ObjectPoolLayout::Render(CGameObject* pGo)
{
#ifdef _DEBUG
    if (ImGui::CollapsingHeader(m_strLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen) == false)
        return;

    if (m_pGameInstance == nullptr)
    {
        ImGui::TextDisabled("GameInstance: null");
        return;
    }

    struct Track
    {
        std::vector<float> samples;
        int   write = 0;
        float acc = 0.f;
        float sampleInterval = 0.10f; // 10Hz
        bool  pause = false;

        void Reset()
        {
            samples.assign(300, 0.f);
            write = 0;
            acc = 0.f;
        }

        void Push(float dt, float v)
        {
            if (pause) return;
            acc += dt;
            if (acc < sampleInterval) return;
            acc = 0.f;

            if (samples.empty())
                Reset();

            samples[write] = v;
            write = (write + 1) % (int)samples.size();
        }
    };

    struct Selected
    {
        bool used = false;
        std::wstring tag;
        Track track;
        ImU32 color = 0;
        int lastActive = 0;
        int lastCap = 0;
    };

    static Selected s_sel[5];
    static ImU32 s_colors[5] =
    {
        IM_COL32(255,  64,  64, 255), // Red
        IM_COL32(255, 220,  64, 255), // Yellow
        IM_COL32(64, 128, 255, 255), // Blue
        IM_COL32(255,  64, 255, 255), // Magenta
        IM_COL32(64, 255, 128, 255), // Green
    };

    static std::vector<std::wstring> s_tags;
    static int  s_level = 0;
    static int  s_tagSel = -1;
    static bool s_pauseAll = false;

    static bool s_init = false;
    if (!s_init)
    {
        for (int i = 0; i < 5; ++i)
        {
            s_sel[i].color = s_colors[i];
            s_sel[i].track.Reset();
        }
        s_init = true;
    }

    const float dt = ImGui::GetIO().DeltaTime;
    const int levelCount = (int)ELevelType::END;

    auto RefreshTags = [&]()
        {
            s_tags.clear();
            m_pGameInstance->Collect_PoolTags((_uint)s_level, s_tags);
            if (s_tagSel >= (int)s_tags.size()) s_tagSel = -1;
        };

    auto FindFreeSlot = [&]() -> int
        {
            for (int i = 0; i < 5; ++i) if (!s_sel[i].used) return i;
            return -1;
        };

    auto IsAlreadySelected = [&](const std::wstring& tag) -> bool
        {
            for (int i = 0; i < 5; ++i)
                if (s_sel[i].used && s_sel[i].tag == tag)
                    return true;
            return false;
        };

    auto AddSelected = [&](const std::wstring& tag)
        {
            if (IsAlreadySelected(tag))
                return;

            const int slot = FindFreeSlot();
            if (slot < 0)
                return;

            s_sel[slot].used = true;
            s_sel[slot].tag = tag;
            s_sel[slot].track.Reset();
            s_sel[slot].lastActive = 0;
            s_sel[slot].lastCap = 0;
        };

    auto RemoveSelected = [&](int slot)
        {
            if (slot < 0 || slot >= 5) return;
            s_sel[slot].used = false;
            s_sel[slot].tag.clear();
            s_sel[slot].track.Reset();
            s_sel[slot].lastActive = 0;
            s_sel[slot].lastCap = 0;
        };

    auto ComputeYMax5 = [&]() -> int
        {
            int maxVal = 0;
            for (int i = 0; i < 5; ++i)
            {
                if (!s_sel[i].used) continue;
                maxVal = (std::max)(maxVal, s_sel[i].lastCap);
                maxVal = (std::max)(maxVal, s_sel[i].lastActive);
            }
            maxVal = (std::max)(maxVal, 5);
            // 5의 배수로 올림
            const int r = maxVal % 5;
            if (r != 0) maxVal += (5 - r);
            return maxVal;
        };

    auto DrawMultiGraph = [&](const char* id, float height)
        {
            const ImVec2 canvas(ImGui::GetContentRegionAvail().x, height);
            const ImVec2 p0 = ImGui::GetCursorScreenPos();

            ImGui::InvisibleButton(id, canvas);
            ImDrawList* dl = ImGui::GetWindowDrawList();

            const ImVec2 p1(p0.x + canvas.x, p0.y + canvas.y);
            dl->AddRectFilled(p0, p1, IM_COL32(20, 20, 20, 255));
            dl->AddRect(p0, p1, IM_COL32(80, 80, 80, 255));

            const int yMax = ComputeYMax5();
            if (yMax <= 0) return;

            // y축 눈금(5의 배수) - 너무 많으면 간격 키움(5의 배수 유지)
            int step = 5;
            const int tickCount = yMax / 5;
            if (tickCount > 15)
                step = ((tickCount / 15) * 5);
            if (step < 5) step = 5;

            const float w = canvas.x;
            const float h = canvas.y;

            // grid + label (left)
            for (int v = 0; v <= yMax; v += step)
            {
                const float t = (float)v / (float)yMax;
                const float y = p1.y - t * h;
                dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), IM_COL32(40, 40, 40, 255));

                char buf[32]{};
                sprintf_s(buf, "%d", v);
                dl->AddText(ImVec2(p0.x - 28.f, y - 7.f), IM_COL32(200, 200, 200, 255), buf);
            }

            // series
            for (int si = 0; si < 5; ++si)
            {
                if (!s_sel[si].used) continue;
                auto& tr = s_sel[si].track;
                if (tr.samples.size() < 2) continue;

                const int N = (int)tr.samples.size();
                const float dx = (N > 1) ? (w / (float)(N - 1)) : w;

                // ring buffer를 “시간 순서”로 그리기: write가 다음에 쓸 위치(가장 최신은 write-1)
                int idx0 = tr.write; // 가장 오래된 샘플 시작
                ImVec2 prev;
                bool hasPrev = false;

                for (int i = 0; i < N; ++i)
                {
                    const int idx = (idx0 + i) % N;
                    const float v = (std::max)(0.f, tr.samples[idx]);
                    const float ny = (float)v / (float)yMax;

                    const float x = p0.x + dx * (float)i;
                    const float y = p0.y + (1.f - ny) * h;

                    ImVec2 cur(x, y);
                    if (hasPrev)
                        dl->AddLine(prev, cur, s_sel[si].color, 2.0f);

                    prev = cur;
                    hasPrev = true;
                }
            }
        };

    // --- UI ---
    ImGui::BeginChild("##ObjectPoolScroll", ImVec2(0.f, 0.f), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Level combo
    ImGui::SetNextItemWidth(200.f);
    if (ImGui::BeginCombo("Level", LevelTypeToString(s_level)))
    {
        for (int i = 0; i < levelCount; ++i)
        {
            const bool sel = (i == s_level);
            if (ImGui::Selectable(LevelTypeToString(i), sel))
            {
                s_level = i;
                s_tagSel = -1;
                RefreshTags();
                // 선택 레벨 바뀌면 기존 선택 유지해도 되지만, 원하면 아래 한 줄로 초기화 가능
                // for (int k=0;k<5;++k) RemoveSelected(k);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh Tags"))
        RefreshTags();

    ImGui::SameLine();
    ImGui::Checkbox("Pause", &s_pauseAll);

    // Tag combo (선택 즉시 체크리스트에 추가)
    if (s_tags.empty())
        RefreshTags();

    const char* preview = "<select tag>";
    std::string previewStr;
    if (s_tagSel >= 0 && s_tagSel < (int)s_tags.size())
    {
        previewStr = Engine_Utils::ToString(s_tags[s_tagSel]);
        preview = previewStr.c_str();
    }

    ImGui::SetNextItemWidth(280.f);
    if (ImGui::BeginCombo("PoolTag", preview))
    {
        for (int i = 0; i < (int)s_tags.size(); ++i)
        {
            std::string name = Engine_Utils::ToString(s_tags[i]);
            if (ImGui::Selectable(name.c_str(), false))
            {
                s_tagSel = i;
                AddSelected(s_tags[i]);
            }
        }
        ImGui::EndCombo();
    }

    // Checklist (최대 5개)
    ImGui::Separator();
    ImGui::TextUnformatted("Tracking (max 5):");

    for (int i = 0; i < 5; ++i)
    {
        if (!s_sel[i].used) continue;

        ImGui::PushID(i);
        ImGui::ColorButton("##c", ImGui::ColorConvertU32ToFloat4(s_sel[i].color), ImGuiColorEditFlags_NoTooltip, ImVec2(10.f, 10.f));
        ImGui::SameLine();

        std::string tagStr = Engine_Utils::ToString(s_sel[i].tag);
        ImGui::TextUnformatted(tagStr.c_str());
        ImGui::SameLine();

        if (ImGui::SmallButton("X"))
            RemoveSelected(i);

        ImGui::PopID();
    }

    // 샘플링 (선택된 5개만)
    for (int i = 0; i < 5; ++i)
    {
        if (!s_sel[i].used) continue;

        s_sel[i].track.pause = s_pauseAll;

        const int cap = m_pGameInstance->Get_ObjectPoolCapacity((_uint)s_level, s_sel[i].tag);
        const int act = m_pGameInstance->Get_ObjectPoolActiveCount((_uint)s_level, s_sel[i].tag);

        s_sel[i].lastCap = (cap >= 0 ? cap : 0);
        s_sel[i].lastActive = (act >= 0 ? act : 0);

        s_sel[i].track.Push(dt, (float)s_sel[i].lastActive);
    }

    // 그래프 영역: 패널 남은 공간을 꽉 채움
    ImGui::Separator();
    ImVec2 avail = ImGui::GetContentRegionAvail();
    const float graphH = (avail.y > 120.f) ? avail.y : 120.f;
    DrawMultiGraph("##PoolGraph", graphH);

    // 하단 요약(선택된 것만)
    ImGui::Separator();
    for (int i = 0; i < 5; ++i)
    {
        if (!s_sel[i].used) continue;
        std::string tagStr = Engine_Utils::ToString(s_sel[i].tag);

        ImGui::PushID(i + 100);
        ImGui::ColorButton("##c2", ImGui::ColorConvertU32ToFloat4(s_sel[i].color), ImGuiColorEditFlags_NoTooltip, ImVec2(10.f, 10.f));
        ImGui::SameLine();
        ImGui::Text("%s  Active:%d  Cap:%d", tagStr.c_str(), s_sel[i].lastActive, s_sel[i].lastCap);
        ImGui::PopID();
    }

    ImGui::EndChild();
#endif
}

CImGui_ObjectPoolLayout* CImGui_ObjectPoolLayout::Create()
{
    CImGui_ObjectPoolLayout* pInstance = new CImGui_ObjectPoolLayout();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CImGui_ObjectPoolLayout::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;;
}

void CImGui_ObjectPoolLayout::Free()
{
    Super::Free();
}
