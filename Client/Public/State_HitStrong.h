#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)
class CState_HitStrong final : public CStateBase_Player
{
	using Super = CStateBase_Player;

public:
	enum class HitStrong_AnimIdx { FRONT, BACK, LEFT, RIGHT, END };

private:
	CState_HitStrong(CActionState* pOwnerComponent);
	virtual ~CState_HitStrong() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_HitStrong* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END