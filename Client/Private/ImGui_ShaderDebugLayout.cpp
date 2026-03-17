#include "pch.h"
#include "ImGui_ShaderDebugLayout.h"
#include "GameInstance.h"

CImGui_ShaderDebugLayout::CImGui_ShaderDebugLayout()
	:Super("ShaderDebug")
{
}

HRESULT CImGui_ShaderDebugLayout::Initialize()
{
	return S_OK;
}

void CImGui_ShaderDebugLayout::Render(CGameObject* pGo)
{
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("Shader Debug", ImGuiTreeNodeFlags_DefaultOpen) == false)
		return;

	ImGui::BeginChild("##ShaderDebugScroll", ImVec2(0.f, 0.f), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	constexpr _int iCols{ 2 };
	const ImGuiStyle& style = ImGui::GetStyle();
	const _float fAspect = 16.0f / 9.0f;

	struct ShaderDebugitem
	{
		const _char* name;
		Engine::ERenderTarget eTarget{ Engine::ERenderTarget::END };
	};

	static constexpr ShaderDebugitem items[] =
	{
		{"Diffuse", ERenderTarget::Diffuse},
		{"Normal", ERenderTarget::Normal},
		{"Shade", ERenderTarget::Shade},
		{"SpecularMask", ERenderTarget::SpecularMask},
		{"Specular", ERenderTarget::Specular},
        {"Emissive", ERenderTarget::Emissive},
		{"Depth", ERenderTarget::Depth},
		{"SSAO_Full", ERenderTarget::SSAO_Full},
		{"SceneHDR", ERenderTarget::SceneHDR},
		{"Bloom_Ping", ERenderTarget::Bloom_Ping},
        {"Cascade_0", ERenderTarget::Cascade_0},
        {"Cascade_1", ERenderTarget::Cascade_1},
        {"BakedShadow", ERenderTarget::Shadow_Baked},
        {"OIT_Accum", ERenderTarget::OIT_Accum},
        {"OIT_Reveal", ERenderTarget::OIT_Reveal},
	};

    ImGuiTableFlags flags =
        ImGuiTableFlags_SizingStretchSame |
        ImGuiTableFlags_NoSavedSettings |
        ImGuiTableFlags_PadOuterX;

    if (ImGui::BeginTable("##RTGrid", iCols, flags))
    {
        ImGui::TableSetupColumn("##col0", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##col1", ImGuiTableColumnFlags_WidthStretch);

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.f, 8.f));

        _int idx = 0;
        for (const ShaderDebugitem& it : items)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(idx++);

            const _float fCellW = ImGui::GetContentRegionAvail().x;
            const _float fTitleH = ImGui::GetTextLineHeightWithSpacing();
            const _float fImgW = fCellW;
            const _float fImgH = fImgW / fAspect;
            const _float fCellH = fTitleH + fImgH + style.FramePadding.y * 2.f;            

            ImGui::BeginChild("##RTCell", ImVec2(fCellW, fCellH), true, ImGuiWindowFlags_NoScrollbar);
            ImGui::TextUnformatted(it.name);

            ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RenderTargetSRV(it.eTarget);

            if (pSRV == nullptr)
            {
                ImGui::Spacing();
                ImGui::TextDisabled("SRV: null");
            }
            else
            {
                ImTextureID TextureId = (ImTextureID)pSRV;

                ImGui::Image(TextureId, ImVec2(fImgW, fImgH), ImVec2(0, 0), ImVec2(1, 1));

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(it.name);
                    ImGui::Image(TextureId, ImVec2(fImgW * 2.f, fImgH * 2.f), ImVec2(0, 0), ImVec2(1, 1));
                    ImGui::EndTooltip();
                }
            }

            ImGui::EndChild();
            ImGui::PopID();
        }

        ImGui::PopStyleVar();
        ImGui::EndTable();
    }

    // -----------------------
    // Baked Shadow Debug
    // -----------------------
    if (ImGui::CollapsingHeader("Baked Shadow", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("BakedShadow");

        static int iSlice = 0;
        static int iPrevSlice = -1;

        constexpr int iMaxSlice = BAKED_SECTION_COUNT_X * BAKED_SECTION_COUNT_Z - 1;

        // Slice 선택
        ImGui::SliderInt("Slice", &iSlice, 0, iMaxSlice);

        // 선택 slice가 바뀌면 디버그용 2D 텍스처 갱신
        if (iPrevSlice != iSlice)
        {
            m_pGameInstance->Update_BakedShadowDebugTexture(iSlice);
            iPrevSlice = iSlice;
        }

        // 그리드 좌표 표시
        const int iX = iSlice % BAKED_SECTION_COUNT_X;
        const int iZ = iSlice / BAKED_SECTION_COUNT_X;
        ImGui::Text("Grid: [%d, %d]", iX, iZ);

        // 디버그용 2D SRV 출력
        ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_BakedShadowDebugSRV();
        if (pSRV)
        {
            ImGui::Image((ImTextureID)pSRV, ImVec2(256.f, 256.f));

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Slice %d", iSlice);
                ImGui::Text("Grid [%d, %d]", iX, iZ);
                ImGui::Separator();
                ImGui::Image((ImTextureID)pSRV, ImVec2(512.f, 512.f));
                ImGui::EndTooltip();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Debug slice preview not ready");
        }

        // 활성 섹션 정보
        ImGui::SeparatorText("Active Sections");
        auto& active = m_pGameInstance->Get_ActiveBakedSectionSet();
        ImGui::Text("Count: %d", active.iCount);

        bool bSelectedIsActive = false;

        for (_uint i = 0; i < active.iCount; ++i)
        {
            const auto& s = active.sections[i];

            if (s.iArraySlice == iSlice)
                bSelectedIsActive = true;

            ImGui::Text("  [%d] grid(%d,%d) slice=%d",
                i, s.iSectionX, s.iSectionZ, s.iArraySlice);
        }

        ImGui::Text("Selected Slice Active: %s", bSelectedIsActive ? "YES" : "NO");

        ImGui::PopID();
    }

	ImGui::EndChild();
#endif
}

CImGui_ShaderDebugLayout* CImGui_ShaderDebugLayout::Create()
{
	CImGui_ShaderDebugLayout* pInstance = new CImGui_ShaderDebugLayout();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_ShaderDebugLayout::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;;
}

void CImGui_ShaderDebugLayout::Free()
{
	Super::Free();
}
