#include "pch.h"
#include "ImGui_PlayerInspector.h"
#include "ImGui_TransformLayout.h"
#include "ImGui_StateLayout.h"
#include "ImGui_FrameLayout.h"
#include "GameObject.h"
#include "GameInstance.h"

CImGui_PlayerInspector::CImGui_PlayerInspector()
	: Super("PlayerInsepctor", ELayoutType::COUNT)
{
}

HRESULT CImGui_PlayerInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::Frame, CImGui_FrameLayout::Create())))
		return E_FAIL;

	if (FAILED(Add_Layout(ELayoutType::Transform, CImGui_TransformLayout::Create())))
		return E_FAIL;

	if (FAILED(Add_Layout(ELayoutType::State, CImGui_StateLayout::Create())))
		return E_FAIL;

	return S_OK;
}

void CImGui_PlayerInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::Frame, nullptr);

		if (pGo == nullptr || pGo->IsDead())
		{
			ImGui::Text("Player is nullptr");
			ImGui::End();
			return;
		}

		Render_Layout(ELayoutType::Transform, pGo);
		Render_Layout(ELayoutType::State, pGo);
	}
	
	ImGui::End();
}

CImGui_PlayerInspector* CImGui_PlayerInspector::Create()
{
	CImGui_PlayerInspector* pInstance = new CImGui_PlayerInspector();
	if(FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_PlayerInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_PlayerInspector::Free()
{
	Super::Free();
}
