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
	
	Create_Canvas();

	if (!m_vecEditor_CanvasInfo.empty())
		Edit_Canvas();

	ImGui::End();
	return S_OK;
}

void CUI_Inspector::SetUp_Level()
{
	ImGui::Text("<<SetUp Level>>");
	ImGui::Combo("Current_Selected_Level", &m_iCurSelectLevelID, m_szArrClientLevelType, m_vecClientLevelType.size());
}

void CUI_Inspector::Create_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Canvas>>");

	Input_Canvas_Tag();
	Create_Canvas_Btn();

	/* Create 버튼을 눌렀다면 */
	if (m_isCreateCanvas)
	{
		if (m_strCurEditor_CanvasTag == "")
		{
			MSG_BOX("Canvas Tag Empty");
			m_isCreateCanvas = FALSE;
			return;
		}

		/* 현재 Editor에 있는 CanvasTag를 순회 */
		for (const auto& Desc : m_vecEditor_CanvasInfo)
		{
			/* 만약 CurTag가 vector에 있다면 */
			if (Desc.strTag == m_strCurEditor_CanvasTag)
			{
				MSG_BOX("Canvas Tag Already Created!");
				m_isCreateCanvas = FALSE;
				break;
			}
		}

		/* 위에 루프를 돌고 왔는데 m_isCreateCanvas가 FALSE가 아니라면 -> Tag가 없다는 뜻이므로 */
		if (m_isCreateCanvas)
		{
			CANVAS_DESC tDesc = {};
			tDesc.strTag = m_strCurEditor_CanvasTag;
			m_vecEditor_CanvasTag.push_back(m_strCurEditor_CanvasTag);

			m_vecEditor_CanvasInfo.push_back(tDesc);
			m_vecCanvasInfo.push_back(tDesc);

			m_iCurEditor_CanvasIndex = TagToIndex(m_strCurEditor_CanvasTag);
			
			m_strCurEditor_CanvasTag = "";
			m_isCreateCanvas = FALSE;
			m_isEditCanvas = TRUE;
		}
	}
}

void CUI_Inspector::Edit_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Select CanvasTag To Edit>>");
	ImGui::BeginChild("CanvasList", ImVec2(0, 100), true);
	for (int i = 0; i < (int)m_vecEditor_CanvasTag.size(); ++i)
	{
		bool selected = (m_iCurEditor_CanvasIndex == i);
		if (ImGui::Selectable(m_vecEditor_CanvasTag[i].c_str(), selected))
			m_iCurEditor_CanvasIndex = i;
	}
	ImGui::EndChild();

	Setting_Canvas_CustomSize_Btn();
	ImGui::SameLine();
	Setting_Canvas_ViewportSize_Btn();
	Input_Canvas_TransformInfo();

	if (ImGui::Button("Delete Selected Canvas"))
	{
		m_vecEditor_CanvasInfo.erase(m_vecEditor_CanvasInfo.begin() + m_iCurEditor_CanvasIndex);
		m_vecEditor_CanvasTag.erase(m_vecEditor_CanvasTag.begin() + m_iCurEditor_CanvasIndex);
		m_iCurEditor_CanvasIndex = 0;
	}
}

void CUI_Inspector::Input_Canvas_Tag()
{
	ImGui::TextUnformatted("CanvasTag :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##CanvasTag", &m_strCurEditor_CanvasTag);
}

void CUI_Inspector::Input_Canvas_TransformInfo()
{
	/* 내맘대로 만들겠다 */
	if (m_isCustomSize)
	{
		/* Width / Height */
		ImGui::TextUnformatted("Width :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeX", ImGuiDataType_S32, &m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iWidth);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Height :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeY", ImGuiDataType_S32, &m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iHeight);

		/* Pos X / Y / Z */
		ImGui::TextUnformatted("X :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosX", ImGuiDataType_S32, & m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosX);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Y :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosY", ImGuiDataType_S32, &m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosY);

		ImGui::SameLine(0.f, 16.f);
		
		ImGui::TextUnformatted("Z :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosZ", ImGuiDataType_S32, &m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosZ);
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].isUsingViewport = FALSE;

		m_vecCanvasInfo[m_iCurEditor_CanvasIndex] = m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex];

	}
	/* 뷰포트 기준으로 캔버스를 만들겠다 */
	else if (m_isViewportSize)
	{
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iWidth = 0;
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iHeight = 0;
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosX = 0;
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosY = 0;
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].iPosZ = 0;
		m_vecCanvasInfo[m_iCurEditor_CanvasIndex].isUsingViewport = TRUE;

		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].iWidth = m_pToolManager->Get_CurViewportSize().x;
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].iHeight = m_pToolManager->Get_CurViewportSize().y;
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].iPosX = 0;
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].iPosY = 0;
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].iPosZ = 0;
		m_vecEditor_CanvasInfo[m_iCurEditor_CanvasIndex].isUsingViewport = TRUE;
	}
}

void CUI_Inspector::Create_Canvas_Btn()
{
	if (ImGui::Button("Create With This Tag"))
		m_isCreateCanvas = TRUE;
}

void CUI_Inspector::Setting_Canvas_CustomSize_Btn()
{
	if (ImGui::Button("Setting By CustomSize"))
	{
		m_isCustomSize = TRUE;
		m_isViewportSize = FALSE;

	}
}

void CUI_Inspector::Setting_Canvas_ViewportSize_Btn()
{
	if (ImGui::Button("Setting By ViewportSize"))
	{
		m_isCustomSize = FALSE;
		m_isViewportSize = TRUE;
	}
}

uint32_t CUI_Inspector::TagToIndex(const _string& Tag)
{
	uint32_t index = {0};
	for (const auto& Desc : m_vecCanvasInfo)
	{
		/* 만약 Tag가 vector에 있다면 */
		if (Desc.strTag == Tag)
			return index;
		++index;
	}
	/* 없으면 현재 인덱스를 반환 */
	return m_iCurEditor_CanvasIndex;
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


