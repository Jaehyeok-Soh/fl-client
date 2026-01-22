#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)

class CImGui_ToolManager;

class CUI_Inspector final : public CImGui_Panel
{
	using Super = CImGui_Panel;
private:
	explicit CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CUI_Inspector() = default;

public:	
	HRESULT Initialize_Prototype();

	virtual void Update(const _float fTimeDelta)override;
	virtual HRESULT Render(CToolObject* pGo) override;

private:
	void SetUp_Level();
	void SetUp_Canvas();
	void Calc_CanvasSize();

private:
	void Button_Create_Canvas_CustomSize();
	void Button_Create_Canvas_ViewportSize();
	
	/* 내부 변수 */
private:
	CImGui_ToolManager* m_pToolManager = { nullptr };

	ImGuiWindowFlags m_Flag = {};

	vector<_string> m_vecClientLevelType; 
	const _char* m_szArrClientLevelType[g_iClientLevelType_Count];

	_bool m_isCustomSize = { FALSE };
	_bool m_isViewportSize = { FALSE };

	/* 에디터에서 보여줄 캔버스 사이즈 */
	int32_t m_iEditor_CanvasSizeX = {};
	int32_t m_iEditor_CanvasSizeY = {};
	int32_t m_iEditor_CanvasPosX = {};
	int32_t m_iEditor_CanvasPosY = {};
	int32_t m_iEditor_CanvasPosZ = {};

	/* 파싱용 데이터 */
private:
	/* 현재 선택 된 UI가 어떤 레벨에 배치 될 것 인지*/
	int32_t m_iCurSelectLevelID = {};

	/* 만들어질 캔버스 사이즈 */
	_bool m_isUsingViewport = { FALSE };
	int32_t m_iCanvasSizeX = {};
	int32_t m_iCanvasSizeY = {};
	int32_t m_iCanvasPosX = {};
	int32_t m_iCanvasPosY = {};
	int32_t m_iCanvasPosZ = {};

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END