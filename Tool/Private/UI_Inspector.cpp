#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"

CUI_Inspector::CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pToolManager);
}

HRESULT CUI_Inspector::Initialize_Prototype()
{
	m_vecClientLevelType.resize(g_iClientLevelType_Count);

	for (size_t i = 0; i < g_iClientLevelType_Count; ++i)
	{
		m_vecClientLevelType[i] = (ClientleveltypeToString(static_cast<EClientLevelType>(i)));
		m_szArrClientLevelType[i] = m_vecClientLevelType[i].c_str();
	}

	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{
}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);
	
	SetUp_Level();
	
	SetUp_Canvas();

	ImGui::End();
	return S_OK;
}

void CUI_Inspector::SetUp_Level()
{
	ImGui::Text("<<SetUp Level>>");
	ImGui::Combo("Current_Selected_Level", &m_iCurSelectLevelID, m_szArrClientLevelType, m_vecClientLevelType.size());
}

void CUI_Inspector::SetUp_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Canvas>>");

	Button_Create_Canvas_CustomSize();
	ImGui::SameLine();
	Button_Create_Canvas_ViewportSize();

	Calc_CanvasSize();

}

void CUI_Inspector::Calc_CanvasSize()
{
	/* 내맘대로 만들겠다 */
	if (m_isCustomSize)
	{
		/* Width / Height */
		ImGui::TextUnformatted("Width :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeX", ImGuiDataType_S32, &m_iCanvasSizeX);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Height :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeY", ImGuiDataType_S32, &m_iCanvasSizeY);

		/* Pos X / Y / Z */
		ImGui::TextUnformatted("X :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosX", ImGuiDataType_S32, &m_iCanvasPosX);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Y :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosY", ImGuiDataType_S32, &m_iCanvasPosY);

		ImGui::SameLine(0.f, 16.f);
		
		ImGui::TextUnformatted("Z :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosZ", ImGuiDataType_S32, &m_iCanvasPosZ);

		m_iEditor_CanvasSizeX = m_iCanvasSizeX;
		m_iEditor_CanvasSizeY = m_iCanvasSizeY;

		m_iEditor_CanvasPosX = m_iCanvasPosX;
		m_iEditor_CanvasPosY = m_iCanvasPosY;
		m_iEditor_CanvasPosZ = m_iCanvasPosZ;

	}
	/* 뷰포트 기준으로 캔버스를 만들겠다 */
	else if (m_isViewportSize)
	{
		m_iCanvasSizeX = 0;
		m_iCanvasSizeY = 0;
		m_iCanvasPosX = 0;
		m_iCanvasPosY = 0;
		m_iCanvasPosZ = 0;

		m_iEditor_CanvasPosX = 0;
		m_iEditor_CanvasPosY = 0;
		m_iEditor_CanvasPosZ = 0;
		m_iEditor_CanvasSizeX = m_pToolManager->Get_CurViewportSize().x;
		m_iEditor_CanvasSizeY = m_pToolManager->Get_CurViewportSize().y;

	}
	/* */
	else
	{
		m_iEditor_CanvasSizeX = 0;
		m_iEditor_CanvasSizeY = 0;

		m_iEditor_CanvasPosX = 0;
		m_iEditor_CanvasPosY = 0;
		m_iEditor_CanvasPosZ = 0;
	}
}

void CUI_Inspector::Button_Create_Canvas_CustomSize()
{
	if (ImGui::Button("Create_CustomSize"))
	{
		m_isCustomSize = TRUE;
		m_isViewportSize = FALSE;

		m_isUsingViewport = FALSE;
	}
}

void CUI_Inspector::Button_Create_Canvas_ViewportSize()
{
	if (ImGui::Button("Create_ViewportSize"))
	{
		m_isCustomSize = FALSE;
		m_isViewportSize = TRUE;

		m_isUsingViewport = TRUE;
	}
}

CUI_Inspector* CUI_Inspector::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Inspector* pInstance = new CUI_Inspector(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Inspector::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Inspector::Free()
{
	Safe_Release(m_pToolManager);
	Super::Free();
}


