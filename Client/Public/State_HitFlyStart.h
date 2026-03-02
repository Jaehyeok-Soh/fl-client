#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)
class CState_HitFlyStart final : public CStateBase_Player
{
	using Super = CStateBase_Player;

private:
	CState_HitFlyStart(CActionState* pOwnerComponent);
	virtual ~CState_HitFlyStart() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual _uint	Get_Capabilities() const override { return	0; }

public:
	static CState_HitFlyStart* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END