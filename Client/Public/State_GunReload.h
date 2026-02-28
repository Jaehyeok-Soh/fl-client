#pragma once
#include "State_GunBase.h"

NS_BEGIN(Client)
class CState_GunReload final : public CState_GunBase
{
	using Super = CState_GunBase;

private:
	CState_GunReload(CActionState* pOwnerComponent);
	virtual ~CState_GunReload() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_GunReload* Create(CActionState* pOwnerComponent, void* pArg);
	virtual void Free() override;
};

NS_END