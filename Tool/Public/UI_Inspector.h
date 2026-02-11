#pragma once
#include "ImGui_Panel.h"
#include "DataStruct_UI.h"

NS_BEGIN(Tool)
class CImGui_ToolManager;
class CImGui_UIManager;
class CToolUI;

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
	
	void SetUp_Public_Info();
	void Input_RectTransform();
	void Input_TextureTag();
	void SetUp_Class();
	void SetUp_Owner();

	void SetUp_ShaderPass();
	void SetUp_UIProgress();
	void SetUp_TextData();
	void SetUp_TriggerData();

private:
	/// <summary>
	/// 드래그(스크럽) 방식으로 실수 값을 편집하는 ImGui용 유틸리티 함수.
	/// </summary>
	/// <param name="label">			: 화면에 표시될 텍스트 레이블 !절대 ""를 넣지 말 것!</param>
	/// <param name="Id">				: 내부 상태 구분용 아이디(중복X).</param>
	/// <param name="pValue">			: 입력 및 출력으로 사용되는 실수 포인터(OUT). 현재 값을 전달하고 함수 호출로 변경된 값으로 반환.</param>
	/// <param name="fValuePerPixel">	: 픽셀 이동당 값 변화량(민감도). 기본값 0.01f.</param>
	/// <param name="fScale">			: Ctrl 누른 상태에서, 드래그시 fScale 배수 크게 이동. 기본값 10.0f.</param>
	/// <param name="fStep">			: +, - 버튼 눌렀을 때 얼마나 바뀔지 정하는 값입니다. 기본값 0.1f.</param>
	/// <param name="fStep_fast">		: Ctrl 누른 상태에서, +, - 버튼 눌렀을 때 얼마나 바뀔지 정하는 값입니다. 기본값 1.0f.</param>
	/// <param name="fSize">			: 스크럽 영역의 시각적 크기(픽셀). 기본값 100.f.</param>
	/// <returns>						값이 변경되면 true를 반환하고, 변경이 없으면 false를 반환합니다.</returns>
	_bool Scrub_Float(const _char* label, const _char* Id, OUT _float* pValue,
		float fValuePerPixel = 0.01f, float fScale = 10.f, float fStep = 0.1f, float fStep_fast = 1.0f, float fSize = 100.f);

	bool Begin_Card(const char* Label, const char* ID, float fHeight);
	void End_Card();
private:
	CImGui_ToolManager* m_pToolManager = { nullptr };
	CImGui_UIManager* m_pUIManager = { nullptr };
	ImGuiWindowFlags m_Flag = {};
	vector<_string> m_vecClientLevelType;
	const _char* m_szArrClientLevelType[g_iClientLevelType_Count];

	CToolUI* m_pSelectedUI = { nullptr };
	uint32_t m_iRectTransformIndex = {};


	std::vector<_string> m_VecClassTag;
	std::vector<_string> m_VecOwnerTag;
	std::vector<_string> m_VecShaderPassTag;

	// Trigger Values
	_bool m_isHoverEnter	= {FALSE};
	_bool m_isHoverExit		= {FALSE};
	_bool m_isPressEnter	= {FALSE};
	_bool m_isPressExit		= {FALSE};
	_string m_strTriggerUIName_UserInput = {};
	_string m_strTriggerUIName = {};
	_string m_strTriggerCanvasName = {};

	ImVec2 m_vLastCardPos = {};
	ImVec2 m_vLastCardSize = {};


	













private:
	// ======== ImGui 값이 변동 됐다는걸 알리는 변수 ========
	string              m_strSelectedFolder = "";
private:
	//  =======  폴더명 & 파일명을 대신할 Mesh 전용 vector 컨테이너
	std::map<string, std::vector<std::pair<string/*path*/, string/*Name*/>>> m_TextureMap;
	std::vector<string> m_TextureFolderNames;

	std::vector<std::pair<string/*Path*/, string/*Name*/>> m_TextureFileNames;

public:
	static CUI_Inspector* Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

NS_END