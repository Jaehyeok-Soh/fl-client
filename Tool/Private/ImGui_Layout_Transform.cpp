#include "ImGui_ToolManager.h"
#include "Transform.h"
#include "CameraMan.h"
#include "ToolObject.h"
#include "ImGui_Layout_Transform.h"

CImGui_Layout_Transform::CImGui_Layout_Transform(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pLabel, pDevice, pDeviceContext)
{
}

HRESULT CImGui_Layout_Transform::Render(CToolObject* pGo)
{
    auto makeVerticalBar = [&](ImVec2 minPos, ImVec2 maxPos, _float fThickness, ImU32 Color)
    {
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(minPos.x, minPos.y),
            ImVec2(minPos.x + fThickness, maxPos.y),
            Color,
            3.0f
        );
    };

    if (!pGo)
    {
        m_pPrev = pGo;
    }
    else
    {
        ///////////////
        // Trnasform //
        ///////////////

        CImGui_ToolManager::EGuizmoState eState = CImGui_ToolManager::GetInstance()->Get_GuizmoState();
        _bool bTrans = { false };
        _bool bRotate = { false };
        _bool bScale = { false };
        switch (eState)
        {
        case Tool::CImGui_ToolManager::TRANSLATION:
            bTrans = true;
            break;
        case Tool::CImGui_ToolManager::ROTATION:
            bRotate = true;
            break;
        case Tool::CImGui_ToolManager::SCALE:
            bScale = true;
            break;
        }
        if (ImGui::RadioButton("T", bTrans))
            CImGui_ToolManager::GetInstance()->Set_GuizmoState(Tool::CImGui_ToolManager::TRANSLATION);
        ImGui::SameLine();

        if(ImGui::RadioButton("R", bRotate))
            CImGui_ToolManager::GetInstance()->Set_GuizmoState(Tool::CImGui_ToolManager::ROTATION);
        ImGui::SameLine();

        if(ImGui::RadioButton("S", bScale))
            CImGui_ToolManager::GetInstance()->Set_GuizmoState(Tool::CImGui_ToolManager::SCALE);
        ImGui::SameLine();

        if (ImGui::Button("InitCamera"))
        {
            CTransform* pMainCamTransform = CGameInstance::GetInstance()->Get_MainCamera()->Get_Component<CTransform>();
            pMainCamTransform->Set_Info(TRANSFORM_INFO_STATE::POS, ::XMVectorSet(0.f, 0.5f, -2.f, 1.f));
            pMainCamTransform->Look_At(::XMVectorSet(0.f, 0.f, 0.f, 1.f));
        }


        if (ImGui::BeginTable("Transform#map", 15, ImGuiTableFlags_SizingStretchSame))
        {
            CTransform* pTransform = pGo->Get_Component<CTransform>();
            _float4x4 matWorld = pTransform->Get_WorldMatrix();
            _float3 vScale = {};
            _float3 vRotation = {};
            _float3 vTranslation = {};
            ImGuizmo::DecomposeMatrixToComponents(*matWorld.m, &vTranslation.x, &vRotation.x, &vScale.x);

            ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60.0f);
            ImGui::TableSetupColumn("Sep", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 6.0f);
            ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 100.f);
            ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 100.f);
            ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 100.f);

            auto makeInputFiled = [&](_int iCol, const _char* pLabel, _float* pValue, _float fSens, ImU32 Color)
            {
                ImGui::TableSetColumnIndex(iCol);
                ImGui::SetNextItemWidth(-FLT_MIN);
                _bool bChanged = ImGui::DragFloat(pLabel, pValue, fSens);
                makeVerticalBar(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 4.0f, Color);
            };

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Location");
            ImGui::TableSetColumnIndex(1);
            _float fHeight = ImGui::GetFrameHeight();
            ImGui::Dummy(ImVec2(1, fHeight));
            makeVerticalBar(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.0f, IM_COL32(100, 100, 100, 160));
            makeInputFiled(2, "LocationX##Map", &vTranslation.x, 0.01f, IM_COL32(200, 60, 60, 255));
            makeInputFiled(3, "LocationY##Map", &vTranslation.y, 0.01f, IM_COL32(60, 200, 60, 255));
            makeInputFiled(4, "LocationZ##Map", &vTranslation.z, 0.01f, IM_COL32(60, 120, 220, 255));

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Rotation");
            ImGui::TableSetColumnIndex(1);
            fHeight = ImGui::GetFrameHeight();
            ImGui::Dummy(ImVec2(1, fHeight));
            makeVerticalBar(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.0f, IM_COL32(100, 100, 100, 160));
            makeInputFiled(2, "RotationX##Map", &vRotation.x, 0.1f, IM_COL32(200, 60, 60, 255));
            makeInputFiled(3, "RotationY##Map", &vRotation.y, 0.1f, IM_COL32(60, 200, 60, 255));
            makeInputFiled(4, "RotationZ##Map", &vRotation.z, 0.1f, IM_COL32(60, 120, 220, 255));

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted("Scale");
            ImGui::TableSetColumnIndex(1);
            fHeight = ImGui::GetFrameHeight();
            ImGui::Dummy(ImVec2(1, fHeight));
            makeVerticalBar(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 2.0f, IM_COL32(100, 100, 100, 160));
            makeInputFiled(2, "ScaleX##Map", &vScale.x, 0.01f, IM_COL32(200, 60, 60, 255));
            makeInputFiled(3, "ScaleY##Map", &vScale.y, 0.01f, IM_COL32(60, 200, 60, 255));
            makeInputFiled(4, "ScaleZ##Map", &vScale.z, 0.01f, IM_COL32(60, 120, 220, 255));
            
            if (m_pPrev && m_pPrev == pGo)
            {
                ImGuizmo::RecomposeMatrixFromComponents(&vTranslation.x, &vRotation.x, &vScale.x, *matWorld.m);
                pTransform->Set_Info(TRANSFORM_INFO_STATE::RIGHT, *reinterpret_cast<_float4*>(&matWorld.m[0]));
                pTransform->Set_Info(TRANSFORM_INFO_STATE::UP, *reinterpret_cast<_float4*>(&matWorld.m[1]));
                pTransform->Set_Info(TRANSFORM_INFO_STATE::LOOK, *reinterpret_cast<_float4*>(&matWorld.m[2]));
                pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, *reinterpret_cast<_float4*>(&matWorld.m[3]));
            }

            m_pPrev = pGo;
            ImGui::EndTable();
        }

        ImGui::NewLine();
    }

    return S_OK;
}

CImGui_Layout_Transform* CImGui_Layout_Transform::Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    return new CImGui_Layout_Transform(pLabel, pDevice, pDeviceContext);
}

void CImGui_Layout_Transform::Free()
{
    Super::Free();
}