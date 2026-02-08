#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)

class CToolCanvas;
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

	void Safe_Change_Canvas(int32_t iNewCanvasIndex);
	void Safe_Change_UI(int32_t iNewUIIndex);

	int32_t Get_NumCanvas();
	int32_t Get_NumUI();

	int32_t Get_CurCanvasIndex() { return m_iCurCanvasIndex; }
	int32_t Get_CurUIIndex() { return m_iCurUIIndex; }

	vector<CToolCanvas*>*	Safe_Access_CanvasVector() { if (m_vecCanvas.empty())return nullptr; return &m_vecCanvas; };
	CToolCanvas*	Safe_Access_Canvas(int32_t index);

	vector<CToolUI*>* Safe_Access_UIVector();
	CToolUI*		Safe_Access_UI(int32_t index);

	void Move_CanvasCache(map<_string, CToolCanvas*> CanvasCache) { m_MapCanvasCache = std::move(CanvasCache); }
	void Move_UICache(map<_string, CToolUI*> ToolCache) { m_MapUICache = std::move(ToolCache); }

	HRESULT Safe_Add_CanvasCache(const _string& strTag, CToolCanvas* pCache);
	HRESULT Safe_Add_UICache(const _string& strTag, CToolUI* pCache);

	CToolCanvas* Find_Canvas(const _string& strTag);
	CToolUI* Find_UI(const _string& strTag);

	void Clear();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	vector<CToolCanvas*> m_vecCanvas;

	map<_string, CToolCanvas*> m_MapCanvasCache;
	map<_string, CToolUI*> m_MapUICache;

private:
	int32_t m_iCurCanvasIndex = {};
	int32_t m_iCurUIIndex = {};

public:
	virtual void Free()override;
};

NS_END