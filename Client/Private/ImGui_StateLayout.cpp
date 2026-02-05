#include "pch.h"
#include "ImGui_StateLayout.h"
#include "GameObject.h"
#include "ActionState.h"
#include "GameInstance.h"

CImGui_StateLayout::CImGui_StateLayout()
	: Super("StateInfo")
{
}

_bool CImGui_StateLayout::Can_Render(CGameObject* pGo)
{
	return Super::Can_Render(pGo) && (pGo->Get_Component<CActionState>() != nullptr);
}

void CImGui_StateLayout::Render(CGameObject* pGo)
{
	if (Can_Render(pGo) == false)
		return;

	CActionState* pActionState = pGo->Get_Component<CActionState>();
	string	strStateName	= pActionState->Get_CurrentStateName();
	_uint	iMainIdx		= pActionState->Get_CurrentState()->Get_MainAnimIdx();
	_float	fStateTime		= pActionState->Get_CurrentState()->Get_StateElapsedTime();

	ImGui::BeginGroup();
	ImGui::SeparatorText(m_strLabel.c_str());

	ImGui::Text("State : ");
	ImGui::SameLine();
	ImGui::Text(strStateName.c_str());

	ImGui::Text("Main Ani Idx : ");
	ImGui::SameLine();
	ImGui::Text("%u", iMainIdx);

	ImGui::Text("State Duration : ");
	ImGui::SameLine();
	ImGui::Text("%f", fStateTime);

	ImGui::EndGroup();
}

CImGui_StateLayout* CImGui_StateLayout::Create()
{
	return new CImGui_StateLayout();
}

void CImGui_StateLayout::Free()
{
	Super::Free();
}
