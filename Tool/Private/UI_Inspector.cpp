#include "UI_Inspector.h"

CUI_Inspector::CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext)
{
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
	Super::Free();
}


