#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_DashBack final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_DashBack(CActionState* pOwnerComponent);
	virtual ~CState_DashBack() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual _uint	Get_Capabilities() const override;

public:
	static CState_DashBack* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
