#include "pch.h"
#include "ImGui_StateLayout.h"
#include "GameObject.h"
#include "ActionState.h"

#include "ContainerObject.h"
#include "Body.h"
#include "Model.h"
#include "Bone.h"

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

	CModel* pModel = static_cast<CContainerObject*>(pGo)->Get_Part<CBody>(0)->Get_Component<CModel>();
	_float fBlendTime = pModel->Get_BlentTime();
	_int iAnimIdx = pModel->Get_CurrentAnimationIndex();
	Vector3 vCamBonePos = pModel->Get_Bone(417)->Get_CombinedTransformMatrix().Translation();

	ImGui::BeginGroup();
	ImGui::SeparatorText(m_strLabel.c_str());

	ImGui::Text("State : ");
	ImGui::SameLine();
	ImGui::Text(strStateName.c_str());

	ImGui::Text("Main Ani Idx : "); // state 내부에서 anim idx
	ImGui::SameLine();
	ImGui::Text("%u", iMainIdx);

	ImGui::Text("Model Ani Idx : "); // model에서 바인딩된 anim idx
	ImGui::SameLine();
	ImGui::Text("%u", iAnimIdx);

	ImGui::Text("State Duration : ");
	ImGui::SameLine();
	ImGui::Text("%f", fStateTime);

	ImGui::Text("BlendTime : ");
	ImGui::SameLine();
	ImGui::Text("%f", fBlendTime);

	ImGui::Text("CamBone Pos : ");
	ImGui::SameLine();
	ImGui::Text("%f", vCamBonePos.x);
	ImGui::SameLine();
	ImGui::Text("%f", vCamBonePos.y);
	ImGui::SameLine();
	ImGui::Text("%f", vCamBonePos.z);

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
