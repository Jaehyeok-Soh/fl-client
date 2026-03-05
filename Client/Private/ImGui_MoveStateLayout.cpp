#include "pch.h"
#include "ImGui_MoveStateLayout.h"
#include "GameObject.h"
#include "Transform.h"
#include "PhysicsCCT.h"
#include "GameInstance.h"

CImGui_MoveStateLayout::CImGui_MoveStateLayout()
	: Super("MoveState")
{
}

_bool CImGui_MoveStateLayout::Can_Render(CGameObject* pGo)
{
	return Super::Can_Render(pGo) && (pGo->Get_Component<CPhysicsCCT>() != nullptr);
}

void CImGui_MoveStateLayout::Render(CGameObject* pGo)
{
    if (Can_Render(pGo) == false)
        return;

    CPhysicsCCT* pCCT = pGo->Get_Component<CPhysicsCCT>();
    CPhysicsCCT::CCTMOVEMENTSTATE* moveState = pCCT->GetMoveState();

    if (m_bInit == false)
    {
    }

    ImGui::BeginGroup();
    ImGui::SeparatorText(m_strLabel.c_str());

    // ==========================================
    // 1. 기본 세팅 (Editable)
    // ==========================================
    if (ImGui::TreeNodeEx("1. Base Settings", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Apply Gravity", &moveState->bGravity);

        // 중력은 음수이므로 범위나 속도를 적절히 조절 (DragFloat 사용)
        ImGui::DragFloat("Gravity Force", &moveState->fGravity, 0.1f);

        ImGui::Spacing();
        ImGui::DragFloat("Accel Rate", &moveState->CMAccelRate.x, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("DeAccel Rate", &moveState->CMDeAccelRate.x, 0.1f, 0.0f, 100.0f);

        ImGui::TreePop();
    }

    // ==========================================
    // 2. 속도 제한 세팅 (Editable)
    // ==========================================
    // CurMinMax가 내부에 x, y, z (또는 cur, min, max) float 3개로 구성되어 있다고 가정합니다.
    if (ImGui::TreeNodeEx("2. Speed Limits", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // Vector3 형태이므로 DragFloat3 사용
        ImGui::DragFloat3("Speed (Cur/Min/Max)", &moveState->CMSpeed.x, 0.1f);
        ImGui::DragFloat3("Vertical Speed (Cur/Min/Max)", &moveState->CMVerticalSpeed.x, 0.1f);

        ImGui::TreePop();
    }

    // ==========================================
    // 3. 실시간 모니터링 (Read-only)
    // ==========================================
    if (ImGui::TreeNodeEx("3. Real-time Vectors (Read Only)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        // 물리 연산 결과값이므로 조작하지 못하게 회색으로 비활성화 처리
        ImGui::BeginDisabled();

        // PxVec3는 연속된 3개의 float(x,y,z)이므로 주소값(&vec.x)을 넘겨주면 완벽히 호환됩니다.
        ImGui::InputFloat3("Input Dir", &moveState->vInputDir.x);
        ImGui::InputFloat3("Target Vel", &moveState->vTargetVelocity.x);

        ImGui::Spacing();
        ImGui::InputFloat3("Current Vel", &moveState->vVelocity.x);
        ImGui::InputFloat3("Fixed Move", &moveState->vFixedMove.x);

        ImGui::Spacing();
        ImGui::InputFloat3("Accel (Total)", &moveState->vAccelation.x);
        ImGui::InputFloat3("Accel (Extern)", &moveState->vExternAccelation.x);
        ImGui::InputFloat3("Accel (Impulse)", &moveState->vImpulsAccelation.x);

        ImGui::Spacing();
        ImGui::InputFloat3("Impulse", &moveState->vImpulsAccelation.x);

        ImGui::EndDisabled();

        ImGui::TreePop();
    }

    ImGui::EndGroup();
}

CImGui_MoveStateLayout* CImGui_MoveStateLayout::Create()
{
	return new CImGui_MoveStateLayout();
}

void CImGui_MoveStateLayout::Free()
{
	Super::Free();
}
