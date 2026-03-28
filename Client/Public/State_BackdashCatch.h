#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CMonsterActionState;
class CMonsterControlContext;

class CState_BackdashCatch final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CState_BackdashCatch(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CState_BackdashCatch() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
private:
	_bool m_bOnce = { false };
	const _float m_fDesiredCatchDistance = { 6.5f };
	const _float  m_fDistanceTolerance = { 0.2f };
	const _float  m_fBackMovePerSec = 5.0f;
	CMonsterActionState* m_pOwnerActionState{ nullptr };
	CMonsterControlContext* m_pOwnerControlContext{ nullptr };
public:
	static CState_BackdashCatch* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END