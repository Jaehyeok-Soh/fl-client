#include "pch.h"
#include "UI_Hierachy.h"
#include "ToolCanvas.h"
#include "ToolLayer.h"
#include "ToolUI.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "GameInstance.h"

CUI_Hierachy::CUI_Hierachy(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pToolManager);
}

HRESULT CUI_Hierachy::Initialize_Prototype()
{
	return S_OK;
}

void CUI_Hierachy::Update(const _float fTimeDelta)
{

}

HRESULT CUI_Hierachy::Render(CToolObject* pGo)
{
	auto pVecCanvas = CImGui_UIManager::GetInstance()->Safe_Access_CanvasVector();
	if (nullptr == pVecCanvas)
		return S_OK;
	
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);
	static int s_iSelRoot = -1;
	static int s_iSelChild = -1;
	static int s_iSelGrand = -1;
	
	auto SelectRoot = [&](int r)
		{
			s_iSelRoot = r;
			s_iSelChild = -1;
			s_iSelGrand = -1;
		};
	
	auto SelectChild = [&](int r, int c)
		{
			s_iSelRoot = r;
			s_iSelChild = c;
			s_iSelGrand = -1;
		};
	
	auto SelectGrand = [&](int r, int c, int g)
		{
			s_iSelRoot = r;
			s_iSelChild = c;
			s_iSelGrand = g;
		};
	
	auto DrawNode = [&](const char* id, const _string& label, ImGuiTreeNodeFlags extraFlags, bool isSelected)->bool
		{
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_OpenOnArrow |
				extraFlags;
	
			if (isSelected)
				flags |= ImGuiTreeNodeFlags_Selected;
	
			return ImGui::TreeNodeEx(id, flags, "%s", label.c_str());
		};
	
	auto DrawLeaf = [&](const char* id, const _string& label, bool isSelected)
		{
			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_NoTreePushOnOpen;
	
			if (isSelected)
				flags |= ImGuiTreeNodeFlags_Selected;
	
			ImGui::TreeNodeEx(id, flags, "%s", label.c_str());
		};
	
	// 예시 데이터
	
	static vector<_string> vecRoot;
	static vector<vector<_string>> vecChild;
	static vector<vector<vector<_string>>> vecGrand;
	
	vecRoot.clear();
	vecChild.clear();
	vecGrand.clear();
	
	for (auto canvasData : (*pVecCanvas))
	{
		vecRoot.push_back(canvasData->Get_Tag());
	
		// canvas 하나 추가
		vecChild.emplace_back();   // vecChild.back()가 이 캔버스의 레이어 리스트
		vecGrand.emplace_back();   // vecGrand.back()가 이 캔버스의 [레이어][UI] 구조
		
		if (nullptr != canvasData->Safe_Access_LayerObject_Vector_Ptr())
		{
			for (const auto& layerData : *canvasData->Safe_Access_LayerObject_Vector_Ptr())
			{
				// layer 하나 추가
				vecChild.back().push_back(layerData->Get_Name());
				vecGrand.back().emplace_back(); // 이 레이어의 UI 리스트(= vector<_string>)
	
				if (nullptr != layerData->Safe_Access_UIObject_Vector_Ptr())
				{
					for (const auto& uiData : *layerData->Safe_Access_UIObject_Vector_Ptr())
					{
						vecGrand.back().back().push_back(uiData->Get_Name());
					}
				}
			}
		}
	}
	
	for (int r = 0; r < (int)vecRoot.size(); ++r)
	{
		ImGui::PushID(r);
	
		bool openedRoot = DrawNode("##Root", vecRoot[r], 0, (s_iSelRoot == r && s_iSelChild == -1 && s_iSelGrand == -1));
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			SelectRoot(r);
	
		if (openedRoot)
		{
			if (r < (int)vecChild.size())
			{
				for (int c = 0; c < (int)vecChild[r].size(); ++c)
				{
					ImGui::PushID(c);
	
					bool hasGrand = (r < (int)vecGrand.size() && c < (int)vecGrand[r].size() && !vecGrand[r][c].empty());
	
					if (hasGrand)
					{
						bool openedChild = DrawNode("##Child", vecChild[r][c], 0, (s_iSelRoot == r && s_iSelChild == c && s_iSelGrand == -1));
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							SelectChild(r, c);
	
						if (openedChild)
						{
							for (int g = 0; g < (int)vecGrand[r][c].size(); ++g)
							{
								ImGui::PushID(g);
	
								DrawLeaf("##Grand", vecGrand[r][c][g], (s_iSelRoot == r && s_iSelChild == c && s_iSelGrand == g));
								if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
									SelectGrand(r, c, g);
	
								ImGui::PopID();
							}
							ImGui::TreePop();
						}
					}
					else
					{
						DrawLeaf("##ChildLeaf", vecChild[r][c], (s_iSelRoot == r && s_iSelChild == c && s_iSelGrand == -1));
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
							SelectChild(r, c);
					}
					ImGui::PopID();
				}
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
	ImGui::Text("Selected Root=%d, Child=%d, Grand=%d", s_iSelRoot, s_iSelChild, s_iSelGrand);
	
	
	ImGui::Text("Screen Mouse Pos %d, %d", CGameInstance::GetInstance()->Get_MousePos().x, CGameInstance::GetInstance()->Get_MousePos().y);
	ImGui::Text("Viewport Mouse Pos %.2f, %.2f", CImGui_ToolManager::GetInstance()->Get_ViewportMousePos().x, CImGui_ToolManager::GetInstance()->Get_ViewportMousePos().y);
	ImGui::Text("Viewport Size %.2f, %.2f", CImGui_ToolManager::GetInstance()->Get_CurViewportSize().x, CImGui_ToolManager::GetInstance()->Get_CurViewportSize().y);

	auto* pUI = CImGui_UIManager::GetInstance()->Safe_Access_UI(CImGui_UIManager::GetInstance()->Get_CurUIIndex());
	if(nullptr != pUI)
		ImGui::Text("ui Render pos %.2f, %.2f", pUI->Get_RenderPos().x, pUI->Get_RenderPos().y);
	ImGui::End();

	return S_OK;
}


CUI_Hierachy* CUI_Hierachy::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Hierachy* pInstance = new CUI_Hierachy(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Hierachy::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hierachy::Free()
{
	Safe_Release(m_pToolManager);
	Super::Free();
}


