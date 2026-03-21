#pragma once
#include "GameObject.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIPrefab;

enum class EUITutorialEventID {

	END
};

typedef struct tagUITutorialEventesc
{
	EUITutorialEventID  eEventID = { EUITutorialEventID::END };

	_uint iParam0 = {};

}UI_TUTORIAL_EVENT_DESC;

class CUITutorial_Manager final : public CBase
{
	DECLARE_SINGLETON(CUITutorial_Manager)

	using Super = CBase;
private:
	CUITutorial_Manager();
	virtual ~CUITutorial_Manager() = default;
public:
	void Initialize();
	void Tutorial_Update(const _float fTimeDelta);

	void Set_Current_Tutorial_Step(EUITutorialTypeToPlayerState eState);

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CMulticastDelegate<void(const UI_TUTORIAL_EVENT_DESC&)> m_vTutorialEvents = {};

	EUITutorialTypeToPlayerState m_eTutorialToPlayerState = { EUITutorialTypeToPlayerState ::END};
	_bool m_isChangeState = { false };

public:
	virtual void Free()override;
};

NS_END