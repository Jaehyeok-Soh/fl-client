#pragma once
#include "Base.h"
#include "MulticastDelegate.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class CUIPrefab;

enum class EUIMinimapEventID {

	END
};

typedef struct tagUIMinimapEventesc
{
	EUIMinimapEventID  eEventID = { EUIMinimapEventID::END };

	_uint iParam0 = {};

}UI_MINIMAP_EVENT_DESC;

class CUIMinimap_Manager final : public CBase
{
private:
	CUIMinimap_Manager();
	virtual ~CUIMinimap_Manager() = default;
public:
	CGameInstance* m_pGameInstance = { nullptr };

	CMulticastDelegate<void(const UI_MINIMAP_EVENT_DESC&)> m_vMinimapEvents = {};



public:
	static CUIMinimap_Manager* Create();
	virtual void Free()override;
};

NS_END