#include "pch.h"
#include "ImGui_ShaderLayout.h"
#include "Light.h"
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

    if (m_pDirLight == nullptr)
    {
        m_pDirLight = m_pGameInstance->Get_Light(LIGHT_TYPE::DIRECTIONAL);
        ImGui::TextDisabled("MainDir not found.");
        return;
    }

    if (m_bDefaultCached == false)
    {
        m_defLight = m_pDirLight->Get_LightDesc();
        m_changedLightDesc = m_defLight;
        m_defSSAO = m_pGameInstance->Get_SSAOParamDesc();
        m_defHDR = m_pGameInstance->Get_HDRParamDesc();
        m_defBloom = m_pGameInstance->Get_BloomParamDesc();
        m_defOutline = m_pGameInstance->Get_OutlineParamDesc();
        m_defFog = m_pGameInstance->Get_FogParamDesc();
        m_defToon = m_pGameInstance->Get_ToonParamDesc();
        m_defCascade = m_pGameInstance->Get_CascadeParamDesc();
        m_bDefaultCached = true;
    }

    ImGui::BeginGroup();
    ImGui::SeparatorText("PostProcess / Shader Params");

    ImGui::Checkbox("Auto Apply##ShaderLayout", &m_bAutoApply);
    ImGui::SameLine();
    if (ImGui::SmallButton("Apply All##ShaderLayout"))
        m_pGameInstance->Commit_AllPostParams();

    ImGui::SameLine();
    if (ImGui::SmallButton("Reset All##ShaderLayout"))
    {
        m_pDirLight->Setup_LightDesc(m_defLight);
        m_pGameInstance->Get_SSAOParamDesc() = m_defSSAO;
        m_pGameInstance->Get_HDRParamDesc() = m_defHDR;
        m_pGameInstance->Get_BloomParamDesc() = m_defBloom;
        m_pGameInstance->Get_OutlineParamDesc() = m_defOutline;
        m_pGameInstance->Get_FogParamDesc() = m_defFog;
        m_pGameInstance->Get_ToonParamDesc() = m_defToon;
        m_pGameInstance->Get_CascadeParamDesc() = m_defCascade;
        m_pGameInstance->Commit_AllPostParams();
    }

    ImGui::Separator();
    // -----------------------
    // DirLight
    // -----------------------
    if (ImGui::CollapsingHeader("LightDir", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("LightDir");
        _bool bChanged = false;

        bChanged |= ImGui::DragFloat4("Diffuse", &m_changedLightDesc.vDiffuse.x, 0.01f, 0.0f, 5.0f, "%.2f");
        bChanged |= ImGui::DragFloat4("Ambient", &m_changedLightDesc.vAmbient.x, 0.01f, 0.0f, 5.0f, "%.2f");
        bChanged |= ImGui::DragFloat3("Direction", &m_changedLightDesc.vDirection.x, 0.01f, -1.0f, 1.0f, "%.3f");

        m_changedLightDesc.vSpecular = m_changedLightDesc.vDiffuse;

        ImGui::SameLine();
        if (ImGui::SmallButton("Normalize##Dir"))
        {
            Vector3& vDir = m_changedLightDesc.vDirection;
            if (vDir.LengthSquared() > g_XMEpsilon[0])
            {
                vDir.Normalize();
                bChanged = true;
            }
        }

        auto ApplyDirLight = [&]()
            {
                Vector3& vDir = m_changedLightDesc.vDirection;
                if (vDir.LengthSquared() <= g_XMEpsilon[0])
                    vDir = { 0.f, -1.f, 0.f };
                else
                    vDir.Normalize();

                m_changedLightDesc.vSpecular = m_changedLightDesc.vDiffuse;
                m_pDirLight->Setup_LightDesc(m_changedLightDesc);
            };

        if (m_bAutoApply == false)
        {
            if (ImGui::SmallButton("Apply##DirLight"))
                ApplyDirLight();
        }
        else if (bChanged)
        {
            ApplyDirLight();
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("Reset##DirLight"))
        {
            m_changedLightDesc = m_defLight;
            m_changedLightDesc.vSpecular = m_changedLightDesc.vDiffuse;
            ApplyDirLight();
        }

        ImGui::PopID();
    }


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
        _float col[4] = { o.vColor.x, o.vColor.y, o.vColor.z, o.vColor.w };
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

    // -----------------------
    // Fog
    // -----------------------
    if (ImGui::CollapsingHeader("Fog", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Fog");
        auto& fog = m_pGameInstance->Get_FogParamDesc();

        _bool bChanged = false;

        _float col[4] = { fog.vColor.x, fog.vColor.y, fog.vColor.z, fog.vColor.w };
        if (ImGui::ColorEdit3("FogColor", col))
        {
            fog.vColor = { col[0], col[1], col[2], col[3] };
            bChanged = true;
        }

        _float colH[4] = { fog.vHighColor.x, fog.vHighColor.y, fog.vHighColor.z, fog.vHighColor.w };
        if (ImGui::ColorEdit3("HighColor", colH))
        {
            fog.vHighColor = { colH[0], colH[1], colH[2], colH[3] };
            bChanged = true;
        }

        ImGui::SeparatorText("Distance Fog");
        bChanged |= ImGui::SliderFloat("Start", &fog.fFogStart, 0.f, 200.f);
        bChanged |= ImGui::SliderFloat("End", &fog.fFogEnd, 1.f, 500.f);
        bChanged |= ImGui::SliderFloat("Density", &fog.fFogDensity, 0.f, 0.5f, "%.4f");
        bChanged |= ImGui::SliderFloat("MaxOpacity", &fog.fFogMaxOpacity, 0.f, 1.f);

        ImGui::TextDisabled("Density=0: Linear, >0: Exponential");

        ImGui::SeparatorText("Height Fog");
        bChanged |= ImGui::SliderFloat("BaseHeight", &fog.fFogBaseHeight, -500.f, 500.f, "%.5f");
        bChanged |= ImGui::SliderFloat("HeightFalloff", &fog.fFogHeightFalloff, 0.01f, 1.f, "%.3f");
        bChanged |= ImGui::SliderFloat("HeightDensity", &fog.fFogHeightDensity, 0.f, 0.2f, "%.4f");

        ImGui::SeparatorText("Noise");
        bChanged |= ImGui::SliderFloat("NoiseScale", &fog.fFogNoiseScale, 0.f, 1.f);
        bChanged |= ImGui::SliderFloat("NoiseSpeed", &fog.fFogNoiseSpeed, 0.f, 2.f);

        ImGui::TextDisabled("NoiseScale=0: Noise Off");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##Fog"))
                m_pGameInstance->Commit_FogParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_FogParam();
        }

        if (ImGui::SmallButton("Reset##Fog"))
        {
            fog = m_defFog;
            m_pGameInstance->Commit_FogParam();
        }
        ImGui::PopID();
    }
    // -----------------------
    // Toon
    // -----------------------
    if (ImGui::CollapsingHeader("Toon", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("Toon");
        auto& toon = m_pGameInstance->Get_ToonParamDesc();

        _bool bChanged = false;

        ImGui::SeparatorText("Diffuse");
        bChanged |= ImGui::SliderFloat("Wrap", &toon.fWrap, 0.f, 1.f, "%.2f");
        bChanged |= ImGui::SliderFloat("ShadowMid", &toon.fShadowMid, 0.f, 1.f, "%.2f");
        bChanged |= ImGui::SliderFloat("ShadowSoftness", &toon.fShadowSoftness, 0.f, 0.5f, "%.3f");
        bChanged |= ImGui::SliderFloat("ShadowStrength", &toon.fShadowStrength, 0.f, 1.f, "%.2f");
        bChanged |= ImGui::SliderFloat("DiffuseStrength", &toon.fDiffuseStrength, 0.f, 3.f, "%.2f");

        ImGui::SeparatorText("Rim");
        bChanged |= ImGui::SliderFloat("RimThreshold", &toon.fRimThreshold, 0.f, 1.f, "%.2f");
        bChanged |= ImGui::SliderFloat("RimSoftness", &toon.fRimSoftness, 0.f, 0.5f, "%.3f");
        bChanged |= ImGui::SliderFloat("RimStrength", &toon.fRimStrength, 0.f, 3.f, "%.2f");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##Toon"))
                m_pGameInstance->Commit_ToonParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_ToonParam();
        }

        if (ImGui::SmallButton("Reset##Toon"))
        {
            toon = m_defToon;
            m_pGameInstance->Commit_ToonParam();
        }
        ImGui::PopID();
    }
    // -----------------------
    // Cascade
    // -----------------------
    if (ImGui::CollapsingHeader("Cascade Shadow", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushID("CascadeShadow");
        auto& shadow = m_pGameInstance->Get_CascadeParamDesc();
        _bool bChanged = false;

        ImGui::SeparatorText("Cascade Split");
        bChanged |= ImGui::SliderFloat("Cascade0 End", &shadow.fCascadeEnd0, 3.f, 30.f, "%.1f");
        bChanged |= ImGui::SliderFloat("Cascade1 End", &shadow.fCascadeEnd1, 10.f, 100.f, "%.1f");

        ImGui::SeparatorText("Bias");
        bChanged |= ImGui::SliderFloat("ShadowBias", &shadow.fShadowBias, 0.0001f, 0.02f, "%.4f");
        bChanged |= ImGui::SliderFloat("NormalBias", &shadow.fNormalBias, 0.f, 0.1f, "%.3f");

        ImGui::SeparatorText("Appearance");
        bChanged |= ImGui::SliderFloat("Strength", &shadow.fShadowStrength, 0.f, 1.f, "%.2f");

        if (m_bAutoApply == false)
        {
            ImGui::SameLine();
            if (ImGui::SmallButton("Apply##Shadow"))
                m_pGameInstance->Commit_CascadeParam();
        }
        else if (bChanged)
        {
            m_pGameInstance->Commit_CascadeParam();
        }

        if (ImGui::SmallButton("Reset##Shadow"))
        {
            shadow = m_defCascade;
            m_pGameInstance->Commit_CascadeParam();
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
