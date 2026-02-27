#pragma once
#include "StateBase_Player.h"

/* statebase player와 로직이 똑같이 돈다. 단 weapon change만 신경 쓰면 됨 */
NS_BEGIN(Client)
class CState_GunIdle final : public CStateBase_Player
{
	using Super = CStateBase_Player;

private:
	CState_GunIdle(CActionState* pOwnerComponent);
	virtual ~CState_GunIdle() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_GunIdle* Create(CActionState* pOwnerComponent, void* pArg);
	virtual void Free() override;
};

NS_END