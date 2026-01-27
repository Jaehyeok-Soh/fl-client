#pragma once
#include "Base.h"
#include "UIData_Repository.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolUI;

class CImGui_UIManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CImGui_UIManager)

private:
	CImGui_UIManager();
	virtual ~CImGui_UIManager() = default;

public:
#pragma region FUCTION
	/* 데이터를 새로 추가합니다. */
	void Safe_Add_CanvasData(const CANVAS_DATA& tData);
	void Safe_Add_LayerData(const LAYER_DATA& tData);
	void Safe_Add_UIData(const GENERIC_UI_DATA& tData);
	HRESULT  Safe_Add_UI(LAYER_DATA* pLayer, const GENERIC_UI_DATA& tData);

	/* 커서를 옮길 땐 이걸 써주세요. */
	void Safe_Change_Canvas(int32_t iNewCanvasIndex);
	void Safe_Change_Layer(int32_t iNewLayerIndex);
	void Safe_Change_UI(int32_t iNewUIIndex);

	int32_t Get_NumCanvas();
	int32_t Get_NumLayer();
	int32_t Get_NumUI();

	/* 데이터를 지울 땐 이걸 써주세요. */
	void Safe_Remove_CanvasData();
	void Safe_Remove_LayerData();


	int32_t Get_CurCanvasIndex() { return m_iCurCanvasIndex; }
	int32_t Get_CurLayerIndex() { return m_iCurLayerIndex; }
	int32_t Get_CurUIIndex() { return m_iCurUIIndex; }

#pragma endregion

	HRESULT Remake_UIObjects();
	HRESULT Clear_UIObjects();

	vector<CANVAS_DATA>* Safe_Access_CanvasVector();
	CANVAS_DATA* Safe_Access_Canvas(int32_t index);
	vector<LAYER_DATA>* Safe_Access_LayerVector();
	LAYER_DATA* Safe_Access_Layer(int32_t index);
	vector<GENERIC_UI_DATA>* Safe_Access_UIVector();
	GENERIC_UI_DATA* Safe_Access_UI(int32_t index);

	/// <summary>
	/// 내부 데이터 안전을 보장할 수 없음
	/// </summary>
	/// <returns></returns>
	vector<CANVAS_DATA>& Get_CanvasDataVector_Ref() { return m_vecCanvasData; }

private:
	/* CANVAS_DATA -> LAYER_DATA -> GENERIC_UI_DATA 각각 벡터를 가지는 계층 구조 입니다 */
	vector<CANVAS_DATA> m_vecCanvasData;
	CGameInstance* m_pGameInstance = { nullptr };

private:
	/* 커서들 입니다. */
	int32_t m_iCurCanvasIndex = {};
	int32_t m_iCurLayerIndex = {};
	int32_t m_iCurUIIndex = {};

public:
	virtual void Free()override;
};

NS_END