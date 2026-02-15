#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_Charge final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Charge(CActionState* pOwnerComponent);
	virtual ~CState_Charge() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	void Go_Front(const _float fTimeDelta);

public:
	static CState_Charge* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
