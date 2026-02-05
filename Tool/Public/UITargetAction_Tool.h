#pragma once
#include "IUIActionForTarget.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Tool)
class CToolCanvas;
class CToolLayer;
class CToolUI;
class CUITargetAction_Tool final : public IUIActionForTarget
{
	using Super = IUIActionForTarget;
private:
	CUITargetAction_Tool(CToolUI* pUI);
	virtual ~CUITargetAction_Tool() = default;

public:
	CToolCanvas* Find_Canvas(const _string& strCanvasTag);
	CToolLayer* Find_Layer(const _string& strLayerTag);

public:
	void Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag, DTO::EUIAction eAction, const json& jTargetActionParam) override;
	void Trigger_All_Layer(uint32_t iLevelIndex, const _string& strLayerTag, DTO::EUIAction eAction, const json& jTargetActionParam) override;
	void Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag, DTO::EUIAction eAction, const json& jTargetActionParam) override;

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	CToolUI* m_pOwner = { nullptr };
	CGameObject* m_pTarget = { nullptr };
	
	CToolCanvas* m_pTargetCanvasCache = { nullptr };
	CToolLayer* m_pTargetLayerCache = { nullptr };

public:
	static CUITargetAction_Tool* Create(CToolUI* pUI);
	virtual void Free()override;
};

NS_END