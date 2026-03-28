#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CMonsterActionState;
class CMonsterControlContext;

class CState_StartCatch final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CState_StartCatch(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CState_StartCatch() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
private:

public:
	static CState_StartCatch* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END