#include "pch.h"
#include "Panel_Parts.h"

// has obj
#include "AnimTool_Manager.h"

CPanel_Parts::CPanel_Parts(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance())
{
}

HRESULT CPanel_Parts::Initialize()
{
	return S_OK;
}

HRESULT CPanel_Parts::Render(CToolObject* pGo)
{
	ImGui::Begin("Parts");

	Render_SelectPart();
	ImGui::Separator();

	Render_PartInfo();

	ImGui::End();

	return S_OK;
}

void CPanel_Parts::Update(const _float fTimeDelta)
{
	if (m_pAnimToolManager->Get_AnimControllInfo().pCurrentObject)
	{
		m_iPartNums = _uint(m_pAnimToolManager->Get_AnimControllInfo().pCurrentObject->Get_PartList().size());

		if (m_pSelectedWeapon)
		{
			CModel* pModel = m_pSelectedWeapon->Get_Component<CModel>();

			if(pModel)
				m_iCurAnimationIdx = pModel->Get_CurrentAnimationIndex();
		}
	}
}

void CPanel_Parts::Render_SelectPart()
{
	if (m_iPartNums > 0)
	{
		ImGui::SliderInt("Part Index", &m_iSelectPartIdx, 0, m_iPartNums - 1);

		ImGui::SameLine();
		if (ImGui::Button("Apply##PartObj"))
		{
			if(m_iSelectPartIdx >=0)
				Set_PartObj();
		}

		ImGui::Text("Current Part Index : %d", m_iSelectPartIdx);
	}
}

void CPanel_Parts::Render_PartInfo()
{
	if (m_pSelectedWeapon)
	{
		Socket_Info();
		ImGui::Separator();

		State_Info();
		ImGui::Separator();

		SRT_Info();
		ImGui::Separator();

		Animation_Info();
	}
}

void CPanel_Parts::Socket_Info()
{
	ImGui::SetNextItemWidth(80.f); // 원하는 픽셀 길이
	ImGui::InputInt("RootBone Index", &m_tInfo.iSocketIdx, 1);

	/* 값 보정 */
	if (m_tInfo.iSocketIdx < -1)
		m_tInfo.iSocketIdx = -1;

	ImGui::SameLine();

	ImGui::SameLine();
	ImGui::Checkbox("Combine Matrix", &m_bCombine);

	ImGui::SameLine();
	if (ImGui::Button("Apply##WeaponSocket"))
	{
		Set_Socket();
	}
}

void CPanel_Parts::State_Info()
{
	ImGui::Checkbox("Render On", &m_bRender);

	if (m_pSelectedWeapon)
		m_pSelectedWeapon->Set_State(_uint(m_bRender));
}

void CPanel_Parts::SRT_Info()
{
	ImGui::Text(" TRANSFORM");
	ImGui::Spacing();

	ImGui::Text(" POSITION X Y Z");
	ImGui::InputFloat3("##position", m_tInfo.vTranslation);
	ImGui::SameLine();
	if (ImGui::Button("Apply##Transform##Position"))
	{
		Set_SRT(CTool_Weapon::SRT::Translation);
	}
	ImGui::Spacing();

	ImGui::Text(" ROTATION X Y Z");
	ImGui::InputFloat3("##rotation", m_tInfo.vPYR);
	ImGui::SameLine();
	if (ImGui::Button("Apply##Transform##Rotation"))
	{
		Set_SRT(CTool_Weapon::SRT::PYR);
	}
	ImGui::Spacing();

	ImGui::Text(" SCALE X Y Z");
	ImGui::InputFloat3("##scale", m_tInfo.vScale);
	ImGui::SameLine();
	if (ImGui::Button("Apply##Transform##Scale"))
	{
		Set_SRT(CTool_Weapon::SRT::Scale);
	}
	ImGui::Spacing();
}

void CPanel_Parts::Animation_Info()
{
	// ref info
	{
		ImGui::Text("Current Anim Index : %d", m_iCurAnimationIdx);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		ImGui::InputInt("Select AnimIdx##PartAnim", &m_iSelecAnimationIdx, 1);
		// 최소값 -1 제한
		if (m_iSelecAnimationIdx < 0)
			m_iSelecAnimationIdx = 0;
		ImGui::SameLine();
		if (ImGui::Button("Apply##PartAnim"))
		{
			if (m_pSelectedWeapon)
				m_pSelectedWeapon->Change_Animation(m_iSelecAnimationIdx);
		}
	}
}

void CPanel_Parts::Set_PartObj()
{
	m_pSelectedWeapon = m_pAnimToolManager->Get_AnimControllInfo().pCurrentObject->Get_Part<CTool_Weapon>(m_iSelectPartIdx);

	if (m_pSelectedWeapon)
		m_tInfo = m_pSelectedWeapon->Get_Info();
}

void CPanel_Parts::Set_Socket()
{
	if (m_pSelectedWeapon)
		m_pSelectedWeapon->Set_Soket(m_tInfo.iSocketIdx,m_bCombine);
}

void CPanel_Parts::Set_SRT(CTool_Weapon::SRT eSRT)
{
	Vec3 vValue;
	switch (eSRT)
	{
	case CTool_Weapon::SRT::Scale:
		vValue = { m_tInfo.vScale[0],m_tInfo.vScale[1] ,m_tInfo.vScale[2] };

		break;

	case CTool_Weapon::SRT::PYR:
		vValue = { m_tInfo.vPYR[0],m_tInfo.vPYR[1] ,m_tInfo.vPYR[2] };
		break;

	case CTool_Weapon::SRT::Translation:
		vValue = { m_tInfo.vTranslation[0],m_tInfo.vTranslation[1] ,m_tInfo.vTranslation[2] };
		break;
	}

	m_pSelectedWeapon->Set_SRT(eSRT, vValue);
}

CPanel_Parts* CPanel_Parts::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_Parts* pInstance = new CPanel_Parts(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_Parts is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_Parts::Free()
{
	__super::Free();
}
