#pragma once
#include "Base.h"

NS_BEGIN(DTO)
enum class EUIAction;
NS_END

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
	virtual void Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag, DTO::EUIAction eAction, const json& jTargetActionParam)PURE;
	virtual void Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag, DTO::EUIAction eAction, const json& jTargetActionParam)PURE;

public:
	virtual void Free()override;
};

NS_END