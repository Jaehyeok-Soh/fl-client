#pragma once
#include "IUIActionForTarget.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CGenericUI;
class CUITargetAction_Client final : public IUIActionForTarget
{
	using Super = IUIActionForTarget;
private:
	CUITargetAction_Client(CGenericUI* pUI);
	virtual ~CUITargetAction_Client() = default;

public:
	
	void Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag, DTO::EUIAction eAction, const json& jTargetActionParam) override;
	void Trigger_All_Layer(uint32_t iLevelIndex, const _string& strLayerTag, DTO::EUIAction eAction, const json& jTargetActionParam) override;
	void Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag, DTO::EUIAction eAction, const json& jTargetActionParam) override;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CGenericUI* m_pOwner = { nullptr };
	CGameObject* m_pTarget = { nullptr };

public:
	static CUITargetAction_Client* Create(CGenericUI* pUI);
	virtual void Free()override;
};

NS_END