#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameObject;
class CComponent;

class ENGINE_DLL IUIActionForTarget abstract : public CBase
{
	using Super = CBase;
protected:
	IUIActionForTarget();
	virtual ~IUIActionForTarget() = default;

public:
	virtual void Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag)PURE;
	virtual void Trigger_All_Layer(uint32_t iLevelIndex, const _string& strLayerTag)PURE;
	virtual void Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag)PURE;

public:
	virtual void Free()override;
};

NS_END