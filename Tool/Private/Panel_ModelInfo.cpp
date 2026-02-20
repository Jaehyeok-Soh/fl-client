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
	Render_RootMotionInfo();

	return S_OK;
}

void CPanel_ModelInfo::Update(const _float fTimeDelta)
{
}

void CPanel_ModelInfo::Render_RootMotionInfo()
{
	ImGui::Begin("RootBone Info");

	ImGui::InputInt("RootBone Index", &m_iRootBondIdx, 1);

	/* °ª º¸Á¤ */
	if (m_iRootBondIdx < -1)
		m_iRootBondIdx = -1;

	ImGui::SameLine();

	if (ImGui::Button("Apply"))
	{
		Set_RootBone();
	}

	ImGui::End();
}

void CPanel_ModelInfo::Set_RootBone()
{
	m_pAnimToolManager->Set_RootBone(m_iRootBondIdx);
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
