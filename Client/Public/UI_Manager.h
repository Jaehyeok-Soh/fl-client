#pragma once
#include "Base.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
#pragma pop_macro("new")

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CCanvas;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;
private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<CCanvas*>* m_vecCanvas[g_iLevelType_Count] = { nullptr };
public:
	virtual void Free()override;

};

NS_END