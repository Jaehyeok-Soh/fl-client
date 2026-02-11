#include "pch.h"
#include "ImGui_FrustrumLayout.h"
#include "GameObject.h"
#include "Transform.h"
#include "CameraMan.h"
#include "GameInstance.h"

CImGui_FrustrumLayout::CImGui_FrustrumLayout()
    : Super("FrustrumInfo")
{
}

_bool CImGui_FrustrumLayout::Can_Render(CGameObject* pGo)
{
    return m_pGameInstance->Get_MainCamera() != nullptr;
}

void CImGui_FrustrumLayout::Render(CGameObject* pGo)
{
    if (Can_Render(pGo) == false)
        return;

    CCamera* pCameraComp = m_pGameInstance->Get_MainCamera()->Get_Component<CCamera>();
    _float fMidStart = m_pGameInstance->Get_FrustrumMidStart();
    _float fFarStart = m_pGameInstance->Get_FrustrumFarStart();


    _float fNear = pCameraComp->Get_Near();
    _float fFar = pCameraComp->Get_Far();
    // mid < far 강제용
    constexpr _float fEpsilon = 0.05f;

    ImGui::BeginGroup();
    ImGui::Text("Frustum Split (Near/Mid/Far)");
    ImGui::Separator();

    bool bChanged = false;

    bChanged |= ImGui::DragFloat("Mid Start", &fMidStart, 0.5f, fNear + 0.01f, fFar - 0.02f, "%.2f");
    bChanged |= ImGui::DragFloat("Far Start", &fFarStart, 0.5f, fNear + 0.02f, fFar - 0.01f, "%.2f");

    // 유효성 보정: 항상 Near < Mid < Far < FarPlane
    fMidStart = std::clamp(fMidStart, fNear + 0.01f, fFar - 0.02f);
    fFarStart = std::clamp(fFarStart, fMidStart + fEpsilon, fFar - 0.01f);

    // Apply 버튼(원하면 자동 적용도 가능)
    static bool sAutoApply = true;
    ImGui::Checkbox("Auto Apply", &sAutoApply);

    if (sAutoApply && bChanged)
    {
        m_pGameInstance->Resize_SplitFrustrum(fMidStart, fFarStart);
    }
    else
    {
        if (ImGui::Button("Apply"))
        {
            m_pGameInstance->Resize_SplitFrustrum(fMidStart, fFarStart);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset (30/80)"))
        {
            m_pGameInstance->Resize_SplitFrustrum(30.f, 80.f);
        }
    }

    // 디버그 표시
    ImGui::Text("Near=%.2f  Mid=%.2f  Far=%.2f  FarPlane=%.2f", fNear, fMidStart, fFarStart, fFar);

    ImGui::EndGroup();
    
}

CImGui_FrustrumLayout* CImGui_FrustrumLayout::Create()
{
    return new CImGui_FrustrumLayout();
}

void CImGui_FrustrumLayout::Free()
{
    Super::Free();
}
