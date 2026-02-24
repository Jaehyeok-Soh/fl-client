#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)
class CState_GunBase abstract : public CStateBase_Player
{
	using Super = CStateBase_Player;

protected:
	CState_GunBase(CActionState* pOwnerComponent, const string& strName);
	virtual ~CState_GunBase() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual void Free() override;
};

NS_END