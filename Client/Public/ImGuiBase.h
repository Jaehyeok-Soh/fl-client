#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CImGuiBase abstract : public CBase
{
	using Super = CBase;
protected:
	CImGuiBase(std::string_view label);
	virtual ~CImGuiBase() = default;
public:
	virtual void Render(CGameObject* pGo) PURE;
protected:
	string m_strLabel = "";
	CGameInstance* m_pGameInstance = { nullptr };
public:
	virtual void Free() override;
};

NS_END