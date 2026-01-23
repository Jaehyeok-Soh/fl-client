#pragma once
#include "ImGui_Panel.h"

NS_BEGIN(Tool)
typedef struct tagCanvasDesc CANVAS_DESC;

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

	/* 플로우 함수 */
private:
	void SetUp_Level();
	void SetUp_Canvas();
	void Input_Canvas_Tag();
	void Input_Canvas_TransformInfo();

	void SetUp_UIType();

	/* 기능 */
private:
	void Create_Canvas();
	void Setting_Canvas_CustomSize();
	void Setting_Canvas_ViewportSize();
	
	uint32_t TagToIndex(const _string& Tag);

private:
	CImGui_ToolManager* m_pToolManager = { nullptr };

	ImGuiWindowFlags m_Flag = {};

	vector<_string> m_vecClientLevelType; 
	const _char* m_szArrClientLevelType[g_iClientLevelType_Count];

	_bool m_isCreateCanvas	= { FALSE };
	_bool m_isCustomSize	= { FALSE };
	_bool m_isViewportSize	= { FALSE };

	_string m_strCurEditor_CanvasTag = {};
	uint32_t m_iCurEditor_CanvasIndex = {};

	vector<CANVAS_DESC> m_vecEditor_CanvasInfo;

private:
	int32_t m_iCurSelectLevelID = {};

	vector<CANVAS_DESC> m_vecCanvasInfo;
	vector<_string> m_vecLayers;

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

typedef struct tagCanvasDesc
{
	_string strTag;
	_bool isUsingViewport;

	int32_t iWidth;
	int32_t iHeight;
	int32_t iPosX;
	int32_t iPosY;
	int32_t iPosZ;

}CANVAS_DESC;

NS_END