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
    _float fNoLimitTimeDelta = m_pGameInstance->Get_TimeDelta(L"Timer_Work");
    if (fTimeDelta < 1e-6f)
        fTimeDelta = 1e-6f;
    if (fNoLimitTimeDelta < 1e-6f)
        fNoLimitTimeDelta = 1e-6f;

    // 평균
    constexpr _float fAlpha = 0.05f;
    {
        if (m_fAvgFrame <= 0.f)
            m_fAvgFrame = fTimeDelta;
        else
            m_fAvgFrame = m_fAvgFrame + (fTimeDelta - m_fAvgFrame) * fAlpha;

        if (m_fAvgNoLimitFrame <= 0.f)
            m_fAvgNoLimitFrame = fNoLimitTimeDelta;
        else
            m_fAvgNoLimitFrame = m_fAvgNoLimitFrame + (fNoLimitTimeDelta - m_fAvgNoLimitFrame) * fAlpha;
    }

    // 최소
    {
        if (m_fMinFrame <= 0.f)
            m_fMinFrame = fTimeDelta;
        else
            m_fMinFrame = (std::max)(m_fMinFrame, fTimeDelta);

        if (m_fMinNoLimitFrame <= 0.f)
            m_fMinNoLimitFrame = fNoLimitTimeDelta;
        else
            m_fMinNoLimitFrame = (std::max)(m_fMinNoLimitFrame, fNoLimitTimeDelta);
    }

    // 최대
    {
        if (m_fBestNoLimitFrame <= 0.f)
            m_fBestNoLimitFrame = fNoLimitTimeDelta;
        else
            m_fBestNoLimitFrame = (std::min)(m_fBestNoLimitFrame, fNoLimitTimeDelta);
    }

    const _float fFPS = 1.f / fTimeDelta;
    const _float fAvgFPS = 1.f / m_fAvgFrame;
    const _float fMinFPS = 1.f / m_fMinFrame;
    const _float fNoLimitFPS = 1.f / fNoLimitTimeDelta;
    const _float fNoLimitAvgFPS = 1.f / m_fAvgNoLimitFrame;
    const _float fNoLimitMinFPS = 1.f / m_fMinNoLimitFrame;
    const _float fNoLimitMaxFPS = 1.f / m_fBestNoLimitFrame;

    m_fUIAccSec += fTimeDelta;
    if (m_fUIAccSec >= 1.0f)
    {
        m_fUIAccSec -= 1.0f;

        m_fDispDT60 = fTimeDelta;
        m_fDispFPS60 = fFPS;
        m_fDispAvgFPS60 = fAvgFPS;
        m_fDispMinFPS60 = fMinFPS;

        m_fDispDTNoLimit = fNoLimitTimeDelta;
        m_fDispFPSNoLimit = fNoLimitFPS;
        m_fDispAvgFPSNoLimit = fNoLimitAvgFPS;
        m_fDispMinFPSNoLimit = fNoLimitMinFPS;
        m_fDispMaxFPSNoLimit = fNoLimitMaxFPS;
    }

    ImGui::Text("DT[60]      : %.3f ms", m_fDispDT60 * 1000.f);
    ImGui::Text("FPS[60]     : %.1f", m_fDispFPS60);
    ImGui::Text("Avg FPS     : %.1f", m_fDispAvgFPS60);
    ImGui::Text("Min FPS     : %.1f", m_fDispMinFPS60);
    ImGui::Separator();
    ImGui::Text("DT[Nolimit] : %.3f ms", m_fDispDTNoLimit * 1000.f);
    ImGui::Text("FPS[Nolimit]: %.1f", m_fDispFPSNoLimit);
    ImGui::Text("Avg[Nolimit]: %.1f", m_fDispAvgFPSNoLimit);
    ImGui::Text("Min[Nolimit]: %.1f", m_fDispMinFPSNoLimit);
    ImGui::Text("Max[Nolimit]: %.1f", m_fDispMaxFPSNoLimit);

    if (ImGui::SmallButton("Reset Min"))
    {
        m_fMinFrame = fTimeDelta;
        m_fMinNoLimitFrame = fNoLimitTimeDelta;
        m_fDispMaxFPSNoLimit = 0.f;
    }

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
