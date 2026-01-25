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


	void Load_Data();
	void Clear_Data();
	void SetUp_Level();
	void Make_Canvas();
	void Edit_Canvas();

	/* 캔버스에 들어갈 레이어를 생성 */
	void Make_Layers();
	void Edit_Layers();

	/* 레이어에 들어갈 UI를 생성 */
	void Add_NewUI();
	void Edit_UI();

	void SetUp_UI_Common_Info();
	/* UI 타입 지정 -> 버튼인지, 이미지인지, 영상인지 등 */
	/* UI 텍스쳐를 지정 -> 파일 패스를 저장해야될듯 */
	/* UI에 붙일 기능을 지정 -> 기능들은 ENUM으로 */

	void Show_UI_List();

private:
	void Input_Canvas_Tag();
	void Input_Canvas_TransformInfo();
	void Input_Layer_Tag();

	void Input_RectTransform();

	/// <summary>
	/// 드래그(스크럽) 방식으로 실수 값을 편집하는 ImGui용 유틸리티 함수.
	/// </summary>
	/// <param name="label">			: 화면에 표시될 텍스트 레이블(중복O).</param>
	/// <param name="Id">				: 내부 상태 구분용 아이디(중복X).</param>
	/// <param name="pValue">			: 입력 및 출력으로 사용되는 실수 포인터(OUT). 현재 값을 전달하고 함수 호출로 변경된 값으로 반환.</param>
	/// <param name="fValuePerPixel">	: 픽셀 이동당 값 변화량(민감도). 기본값 0.01f.</param>
	/// <param name="fScale">			: Ctrl 누른 상태에서, 드래그시 fScale 배수 크게 이동. 기본값 10.0f.</param>
	/// <param name="fStep">			: +, - 버튼 눌렀을 때 얼마나 바뀔지 정하는 값입니다. 기본값 0.1f.</param>
	/// <param name="fStep_fast">		: Ctrl 누른 상태에서, +, - 버튼 눌렀을 때 얼마나 바뀔지 정하는 값입니다. 기본값 1.0f.</param>
	/// <param name="fSize">			: 스크럽 영역의 시각적 크기(픽셀). 기본값 100.f.</param>
	/// <returns>						값이 변경되면 true를 반환하고, 변경이 없으면 false를 반환합니다.</returns>
	_bool Scrub_Float(const _char* label, const _char* Id, OUT _float* pValue,
		float fValuePerPixel = 0.01f,float fScale = 10.f, float fStep = 0.1f, float fStep_fast = 1.0f, float fSize = 100.f);

	/* 버튼 */
	void Make_Canvas_Btn();
	void Setting_Canvas_CustomSize_Btn();
	void Setting_Canvas_ViewportSize_Btn();
	
	void Make_Layer_Btn();
	void Make_UI_Btn();

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

	_bool m_isCreateLayer = { FALSE };

	_bool m_isCreateUI = { FALSE };

	_string m_strCanvasTag = {};
	_string m_strLayerTag = {};
	_string m_strUIName = {};

	int32_t m_iRectTransformIndex = {4};

private:
	int32_t m_iCurSelectLevelID = {};
	vector<_string> m_vecLayers;

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END