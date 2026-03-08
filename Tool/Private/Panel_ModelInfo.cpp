#include "pch.h"
#include "Panel_ModelInfo.h"

// has obj
#include "AnimTool_Manager.h"

CPanel_ModelInfo::CPanel_ModelInfo(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance())
{
}

HRESULT CPanel_ModelInfo::Initialize()
{
	return S_OK;
}

HRESULT CPanel_ModelInfo::Render(CToolObject* pGo)
{
	Render_ObjInfo();

	Render_ModelInfo();

	Render_AnimationInfo();

	return S_OK;
}

void CPanel_ModelInfo::Update(const _float fTimeDelta)
{
	ANIMCTRLINFO tInfo = m_pAnimToolManager->Get_AnimControllInfo();

	m_iCurAnimIdx = tInfo.iCurrentAnimIndex;
	if (CModel* pObjModle = tInfo.pModel)
	{
		m_wstrCurAnimName = pObjModle->Get_AnimationName(m_iCurAnimIdx);

		m_bModelAdditiveOn = pObjModle->Get_Is_AdditiveOn();
		m_iModleRefAnimIdx = pObjModle->Get_RefAdditive_AnimIdx();
		m_iModelPosanimIdx = pObjModle->Get_PosAdditive_AnimIdx();

		//m_iRootBondIdx = tInfo.pModel->Get_RootBone();
		//m_fRootMotionOffset = tInfo.pModel->Get_Animatioin_MotionOffset(m_iCurAnimIdx);
	}

	pObj = tInfo.pCurrentObject;
}

void CPanel_ModelInfo::Render_ObjInfo()
{
	ImGui::Begin("Object Info");

	if (pObj)
	{
		Vec3 vPos =	pObj->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

		ImGui::Text("Position : %.3f, %.3f, %.3f",
			vPos.x, vPos.y, vPos.z);
	}

	ImGui::End();
}

void CPanel_ModelInfo::Render_ModelInfo()
{
	ImGui::Begin("Model Info");

	Render_RootMotionInfo();
	
	Render_AdditiveInfo();

	ImGui::End();
}

void CPanel_ModelInfo::Render_RootMotionInfo()
{
	ImGui::Separator();

	ImGui::SetNextItemWidth(120.f); // 원하는 픽셀 길이
	ImGui::InputInt("RootBone Index", &m_iRootBondIdx, 1);

	/* 값 보정 */
	if (m_iRootBondIdx < -1)
		m_iRootBondIdx = -1;

	ImGui::SameLine();

	if (ImGui::Button("Apply##RootBone"))
	{
		Set_RootBone();
	}

}

void CPanel_ModelInfo::Render_AnimationInfo()
{
	ImGui::Begin("Animation Info");

	Anim_Info();

	RootOffset_Info();

	AnimationSpeed();

	ImGui::End();
}

void CPanel_ModelInfo::Anim_Info()
{
	// ===== UI 출력 =====
	ImGui::Separator();
	ImGui::Text("Current Animation Info");
	ImGui::Separator();

	ImGui::Text("Current Anim Index : %u", m_iCurAnimIdx);

	// wstring -> string 변환 필요
	std::string animName(m_wstrCurAnimName.begin(), m_wstrCurAnimName.end());
	ImGui::Text("Current Anim Name  : %s", animName.c_str());

	ImGui::Separator();
}

void CPanel_ModelInfo::RootOffset_Info()
{
	ImGui::SetNextItemWidth(120.f);  // 원하는 픽셀 길이
	ImGui::InputFloat("RootMotion Offset", &m_fRootMotionOffset, 0.01f, 1.0f, "%.3f");

	// 최소값 -1 제한
	if (m_fRootMotionOffset < -1.f)
		m_fRootMotionOffset = -1.f;

	ImGui::SameLine();

	if (ImGui::Button("Apply##RootOffset"))
	{
		Set_RootOffset();
	}
}

void CPanel_ModelInfo::AnimationSpeed()
{
	ImGui::Separator();

	ImGui::SetNextItemWidth(120.f);  // 원하는 픽셀 길이
	ImGui::InputFloat("Animation Speed", &m_iAnimationSpeed, 0.01f, 1.0f, "%.3f");

	// 최소값 -1 제한
	if (m_iAnimationSpeed < 0.01f)
		m_iAnimationSpeed = 0.01f;

	ImGui::SameLine();

	if (ImGui::Button("Apply##AnimationSpeed"))
	{
		if (m_pAnimToolManager->Get_AnimControllInfo().pModel)
			m_pAnimToolManager->Get_AnimControllInfo().pModel->Set_Animation_Speed(m_iCurAnimIdx, m_iAnimationSpeed);
	}
}

void CPanel_ModelInfo::Render_AdditiveInfo()
{
	ImGui::Separator();

	// additive on off?
	{
		ImGui::Text("Current Additive On ? : %u", (_int)m_bModelAdditiveOn);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60);
		ImGui::SliderInt("0 : Off, 1 : On##AddtivieAnimApplyEunbi", &m_iSelectAdditiveOn, 0, 1);
		ImGui::SameLine();
		if (ImGui::Button("Apply##AddtivieAnimApplyEunbi"))
		{
			if (m_pAnimToolManager->Get_AnimControllInfo().pModel)
				m_pAnimToolManager->Get_AnimControllInfo().pModel->Set_Apply_AdditiveAnim((_bool)m_iSelectAdditiveOn);
		}
	}

	// ref info
	{
		ImGui::Text("Current Ref Index : %d", m_iModleRefAnimIdx);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		ImGui::InputInt("Select Ref AnimIdx##EnumbiRefAnimidx", &m_iSelectRefAnimIdx, 1);
		// 최소값 -1 제한
		if (m_iSelectRefAnimIdx < -1)
			m_iSelectRefAnimIdx = -1;
		ImGui::SameLine();
		if (ImGui::Button("Apply##RefAnimIdx"))
		{
			if (m_pAnimToolManager->Get_AnimControllInfo().pModel)
				m_pAnimToolManager->Get_AnimControllInfo().pModel->Set_AdditiveRef_AnimIdx(m_iSelectRefAnimIdx);
		}
	}

	// pos info
	{
		ImGui::Text("Current Pos Index : %d", m_iModelPosanimIdx);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		ImGui::InputInt("Select Pos AnimIdx##EnumbiPosAnimidx", &m_iSelectPosanimIdx, 1);
		// 최소값 -1 제한
		if (m_iSelectPosanimIdx < -1)
			m_iSelectPosanimIdx = -1;
		ImGui::SameLine();
		if (ImGui::Button("Apply##PosasdfsaAnimIdx"))
		{
			if (m_pAnimToolManager->Get_AnimControllInfo().pModel)
				m_pAnimToolManager->Get_AnimControllInfo().pModel->Set_AdditivePos_AnimIdx(m_iSelectPosanimIdx);
		}
	}
}


void CPanel_ModelInfo::Set_RootBone()
{
	m_pAnimToolManager->Set_RootBone(m_iRootBondIdx);
}

void CPanel_ModelInfo::Set_RootOffset()
{
	m_pAnimToolManager->Set_RootOffset(m_iCurAnimIdx, m_fRootMotionOffset);
}

CPanel_ModelInfo* CPanel_ModelInfo::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_ModelInfo* pInstance = new CPanel_ModelInfo(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_ModelInfo is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_ModelInfo::Free()
{
	__super::Free();
}
