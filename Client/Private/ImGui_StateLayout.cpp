#include "pch.h"
#include "ImGui_StateLayout.h"
#include "GameObject.h"
#include "ActionState.h"

#include "ContainerObject.h"
#include "Body.h"
#include "Model.h"
#include "Bone.h"
#include "StateBase_Player.h"
#include "StatCom_Player.h"
#include "Player.h"
#include "PhysicsCCT.h"

#include "GameInstance.h"

#include "PlayerImguiValues.h"

float g_ImguiFloat1;
float g_ImguiFloat2;
float g_ImguiFloat3;

CImGui_StateLayout::CImGui_StateLayout()
	: Super("StateInfo")
{
	g_ImguiFloat1 = 0.f;
	g_ImguiFloat2 = 0.f;
	g_ImguiFloat3 = 0.f;
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
	CStatCom_Player* pStat = static_cast<CStatCom_Player*>(static_cast<CPlayer*>(pGo)->Get_Component<CMyStat>());

	CModel* pModel = static_cast<CContainerObject*>(pGo)->Get_Part<CBody>(0)->Get_Component<CModel>();
	_float fBlendTime = pModel->Get_BlentTime();
	_int iAnimIdx = pModel->Get_CurrentAnimationIndex();
	Vector3 vCamBonePos = pModel->Get_Bone(417)->Get_CombinedTransformMatrix().Translation();
	_uint iDashCount = pStat->Get_Count(CStatCom_Player::TIMER_TYPE::DASH);
	_float fMental = pStat->Get_Stat_Vec2(CMyStat::STAT_TYPE::MENTAL).x;

	CPhysicsCCT* pCCT = pGo->Get_Component<CPhysicsCCT>();


	CTransform* pTrans = pGo->Get_Component<CTransform>();

	Vec3 vPositoin = pTrans->Get_Info(TRANSFORM_INFO_STATE::POS);

	string strRootApply = pModel->Is_RootMotion_Apply() ? "Yes" : " No";

	ImGui::BeginGroup();
	ImGui::SeparatorText(m_strLabel.c_str());;

	ImGui::Text("State : ");
	ImGui::SameLine();
	ImGui::Text(strStateName.c_str());

	ImGui::Text("Position : ");
	ImGui::SameLine();
	ImGui::Text("%f", vPositoin.x);
	ImGui::SameLine();
	ImGui::Text("%f", vPositoin.y);
	ImGui::SameLine();
	ImGui::Text("%f", vPositoin.z);

	ImGui::Text("RootApply : ");
	ImGui::SameLine();
	ImGui::Text(strRootApply.c_str());

	ImGui::Text("Dash Count : ");
	ImGui::SameLine();
	ImGui::Text("%u", iDashCount);

	ImGui::Text("Mental : ");
	ImGui::SameLine();
	ImGui::Text("%f", fMental);

	ImGui::Text("Main Ani Idx : "); // state 내부에서 anim idx
	ImGui::SameLine();
	ImGui::Text("%u", iMainIdx);

	ImGui::Text("Model Ani Idx : "); // model에서 바인딩된 anim idx
	ImGui::SameLine();
	ImGui::Text("%u", iAnimIdx);

	ImGui::Text("BlendTime : ");
	ImGui::SameLine();
	ImGui::Text("%f", fBlendTime);

	ImGui::Text("State Time : ");
	ImGui::SameLine();
	ImGui::Text("%f", fStateTime);


	ImGui::Text("Cur Speed : ");
	ImGui::SameLine();
	ImGui::Text("%f", pCCT->GetMoveState()->vVelocity.magnitude());

	if (ImGui::TreeNode(" PlayerInfo "))
	{
		SHADER_PLAYER_INFO* pInfo = static_cast<CPlayer*>(pGo)->Get_PlayerInfo();
		if (!pInfo)
			ImGui::TreePop();
		ImGui::DragFloat(" MaxSpeed " , &pInfo->fMaxSpeed, 0.001f, 0.f, 100.f);
		ImGui::TreePop();
	}

	Render_Float(pGo);


	ImGui::EndGroup();
}

void CImGui_StateLayout::Render_Float(CGameObject* pGo)
{
	ImGui::Text("Cur float1 : ");
	ImGui::SameLine();
	ImGui::Text("%f", g_ImguiFloat1);

	ImGui::InputFloat("Change Float1", &m_fFloat1, 0.01f, 1.0f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("Apply##Floatwelpkrwlefkwn"))
	{
		g_ImguiFloat1 = m_fFloat1;
	}


	ImGui::Text("Cur flaot2 : ");
	ImGui::SameLine();
	ImGui::Text("%f", g_ImguiFloat2);

	ImGui::InputFloat("Change Float2", &m_fFloat2, 0.01f, 1.0f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("Apply##Floatwelpkrwlefkwn2"))
	{
		g_ImguiFloat2 = m_fFloat2;
	}

	ImGui::Text("Cur float3 : ");
	ImGui::SameLine();
	ImGui::Text("%f", g_ImguiFloat3);

	ImGui::InputFloat("Change Float3", &m_fFloat3, 0.01f, 1.0f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("Apply##Floatwelpkrwlefkwn3"))
	{
		g_ImguiFloat3 = m_fFloat3;
	}
}

CImGui_StateLayout* CImGui_StateLayout::Create()
{
	return new CImGui_StateLayout();
}

void CImGui_StateLayout::Free()
{
	Super::Free();
}
