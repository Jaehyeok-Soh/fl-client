#include "pch.h"
#include "ImGui_PoolInspector.h"
#include "ImGui_ObjectPoolLayout.h"
#include "GameInstance.h"

CImGui_PoolInspector::CImGui_PoolInspector()
	: Super("PoolInspector", ELayoutType::COUNT)
{
}

HRESULT CImGui_PoolInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::Object, CImGui_ObjectPoolLayout::Create())))
		return E_FAIL;

	return S_OK;
}

void CImGui_PoolInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::Object, nullptr);
	}

	ImGui::End();
}

CImGui_PoolInspector* CImGui_PoolInspector::Create()
{
	CImGui_PoolInspector* pInstance = new CImGui_PoolInspector();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_PoolInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_PoolInspector::Free()
{
	Super::Free();
}
