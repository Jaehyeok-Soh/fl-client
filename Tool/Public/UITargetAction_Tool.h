#pragma once
#include "IUIActionForTarget.h"

NS_BEGIN(Tool)
class CToolUI;
class CUITargetAction_Tool final : public IUIActionForTarget
{
	using Super = IUIActionForTarget;
private:
	CUITargetAction_Tool(CToolUI* pUI);
	virtual ~CUITargetAction_Tool() = default;
	
public:
	CGameObject* Find_GameObject(const _string& strTag) override;
	CComponent* Find_Component(const _string& strTag) override;

private:
	CToolUI* m_pOwner = { nullptr };

public:
	static CUITargetAction_Tool* Create(CToolUI* pUI);
	virtual void Free()override;
};

NS_END