#pragma once
#include "Base.h"
#include "UIData_Repository.h"

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
	void Add_CanvasData(const CANVAS_DATA& tData);
	void Add_LayerData(const LAYER_DATA& tData);
	void Add_UIData(const GENERIC_UI_DATA& tData);
	void Add_UI(const GENERIC_UI_DATA& tData);

	/* 커서를 옮길 땐 이걸 써주세요. */
	void Change_Canvas(uint32_t iNewCanvasIndex);
	void Change_Layers(uint32_t iNewLayerIndex);
	void Change_UIData(uint32_t iNewUIIndex);

	uint32_t Get_NumCanvas();
	uint32_t Get_NumLayer(uint32_t iCanvasIndex);
	uint32_t Get_CurNumLayer();
	uint32_t Get_NumUI(uint32_t iCanvasIndex, uint32_t iLayerIndex);
	uint32_t Get_CurNumUI();


	/* 데이터를 지울 땐 이걸 써주세요. */
	void Remove_CanvasData();
	void Remove_LayerData();
	void Remove_UIData();

	/* 내부 데이터 빠르게 꺼내오기 */
	vector<CANVAS_DATA>& Get_CurCanvas_Ref() { return m_vecCanvasData; }
	vector<LAYER_DATA>& Get_CurLayers_Ref() { return m_vecCanvasData[m_iCurCanvasIndex].vecLayers; }
	vector<GENERIC_UI_DATA>& Get_CurUIDatas_Ref() { return Get_CurLayers_Ref()[m_iCurLayerIndex].vecUIData; }

	const vector<CANVAS_DATA>& Get_CurCanvas() const { return m_vecCanvasData; }
	const vector<LAYER_DATA>& Get_CurLayers() const { return m_vecCanvasData[m_iCurCanvasIndex].vecLayers; }
	const vector<GENERIC_UI_DATA>& Get_CurUIDatas() const { return Get_CurLayers()[m_iCurLayerIndex].vecUIData; }

	CANVAS_DATA* Get_CanvasData_Ptr(uint32_t CanvasIndex);
	LAYER_DATA*	Get_LayerData_Ptr(uint32_t LayerIndex);
	GENERIC_UI_DATA* Get_UIData_Ptr(uint32_t UIIndex);
	CToolUI* Get_UI_Ptr(uint32_t UIIndex);

	uint32_t Get_CurCanvasIndex() { return m_iCurCanvasIndex; }
	uint32_t Get_CurLayerIndex() { return m_iCurLayerIndex; }
	uint32_t Get_CurUIIndex() { return m_iCurUIIndex; }

#pragma endregion

private:
	/* CANVAS_DATA -> LAYER_DATA -> GENERIC_UI_DATA 각각 벡터를 가지는 계층 구조 입니다 */
	vector<CANVAS_DATA> m_vecCanvasData;

private:
	/* 커서들 입니다. */
	uint32_t m_iCurCanvasIndex = {};
	uint32_t m_iCurLayerIndex = {};
	uint32_t m_iCurUIIndex = {};

public:
	virtual void Free()override;
};

NS_END