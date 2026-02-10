#include "pch.h"
#include "ImGui_GlobalInspector.h"
#include "ImGui_FrustrumLayout.h"
#include "GameObject.h"
#include "GameInstance.h"

CImGui_GlobalInspector::CImGui_GlobalInspector()
	: Super("GlobalInspector", ELayoutType::COUNT)
{
}

HRESULT CImGui_GlobalInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::Frustrum, CImGui_FrustrumLayout::Create())))
		return E_FAIL;

	return S_OK;
}

void CImGui_GlobalInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::Frustrum, nullptr);
	}

	ImGui::End();
}

CImGui_GlobalInspector* CImGui_GlobalInspector::Create()
{
	CImGui_GlobalInspector* pInstance = new CImGui_GlobalInspector();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_GlobalInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_GlobalInspector::Free()
{
	Super::Free();
}
