#include "pch.h"
#include "ImGui_FrameLayout.h"
#include "GameInstance.h"

CImGui_FrameLayout::CImGui_FrameLayout()
	: Super("FrameInfo")
{
}

void CImGui_FrameLayout::Render(CGameObject* pGo)
{
    ImGui::BeginGroup();
    ImGui::SeparatorText(m_strLabel.c_str());

    _float fTimeDelta = m_pGameInstance->Get_TimeDelta(L"Timer_60");
    if (fTimeDelta < 1e-6f)
        fTimeDelta = 1e-6f;

    // Æò±Õ
    constexpr _float fAlpha = 0.05f;
    if (m_fAvgFrame <= 0.f)
        m_fAvgFrame = fTimeDelta;
    else
        m_fAvgFrame = m_fAvgFrame + (fTimeDelta - m_fAvgFrame) * fAlpha;

    // ÃÖ¼Ò
    if (m_fMinFrame <= 0.f)
        m_fMinFrame = fTimeDelta;
    else
        m_fMinFrame = (std::max)(m_fMinFrame, fTimeDelta);

    const _float fFPS = 1.f / fTimeDelta;
    const _float fAvgFPS = 1.f / m_fAvgFrame;
    const _float fMinFPS = 1.f / m_fMinFrame;

    ImGui::Text("DT[60]      : %.3f ms", fTimeDelta * 1000.f);
    ImGui::Text("FPS[60]     : %.1f", fFPS);
    ImGui::Text("Avg FPS     : %.1f", fAvgFPS);
    ImGui::Text("Min FPS     : %.1f", fMinFPS);

    if (ImGui::SmallButton("Reset Min"))
        m_fMinFrame = fTimeDelta;

    ImGui::EndGroup();
}

CImGui_FrameLayout* CImGui_FrameLayout::Create()
{
    return new CImGui_FrameLayout();
}

void CImGui_FrameLayout::Free()
{
	Super::Free();
}
