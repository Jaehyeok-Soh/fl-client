#pragma once
#include "StateBase_Monster.h"

NS_BEGIN(Client)

class CMonsterActionState;
class CMonsterControlContext;

class CState_GimmikCamera final : public CStateBase_Monster
{
	using Super = CStateBase_Monster;
private:
	CState_GimmikCamera(CActionState* pOwnerComponent, _uint iStateIndex);
	virtual ~CState_GimmikCamera() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
private:
	_float m_fElapsed = { 0.f };
	const _float m_fHoldTime = { 0.5f };
public:
	static CState_GimmikCamera* Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END