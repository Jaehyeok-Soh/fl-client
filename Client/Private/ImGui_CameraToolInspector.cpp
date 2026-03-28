#include "pch.h"
#include "ImGui_CameraToolInspector.h"
#include "ImGui_CameraShotLayout.h"
#include "GameInstance.h"

CImGui_CameraToolInspector::CImGui_CameraToolInspector()
	: Super("CameraDebugInspector", ELayoutType::COUNT)
{
}

HRESULT CImGui_CameraToolInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::CameraShot, CImGui_CameraShotLayout::Create())))
		return E_FAIL;

	return S_OK;
}

void CImGui_CameraToolInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::CameraShot, pGo);
	}
	ImGui::End();
}

CImGui_CameraToolInspector* CImGui_CameraToolInspector::Create()
{
	CImGui_CameraToolInspector* pInstance = new CImGui_CameraToolInspector();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_CameraToolInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_CameraToolInspector::Free()
{
	Super::Free();
}