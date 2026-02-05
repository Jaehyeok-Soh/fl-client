#pragma once
#include "Base.h"
#include "DataStruct_UI.h"

NS_BEGIN(Engine)

class IUIActionForMe;
class IUIActionForTarget;

class ENGINE_DLL CUIAction_Registry final : public CBase
{
	using Super = CBase;
public:
	using ActionFunc = std::function<void(IUIActionForMe*, IUIActionForTarget*)>;
	using FactoryFunc = std::function<ActionFunc(const json&)>;

private:
	CUIAction_Registry();
	virtual ~CUIAction_Registry() = default;

private:
	void Initialize_CommonAction();
	void Initialize_CommonTargetAction();

public:
	/* Action */
	void Register_Factory(DTO::EUIAction ActionType, FactoryFunc factory);
	ActionFunc Build_Action(DTO::EUIAction ActionType, const json& params) const;

	void Clear();

private:
	vector<FactoryFunc > m_Factories;

public:
	static CUIAction_Registry* Create();
	virtual void Free() override;
};

NS_END
