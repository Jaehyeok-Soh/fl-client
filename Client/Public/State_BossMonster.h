#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CState_BossMonster final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CState_BossMonster(CActionState* pOwnerComponent);
	virtual ~CState_BossMonster() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	static CState_BossMonster* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END