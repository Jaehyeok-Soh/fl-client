#include "pch.h"
#include "Effect_Preview_panel.h"
#include "Engine_Utils.h"
#include "Transform.h"
#include "Effect.h"
#include "CEffectObject.h"
#include "VIBuffer_Particle_Point.h"
#include "Texture.h"
#include "Gravity_Force.h"
#include "Tool_PartObject.h"

#include "GameInstance.h"

CEffect_Preview_panel::CEffect_Preview_panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEffect_Preview_panel::Update(const _float fDT)
{


}

HRESULT CEffect_Preview_panel::Render(CToolObject* pGo)
{


    return S_OK;
}

void CEffect_Preview_panel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}

void CEffect_Preview_panel::Draw_PreviewActor(CToolObject* pGo)
{
	//// 실제 모델을 생성하는 창
	//ImGui::Begin("Effect Animation Paenl");

 //   // 1. 모델 및 애니메이션 선택
 //   if (ImGui::Combo("Preview Model", &m_iSelectedModel, m_pModelList)) { /* 모델 로드 */ }
 //   if (ImGui::Combo("Animation", &m_iSelectedAnim, m_pAnimList)) { /* 애니메이션 설정 */ }

 //   // 2. 타임라인 슬라이더 (Scrubbing)
 //   float fProgress = m_pAnimator->GetProgress(); // 0.0 ~ 1.0
 //   if (ImGui::SliderFloat("Timeline", &fProgress, 0.0f, 1.0f))
 //   {
 //       m_pAnimator->SetProgress(fProgress); // 슬라이더 조작 시 애니메이션 프레임 강제 고정
 //   }

 //   // 3. 재생 제어
 //   if (ImGui::Button("Play"))  m_pAnimator->Play();
 //   ImGui::SameLine();
 //   if (ImGui::Button("Pause")) m_pAnimator->Pause();

 //   // 4. 디버그 옵션
 //   ImGui::Checkbox("Show Hitbox", &m_bShowHitbox);
 //   ImGui::Checkbox("Show Bones", &m_bShowBones);

 //   ImGui::End();

}
