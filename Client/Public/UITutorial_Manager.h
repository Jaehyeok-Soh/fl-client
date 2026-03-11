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

class CUITutorial_Manager final : public CGameObject
{
	using Super = CGameObject;
private:
	CUITutorial_Manager();
	virtual ~CUITutorial_Manager() = default;
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	HRESULT Awake(const _uint iCurrentLevelID) override;
	CGameObject* Clone(void* pArg) override;

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CMulticastDelegate<void(const UI_TUTORIAL_EVENT_DESC&)> m_vTutorialEvents = {};

	

public:
	static CUITutorial_Manager* Create();
	virtual void Free()override;

};

NS_END