#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
#pragma pop_macro("new")

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	/* Builder에 MapCache를 넘길거임 */
	HRESULT Swap_MapCanvasCache(uint32_t iLevelIndex, unordered_map<_string, CCanvas*>&& Cache) { if (iLevelIndex >= g_iLevelType_Count)return E_FAIL; m_mapCanvasCache[iLevelIndex].swap(Cache);return S_OK; }
	HRESULT Swap_MapGenericUICache(uint32_t iLevelIndex, unordered_map<_string, CGenericUI*>&& Cache) { if (iLevelIndex >= g_iLevelType_Count)return E_FAIL; m_mapUICache[iLevelIndex].swap(Cache); return S_OK; }

	/* Builder에서 만들면서 넣어줄거임 */
	HRESULT Add_VecCanvasCache(uint32_t iLevelIndex, CCanvas* pCache) { if (iLevelIndex >= g_iLevelType_Count)return E_FAIL; m_vecCanvasCache[iLevelIndex].push_back(pCache); return S_OK; }
	HRESULT Add_VecGenericUICache(uint32_t iLevelIndex, CGenericUI* pCache) { if (iLevelIndex >= g_iLevelType_Count)return E_FAIL; m_vecGenericUICache[iLevelIndex].push_back(pCache); return S_OK; }

	/* 특정 UI 오브젝트를 찾아야 할 때 */
	CCanvas* Find_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag);
	CGenericUI* Find_GenericUI(uint32_t iLevelIndex, const _string& strUITag);

	/* 특정 UI 오브젝트가 보관하고 있는 UI 오브젝트들을 가져올 때 */
	vector<CGenericUI*>* Find_GenericUI_Vector(uint32_t iLevelIndex, const _string& strUILayerTag);

	/* 레벨에 존재하는 모든 UI 오브젝트를 가져올 때 */
	vector<CCanvas*>* Get_Level_All_Canvas(uint32_t iLevelIndex);
	vector<CGenericUI*>* Get_Level_All_GenericUI(uint32_t iLevelIndex);

	void Add_RenderGroup(uint32_t iLevelIndex);
	void Request_SortUI();
	void Clear_Cache(uint32_t iLevelIndex);

private:
	void Sort_UI(vector<CGenericUI*>& Target);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	/* 특정 오브젝트를 딱 찝어서 이벤트를 발생 시켜야 할 때 */
	array<unordered_map<_string, CCanvas*>,		g_iLevelType_Count >m_mapCanvasCache;
	array<unordered_map<_string, CGenericUI*>,	g_iLevelType_Count >m_mapUICache;

	/* 레벨에 같은 타입의 모든 오브젝트들에게 이벤트를 발생시킬 때 */
	array<vector<CCanvas*>, g_iLevelType_Count> m_vecCanvasCache;
	array<vector<CGenericUI*>, g_iLevelType_Count> m_vecGenericUICache;

	vector<CGenericUI*> m_vecSortUI;
	_bool m_isSort = { FALSE };

public:
	virtual void Free()override;

};

NS_END