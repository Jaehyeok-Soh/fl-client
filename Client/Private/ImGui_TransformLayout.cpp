#include "pch.h"
#include "ImGui_TransformLayout.h"
#include "GameObject.h"
#include "Transform.h"
#include "PhysicsCCT.h"
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

    if (m_bInit == false)
    {
        m_vDefaultPos = vPos;
        m_vEditPos = vPos;
        m_bInit = true;
    }

    const _bool bDirty =
        fabsf(m_vEditPos.x - vPos.x) > g_XMEpsilon.f[0] ||
        fabsf(m_vEditPos.y - vPos.y) > g_XMEpsilon.f[0] ||
        fabsf(m_vEditPos.z - vPos.z) > g_XMEpsilon.f[0];

    ImGui::BeginGroup();
    ImGui::SeparatorText(m_strLabel.c_str());

    _float pos[3]{ m_vEditPos.x, m_vEditPos.y, m_vEditPos.z };
    if (ImGui::DragFloat3("##Pos", pos, 0.01f, -100000.f, 100000.f, "%.3f"))
    {
        m_vEditPos = Vec3(pos[0], pos[1], pos[2]);
    }
    
    {
        ImGui::BeginDisabled(bDirty == false);

        if (ImGui::Button("Apply"))
        {
            pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, m_vEditPos);
            pGo->Get_Component<CPhysicsCCT>()->AddFixedMove(m_vEditPos);
            m_vDefaultPos = m_vEditPos;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            m_vEditPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
        }

        ImGui::EndDisabled();
    }
    

    ImGui::Spacing();

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
