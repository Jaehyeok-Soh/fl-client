#include "pch.h"
#include "ImGui_TextureDebugInspector.h"
#include "ImGui_ShaderDebugLayout.h"
#include "GameInstance.h"

CImGui_TextureDebugInspector::CImGui_TextureDebugInspector()
	: Super("TextureDebugInspector", ELayoutType::COUNT)
{
	
}

HRESULT CImGui_TextureDebugInspector::Initialize()
{
	if (FAILED(Add_Layout(ELayoutType::ShaderDebug, CImGui_ShaderDebugLayout::Create())))
		return E_FAIL;		

	return S_OK;
}

void CImGui_TextureDebugInspector::Render(CGameObject* pGo)
{
	if (ImGui::Begin(m_strLabel.c_str()))
	{
		Render_Layout(ELayoutType::ShaderDebug, nullptr);
	}
	ImGui::End();
}

CImGui_TextureDebugInspector* CImGui_TextureDebugInspector::Create()
{
	CImGui_TextureDebugInspector* pInstance = new CImGui_TextureDebugInspector();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CImGui_TextureDebugInspector::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CImGui_TextureDebugInspector::Free()
{
	Super::Free();
}
