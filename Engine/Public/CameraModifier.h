#pragma once
#include "Base.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class ENGINE_DLL ICameraModifier final : public CBase
{
	using Super = CBase;
public:
	virtual void Start() PURE;
	virtual void Update(const _float fTimeDelta) PURE;
	virtual _bool IsFinished() const PURE;

	virtual ECameraModifierType Get_Type() const PURE;

	// BasePose 기준으로 Modifier 누적 결과를 산출
	virtual void Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const PURE;
public:
	virtual void Free();
};

NS_END