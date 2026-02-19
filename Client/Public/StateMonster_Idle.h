#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CStateMonster_Idle final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CStateMonster_Idle(CActionState* pOwnerComponent);
	virtual ~CStateMonster_Idle() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CStateMonster_Idle* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END