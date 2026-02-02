#pragma once
#include "Base.h"

NS_BEGIN(Tool)
class CToolUI;

class CUI_Action_Registry final : public CBase
{
	DECLARE_SINGLETON(CUI_Action_Registry)
	using Super = CBase;
	using ActionFunc = std::function<void(void*)>;

private:
	CUI_Action_Registry();
	virtual ~CUI_Action_Registry() = default;

public:
	void Register_Action(const _string& strActionName, ActionFunc function);
	void Execute_Action(const _string& strActionName);
	void Clear_Actions();

private:
	map<string, ActionFunc> m_Actions;

public:
	virtual void Free() override;
};

NS_END