#pragma once
#include "Base.h"
#include "UIData_Repository.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolCanvas;
class CToolLayer;
class CToolUI;

class CImGui_UIManager final : public CBase
{
	using Super = CBase;
	DECLARE_SINGLETON(CImGui_UIManager)

private:
	CImGui_UIManager();
	virtual ~CImGui_UIManager() = default;

public:
	HRESULT Safe_Add_Canvas(CToolCanvas* pCanvas);

	/* 커서를 옮길 땐 이걸 써주세요. */
	void Safe_Change_Canvas(int32_t iNewCanvasIndex);
	void Safe_Change_Layer(int32_t iNewLayerIndex);
	void Safe_Change_UI(int32_t iNewUIIndex);

	int32_t Get_NumCanvas();
	int32_t Get_NumLayer();
	int32_t Get_NumUI();

	int32_t Get_CurCanvasIndex() { return m_iCurCanvasIndex; }
	int32_t Get_CurLayerIndex() { return m_iCurLayerIndex; }
	int32_t Get_CurUIIndex() { return m_iCurUIIndex; }


	/* 안전하게 원소에 접근하는 함수들 / nullptr 체크 필수 */
	vector<CToolCanvas*>*	Safe_Access_CanvasVector() { if (m_vecCanvas.empty())return nullptr; return &m_vecCanvas; };
	CToolCanvas*	Safe_Access_Canvas(int32_t index);

	vector<CToolLayer*>* Safe_Access_LayerVector();
	CToolLayer*		Safe_Access_Layer(int32_t index);

	vector<CToolUI*>* Safe_Access_UIVector();
	CToolUI*		Safe_Access_UI(int32_t index);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CToolCanvas*> m_vecCanvas;
private:
	int32_t m_iCurCanvasIndex = {};
	int32_t m_iCurLayerIndex = {};
	int32_t m_iCurUIIndex = {};

public:
	virtual void Free()override;
};

NS_END