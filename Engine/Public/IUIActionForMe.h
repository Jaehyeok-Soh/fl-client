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
	virtual void Set_Visible(_bool isVisible, const _float fDelay) PURE;
	virtual void Set_TextureIndex(_uint uIndex, const _float fDelay) PURE;
	virtual const _string& Get_Tag() const PURE;

	virtual void Start_Lerp_Movement(const Vec3& vTargetPos, const _float fTargetAlpha, const _float& fDuration, _bool isPin, const _float fDelay)PURE;
	virtual void Start_Return_Lerp_Movement(const _float fDelay)PURE;

	virtual void Start_Fade(const _float fStartAlpha, const _float fTargetAlpha, const _float fDuration, const _float fDelay)PURE;

public:
	virtual void Free()override;
};

NS_END