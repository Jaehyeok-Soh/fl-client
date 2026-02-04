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
	virtual CGameObject* Find_GameObject(const _string& strTag)PURE;
	virtual CComponent* Find_Component(const _string& strTag)PURE;

public:
	virtual void Free()override;
};

NS_END