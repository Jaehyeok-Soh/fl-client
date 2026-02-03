#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class IUIActionForMe;

class ENGINE_DLL CUIAction_Registry final : public CBase
{
	using Super = CBase;
public:
	using ActionFunc = std::function<void(IUIActionForMe*)>;
	using FactoryFunc = std::function<ActionFunc(const json&)>;

private:
	CUIAction_Registry();
	virtual ~CUIAction_Registry() = default;

private:
	void Initialize_CommonAction();

public:
	void Register_Factory(const _string& strActionName, FactoryFunc factory);
	ActionFunc Build_Action(const _string& strActionName, const json& params) const;
	void Clear();

private:
	std::map< _string , FactoryFunc > m_Factories;

public:
	static CUIAction_Registry* Create();
	virtual void Free() override;
};

NS_END
