#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_RunLoop final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_RunLoop(CActionState* pOwnerComponent);
	virtual ~CState_RunLoop() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	_float m_fDuration = { 0.f };
	_float m_fInterval = { 0.25f };

public:
	static CState_RunLoop* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END