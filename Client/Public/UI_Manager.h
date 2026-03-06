#pragma once
#include "Base.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUITrigger;
class CUIPrefab;

enum class EUIEventID {
	MENU_OPEN, MENU_CLOSE, END
};

typedef struct tagUIEventesc
{
	EUIEventID eEventID = { EUIEventID::END };

}UIEVENT_DESC;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:



private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;
public:
	/* Builder에서 만들면서 넣어줄거임 */
	HRESULT Add_VecCanvasCache(uint32_t iLevelIndex, CCanvas* pCache);
	HRESULT Add_VecGenericUICache(uint32_t iLevelIndex, CGenericUI* pCache);

	/* Builder에 MapCache를 넘길거임 (누적 Merge) */
	HRESULT Merge_MapCanvasCache(uint32_t iLevelIndex, unordered_map<_string, CCanvas*>&& Cache);
	HRESULT Merge_MapGenericUICache(uint32_t iLevelIndex, unordered_map<_string, CGenericUI*>&& Cache);

	/* 특정 UI 오브젝트를 찾아야 할 때 */
	CCanvas* Find_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag);
	CGenericUI* Find_GenericUI(uint32_t iLevelIndex, const _string& strUITag);

	/* 특정 UI 오브젝트가 보관하고 있는 UI 오브젝트들을 가져올 때 */
	vector<CGenericUI*>* Find_GenericUI_Vector(uint32_t iLevelIndex, const _string& strUILayerTag);

	/* 레벨에 존재하는 모든 UI 오브젝트를 가져올 때 */
	vector<CCanvas*>* Get_Level_All_Canvas(uint32_t iLevelIndex);
	vector<CGenericUI*>* Get_Level_All_GenericUI(uint32_t iLevelIndex);

	void Clear_Cache(uint32_t iLevelIndex);
	
	HRESULT Regist_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, const _wstring& wstrPrototype, const _wstring& wstrPooltag,const _uint iPrototypeLevel, void* pArg, _uint iNumPrefab);
	void Request_Add_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, _uint iSpawnLevel, void* pArg);

	void Request_Clear();
	void Request_Clear_DeadUI();

public:
	// UI 전달 변수 Getter Setter
	const _float* Get_LoadingRatio() const { return m_pLoadingRatio; }
	CMulticastDelegate<void(const UIEVENT_DESC&)>& Get_UIEvents() { return m_vEvents; }

	void Set_LoadingRatio(const _float* p) { m_pLoadingRatio = p; }
	
private:
	CGameInstance* m_pGameInstance = { nullptr };
	/* 특정 오브젝트를 딱 찝어서 이벤트를 발생 시켜야 할 때 */
	array<unordered_map<_string, CCanvas*>,		g_iLevelType_Count >m_mapCanvasCache;
	array<unordered_map<_string, CGenericUI*>,	g_iLevelType_Count >m_mapUICache;
	/* 레벨에 같은 타입의 모든 오브젝트들에게 이벤트를 발생시킬 때 */
	array<vector<CCanvas*>, g_iLevelType_Count> m_vecCanvasCache;
	array<vector<CGenericUI*>, g_iLevelType_Count> m_vecGenericUICache;
	_bool m_isClear = {false};
	_bool m_isDeadUIClear = {false};

	array<_wstring, ENUM_TO_UINT(EUIPrefabType::END)> m_vecPrefabs;

	CMulticastDelegate<void(const UIEVENT_DESC&)> m_vEvents = {};

private:
	// UI 전달 변수 
	const _float* m_pLoadingRatio = { nullptr };

public:
	virtual void Free()override;

};

NS_END