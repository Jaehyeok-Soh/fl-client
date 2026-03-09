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
	MENU_ENTER_ICON_HOVER_ENTER, 
	MENU_ENTER_ICON_HOVER_EXIT, 
	MENU_OPEN, 
	MENU_CLOSE, 
	MENU_ICON_HOVER_ENTER,
	MENU_ICON_HOVER_EXIT,

	BOSS_ACTION_FIN,

	WEAKNESS_FIN, // 약점대비 이벤트가 끝났을 때 다같이 없애려고 
	
	TUTORIAL_PANNEL_START,	// 튜토리얼 패널 나왔을 때 

	END
};

typedef struct tagUIEventesc
{
	EUIEventID eEventID = { EUIEventID::END };

	_uint iParam0 = {};

}UIEVENT_DESC;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;
public:
	HRESULT Regist_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, const _wstring& wstrPrototype, const _wstring& wstrPooltag,const _uint iPrototypeLevel, void* pArg, _uint iNumPrefab);
	void Request_Add_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, _uint iSpawnLevel, void* pArg);
public:
	// UI 전달 변수 Getter Setter
	const _float* Get_LoadingRatio() const { return m_pLoadingRatio; }
	CMulticastDelegate<void(const UIEVENT_DESC&)>& Get_UIEvents() { return m_vEvents; }

	void Set_LoadingRatio(const _float* p) { m_pLoadingRatio = p; }
	
private:
	CGameInstance* m_pGameInstance = { nullptr };
	array<_wstring, ENUM_TO_UINT(EUIPrefabType::END)> m_vecPrefabs;

	CMulticastDelegate<void(const UIEVENT_DESC&)> m_vEvents = {};

private:
	// UI 전달 변수 
	const _float* m_pLoadingRatio = { nullptr };

public:
	virtual void Free()override;

};

NS_END