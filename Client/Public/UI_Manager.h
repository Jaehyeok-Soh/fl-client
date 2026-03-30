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
	TUTORIAL_PANNEL_END,	// 튜토리얼 패널 나왔을 때 

	TUTORIAL_POPUP_EVENT1,	// V 표시 서클 다 채워졌을 때	// 내부 이벤트
	TUTORIAL_POPUP_EVENT2,	// 이벤트 다 끝났을 때			// 내부 이벤트

	QUEST_NAME_CHANGE,	// 퀘스트 이름 바꼈을 때 

	PLAYER_LOW_HP,
	PLAYER_NORMAL_HP,

	QTE_SUCCESS,
	END
};

typedef struct tagUIEventesc
{
	EUIEventID eEventID = { EUIEventID::END };

	_uint iParam0 = {};
	_uint iParam1 = {};
	_uint iParam2 = {};

}UIEVENT_DESC;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;
public:
	HRESULT Initialize_UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	HRESULT Regist_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, const _wstring& wstrPrototype, const _wstring& wstrPooltag,const _uint iPrototypeLevel, void* pArg, _uint iNumPrefab);
	void Request_Add_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, _uint iSpawnLevel, void* pArg);
public:
	// UI 전달 변수 Getter Setter
	const _float* Get_LoadingRatio() const { return m_pLoadingRatio; }
	CMulticastDelegate<void(const UIEVENT_DESC&)>& Get_UIEvents() { return m_vEvents; }

	void Set_LoadingRatio(const _float* p) { m_pLoadingRatio = p; }
	
	void Request_LevelChange_With_Fade(UI_LEVEL_FADE_PREFAB_DATA ePrefabData);
	void Clear_LevelChange_Fade_Delay();

	_bool Get_ClearDelay() const { return m_isClearDelay; }
	void Set_ClearDelay(_bool is) { m_isClearDelay = is; }

	_bool	Get_isCursor_Visible() const { return m_isCursorVisible; }
	void	Set_isCursor_Visible(_bool is) { m_isCursorVisible = is; }

private:
	CGameInstance* m_pGameInstance = { nullptr };
	array<_wstring, ENUM_TO_UINT(EUIPrefabType::END)> m_vecPrefabs;

	CMulticastDelegate<void(const UIEVENT_DESC&)> m_vEvents = {};

private:
	class CUITutorial_Manager* m_pTutorialManager = { nullptr };

private:
	// UI 전달 변수 
	const _float* m_pLoadingRatio = { nullptr };

	_bool m_isClearDelay = { false };
	_bool m_isCursorVisible = { false };
public:
	virtual void Free()override;

};

NS_END