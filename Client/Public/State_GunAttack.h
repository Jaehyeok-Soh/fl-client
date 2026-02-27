#pragma once
#include "State_GunBase.h"

NS_BEGIN(Client)
class CState_GunAttack : public CState_GunBase
{
	using Super = CState_GunBase;

private:
	CState_GunAttack(CActionState* pOwnerComponent);
	virtual ~CState_GunAttack() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_GunAttack* Create(CActionState* pOwnerComponent, void* pArg);
	virtual void Free() override;
};

NS_END