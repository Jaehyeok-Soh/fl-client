#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_Fall final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Fall(CActionState* pOwnerComponent);
	virtual ~CState_Fall() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	_float m_fFallTimeAcc = {0.f};

private:
	void Count_FallTime(const _float fTimeDelta);

private:
	virtual void OwnMove(const _float fTimeDelta) override;
	virtual void Set_NextStateDesc(_uint iNextState) override;

public:
	static CState_Fall* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END