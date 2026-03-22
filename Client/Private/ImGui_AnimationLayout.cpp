#include "pch.h"
#include "ImGui_AnimationLayout.h"

#include "Player.h"
#include "Model.h"
#include "Body.h"
#include "ModelAnimation.h"

CImGui_AnimationLayout::CImGui_AnimationLayout()
	: Super("AnimationInfo##Eunbi")
{
}

_bool CImGui_AnimationLayout::Can_Render(CGameObject* pGo)
{
	if (Super::Can_Render(pGo))
	{
		if (m_pPlayerModel = static_cast<CContainerObject*>(pGo)->Get_Part<CBody>(0)->Get_Component<CModel>())
			return true;
	}

	return false;
}

void CImGui_AnimationLayout::Render(CGameObject* pGo)
{
	if (Can_Render(pGo) == false)
		return;

	Update_ModelInfo();

	ImGui::BeginGroup();

	Render_AnimationList();

	Render_AnimationInfo();

	Render_ChangeAnimInfo();

	ImGui::EndGroup();
}

void CImGui_AnimationLayout::Update_ModelInfo()
{
	if (m_pPlayerModel)
	{
		m_iAnimNums = m_pPlayerModel->Get_AnimationCount();

		m_wstrSelectAnimName = m_pPlayerModel->Get_AnimationName(m_iSelectAnimation);
		m_fSelectOffset = (m_pPlayerModel->Get_Animation(m_iSelectAnimation))->Get_MotionOffset();
		m_fSelectSpeed = (m_pPlayerModel->Get_Animation(m_iSelectAnimation))->Get_AnimationSpeed();
	}
}

void CImGui_AnimationLayout::Render_AnimationList()
{
	ImGui::Separator();
	ImGui::Text("Animation List");
	ImGui::Spacing();

	if (ImGui::BeginListBox("##AnimationList", ImVec2(300.f, 300.f)))
	{
		for (_uint i = 0; i < m_iAnimNums; ++i)
		{
			std::wstring wstrName = m_pPlayerModel->Get_AnimationName(i);
			std::string strName  = Engine_Utils::ToString(wstrName);

			std::string label = std::to_string(i) + " : " + strName;

			if (ImGui::Selectable(label.c_str()))
			{
				m_iSelectAnimation = i;
				//m_iChageAnimation = i;
				Update_ModelInfo();
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Separator();
}

void CImGui_AnimationLayout::Render_AnimationInfo()
{
	// ===== UI 출력 =====
	ImGui::Separator();
	ImGui::Text("Selected Animation Info");
	ImGui::Spacing();

	ImGui::Text("Selected Anim Index : %u", m_iSelectAnimation);

	// wstring -> string 변환 필요
	std::string animName(Engine_Utils::ToString(m_wstrSelectAnimName));
	ImGui::Text("Selected Anim Name  : %s", animName.c_str());



	////////////// speed ////////////////////////

	ImGui::Text("Selected Anim MotionOffset : %f", m_fSelectOffset);
	ImGui::SetNextItemWidth(120.f);  // 원하는 픽셀 길이
	ImGui::InputFloat("Change Offset", &m_fChnageOffset, 0.01f, 1.0f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("Apply##OffsetEnugmsdfdf"))
	{
		if (m_pPlayerModel)
		{
			m_pPlayerModel->Get_Animation(m_iSelectAnimation)->Set_MotionOffset(m_fChnageOffset);
			Update_ModelInfo();
		}
	}


	ImGui::Separator();

	ImGui::Text("Selected Anim Speed : %f", m_fSelectSpeed);
	ImGui::SetNextItemWidth(120.f);  // 원하는 픽셀 길이
	ImGui::InputFloat("Change Speed", &m_fChnageSpeed, 0.01f, 1.0f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("Apply##SpeedEnugmsdfdf"))
	{
		if (m_pPlayerModel)
		{
			m_pPlayerModel->Get_Animation(m_iSelectAnimation)->Set_AnimationSpeed(m_fChnageSpeed);
			Update_ModelInfo();
		}
	}
}

void CImGui_AnimationLayout::Render_ChangeAnimInfo()
{
	ImGui::Separator();

	ImGui::SetNextItemWidth(120.f);  // 원하는 픽셀 길이
	ImGui::InputInt("Choose AnimIdx", &m_iChageAnimation, 1);

	// 최소값 -1 제한
	if (m_iChageAnimation < 0)
		m_iChageAnimation = 0;

	if (m_iChageAnimation >= (_int)m_iAnimNums)
	{
		m_iChageAnimation = m_iAnimNums - 1;
	}

	ImGui::SameLine();
	if (ImGui::Button("Apply##PlayerAnimIdxEUngi"))
	{
		if (m_pPlayerModel)
		{
			m_iSelectAnimation = m_iChageAnimation;
			Update_ModelInfo();
		}
	}
}

CImGui_AnimationLayout* CImGui_AnimationLayout::Create()
{
	return new CImGui_AnimationLayout();
}

void CImGui_AnimationLayout::Free()
{
	__super::Free();
}
