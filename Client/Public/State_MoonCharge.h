#pragma once
#include "State_Charge.h"

NS_BEGIN(Client)

class CState_MoonCharge final : public CState_Charge
{
	using Super = CState_Charge;
private:
	CState_MoonCharge(CActionState* pOwnerComponent);
	virtual ~CState_MoonCharge() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_MoonCharge* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
