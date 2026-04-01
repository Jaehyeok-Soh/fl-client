#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CUIAmbSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CUIAmbSound_Manager)
	using Super = CBase;

	enum class ERegion
	{
		NONE = 0,
		NORMAL	= 1,
		SEA		= 1 << 1,
		FOREST	= 1 << 2,
		CAVE	= 1 << 3,
		BATTLE	= 1 << 4,
	};

private:
	CUIAmbSound_Manager();
	virtual ~CUIAmbSound_Manager() = default;

private:
	CGameInstance* m_pGameInstance = { nullptr };

	_uint m_iCurrentRegion = {};

public:
	virtual void Free()override;
};

NS_END