#include "pch.h"
#include "ImGui_TransformLayout.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameInstance.h"

CImGui_TransformLayout::CImGui_TransformLayout()
    : Super("TransformInfo")
{

}

_bool CImGui_TransformLayout::Can_Render(CGameObject* pGo)
{
    return Super::Can_Render(pGo) && (pGo->Get_Component<CTransform>() != nullptr);
}

void CImGui_TransformLayout::Render(CGameObject* pGo)
{
    if (Can_Render(pGo) == false)
        return;

    CTransform* pTransform = pGo->Get_Component<CTransform>();
    Matrix matWorld = pTransform->Get_WorldMatrix();
    Vec3 vPos = matWorld.Translation();

    ImGui::BeginGroup();
    ImGui::SeparatorText(m_strLabel.c_str());

    ImGui::Text("X: %.3f", vPos.x);
    ImGui::SameLine();
    ImGui::Text("Y: %.3f", vPos.y);
    ImGui::SameLine();
    ImGui::Text("Z: %.3f", vPos.z);
    
    ImGui::Spacing();

    if (ImGui::TreeNode("WorldMatrix"))
    {
        ImGui::Text("% .3f % .3f % .3f % .3f", matWorld._11, matWorld._12, matWorld._13, matWorld._14);
        ImGui::Text("% .3f % .3f % .3f % .3f", matWorld._21, matWorld._22, matWorld._23, matWorld._24);
        ImGui::Text("% .3f % .3f % .3f % .3f", matWorld._31, matWorld._32, matWorld._33, matWorld._34);
        ImGui::Text("% .3f % .3f % .3f % .3f", matWorld._41, matWorld._42, matWorld._43, matWorld._44);
        ImGui::TreePop();
    }

    ImGui::EndGroup();
}

CImGui_TransformLayout* CImGui_TransformLayout::Create()
{
    return new CImGui_TransformLayout();
}

void CImGui_TransformLayout::Free()
{
    Super::Free();
}
