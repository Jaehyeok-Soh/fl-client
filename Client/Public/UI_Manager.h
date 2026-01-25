#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	CGameInstance* m_pGameInstance = { nullptr };

public:
	virtual void Free()override;

};

NS_END