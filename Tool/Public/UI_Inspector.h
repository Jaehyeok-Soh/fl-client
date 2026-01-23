#pragma once
#include "ImGui_Panel.h"
#include "UIData_Repository.h"

NS_BEGIN(Tool)
class CImGui_ToolManager;
class CImGui_UIManager;

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
	void Create_Canvas();
	void Edit_Canvas();

	/* 캔버스에 들어갈 레이어를 생성 */

	/* 레이어에 들어갈 UI를 생성 */
	/* UI 타입 지정 -> 버튼인지, 이미지인지, 영상인지 등 */
	/* UI 텍스쳐를 지정 -> 파일 패스를 저장해야될듯 */
	/* UI에 붙일 기능을 지정 -> 기능들은 ENUM으로 */


private:
	void Input_Canvas_Tag();
	void Input_Canvas_TransformInfo();

	/* 버튼 */
	void Create_Canvas_Btn();
	void Setting_Canvas_CustomSize_Btn();
	void Setting_Canvas_ViewportSize_Btn();
	
private:
	CImGui_ToolManager* m_pToolManager = { nullptr };
	CImGui_UIManager* m_pUIManager = { nullptr };

	ImGuiWindowFlags m_Flag = {};

	vector<_string> m_vecClientLevelType; 
	const _char* m_szArrClientLevelType[g_iClientLevelType_Count];

	_bool m_isCreateCanvas	= { FALSE };
	_bool m_isCustomSize	= { FALSE };
	_bool m_isViewportSize	= { FALSE };
	_bool m_isEditCanvas = { FALSE };

	_string m_strCurEditor_CanvasTag = {};

private:
	int32_t m_iCurSelectLevelID = {};
	vector<_string> m_vecLayers;

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END