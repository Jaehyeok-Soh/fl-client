#pragma once
#include "Base.h"

NS_BEGIN(Engine)


class ENGINE_DLL IUIActionForMe abstract : public CBase
{
	using Super = CBase;
public:

protected:
	IUIActionForMe();
	virtual ~IUIActionForMe() = default;
public:
	virtual void Set_Visible(_bool isVisible) PURE;
	virtual void Set_TextureIndex(_uint index) PURE;
	virtual const _string& Get_Tag() const PURE;

public:
	virtual void Free()override;
};

NS_END