#include "pch.h"
#include "ImGui_ShaderLayout.h"
#include "GameInstance.h"

CImGui_ShaderLayout::CImGui_ShaderLayout()
	: Super("ShaderInfo")
{
}

void CImGui_ShaderLayout::Render(CGameObject* pGo)
{
#ifdef _DEBUG
    if (m_pGameInstance == nullptr)
    {
        ImGui::TextDisabled("Render_Manager not found.");
        return;
    }

    if (!m_bDefaultCached)
    {
        m_defSSAO = m_pGameInstance->Get_SSAOParamDesc();
        m_defHDR = m_pGameInstance->Get_HDRParamDesc();
        m_defBloom = m_pGameInstance->Get_BloomParamDesc();
        m_defOutline = m_pGameInstance->Get_OutlineParamDesc();
        m_bDefaultCached = true;
    }

    ImGui::BeginGroup();
    ImGui::SeparatorText("PostProcess / Shader Params");

    ImGui::Checkbox("Auto Apply", &m_bAutoApply);
    ImGui::SameLine();
    if (ImGui::SmallButton("Apply All"))
        m_pGameInstance->Commit_AllPostParams();

    ImGui::SameLine();
    if (ImGui::SmallButton("Reset All"))
    {
        m_pGameInstance->Get_SSAOParamDesc() = m_defSSAO;
        m_pGameInstance->Get_HDRParamDesc() = m_defHDR;
        m_pGameInstance->Get_BloomParamDesc() = m_defBloom;
        m_pGameInstance->Get_OutlineParamDesc() = m_defOutline;
        m_pGameInstance->Commit_AllPostParams();
    }

    ImGui::Separator();

    // -----------------------
    // SSAO
    // -----------------------
    if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("SSAO");
        auto& ssao = m_pGameInstance->Get_SSAOParamDesc();

        _bool bChanged = false;
        bChanged |= ImGui::SliderFloat("Radius", &ssao.fRadius, 0.1f, 5.0f);
        bChanged |= ImGui::SliderFloat("Bias", &ssao.fBias, 0.0f, 0.2f);
        bChanged |= ImGui::SliderFloat("Intensity", &ssao.fIntensity, 0.0f, 5.0f);
        bChanged |= ImGui::SliderFloat("Power", &ssao.fPower, 0.1f, 4.0f);
        bChanged |= ImGui::SliderFloat("FadeStart", &ssao.fFadeStart, 0.0f, 500.f);
        bChanged |= ImGui::SliderFloat("FadeEnd", &ssao.fFadeEnd, 0.0f, 500.f);

        ImGui::TextDisabled("InvSize is auto from half viewport");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##SSAO"))
                m_pGameInstance->Commit_SSAOParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_SSAOParam();
        }

        if (ImGui::SmallButton("Reset##SSAO"))
        {
            ssao = m_defSSAO;
            m_pGameInstance->Commit_SSAOParam();
        }
        ImGui::PopID();
    }

    // -----------------------
    // HDR
    // -----------------------
    if (ImGui::CollapsingHeader("HDR", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("HDR");
        auto& hdr = m_pGameInstance->Get_HDRParamDesc();

        _bool bChanged = false;
        bChanged |= ImGui::SliderFloat("Exposure", &hdr.fExposure, 0.01f, 5.0f);
        bChanged |= ImGui::SliderFloat("Gamma", &hdr.fGamma, 0.8f, 3.0f);

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##HDR"))
                m_pGameInstance->Commit_HDRParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_HDRParam();
        }

        if (ImGui::SmallButton("Reset##HDR"))
        {
            hdr = m_defHDR;
            m_pGameInstance->Commit_HDRParam();
        }
        ImGui::PopID();
    }

    // -----------------------
    // Bloom
    // -----------------------
    if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Bloom");
        auto& bloom = m_pGameInstance->Get_BloomParamDesc();

        _bool bChanged = false;
        bChanged |= ImGui::SliderFloat("Threshold", &bloom.fThreshold, 0.0f, 5.0f);
        bChanged |= ImGui::SliderFloat("Knee", &bloom.fKnee, 0.0f, 1.0f);
        bChanged |= ImGui::SliderFloat("Intensity", &bloom.fIntensity, 0.0f, 20.f);

        ImGui::TextDisabled("InvSize is auto from half viewport");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##Bloom"))
                m_pGameInstance->Commit_BloomParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_BloomParam();
        }

        if (ImGui::SmallButton("Reset##Bloom"))
        {
            bloom = m_defBloom;
            m_pGameInstance->Commit_BloomParam();
        }
        ImGui::PopID();
    }

    // -----------------------
    // Outline
    // -----------------------
    if (ImGui::CollapsingHeader("Outline", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Outline");
        auto& o = m_pGameInstance->Get_OutlineParamDesc();

        _bool bChanged = false;

        // ColorEdit는 float[4]가 편하니 임시 배열로 연결
        float col[4] = { o.vColor.x, o.vColor.y, o.vColor.z, o.vColor.w };
        if (ImGui::ColorEdit4("Color", col))
        {
            o.vColor = { col[0], col[1], col[2], col[3] };
            bChanged = true;
        }

        bChanged |= ImGui::SliderFloat("ThicknessPx", &o.fThicknessPx, 0.5f, 10.f);
        bChanged |= ImGui::SliderFloat("Opacity", &o.fOpacity, 0.0f, 1.0f);
        bChanged |= ImGui::SliderFloat("NormalThreshold", &o.fNormalThreshold, 0.0f, 5.0f);
        bChanged |= ImGui::SliderFloat("DepthThreshold", &o.fDepthThreshold, 0.0f, 0.2f);
        bChanged |= ImGui::SliderFloat("NormalStrength", &o.fNormalStrength, 0.0f, 10.f);
        bChanged |= ImGui::SliderFloat("DepthStrength", &o.fDepthStrength, 0.0f, 50.f);
        bChanged |= ImGui::SliderFloat("FadeStart", &o.fFadeStart, 0.0f, 500.f);
        bChanged |= ImGui::SliderFloat("FadeEnd", &o.fFadeEnd, 0.0f, 500.f);

        ImGui::TextDisabled("InvSize is auto from default viewport");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##Outline"))
                m_pGameInstance->Commit_OutlineParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_OutlineParam();
        }

        if (ImGui::SmallButton("Reset##Outline"))
        {
            o = m_defOutline;
            m_pGameInstance->Commit_OutlineParam();
        }
        ImGui::PopID();
    }

    ImGui::EndGroup();
#endif
}

CImGui_ShaderLayout* CImGui_ShaderLayout::Create()
{
	return new CImGui_ShaderLayout();
}

void CImGui_ShaderLayout::Free()
{
	Super::Free();
}
