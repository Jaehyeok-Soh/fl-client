#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CGenericUI;
class IItem;

class CUIQuickSlot_Manager final : public CBase
{
	DECLARE_SINGLETON(CUIQuickSlot_Manager)

private:
	CUIQuickSlot_Manager();
	virtual ~CUIQuickSlot_Manager() = default;
private:
	CGameInstance* m_pGameInstance = { nullptr };

	array<IItem*, 8> m_ArrItems = {};

public:
	virtual void Free()override;
};

NS_END