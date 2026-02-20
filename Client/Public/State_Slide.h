#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_Slide final : public CStateBase_Player
{
	using Super = CStateBase_Player;
public:
	enum class ANI {NORMAL, SKY};
private:
	CState_Slide(CActionState* pOwnerComponent);
	virtual ~CState_Slide() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	virtual void OwnMove(const _float fTimeDelta) override;
	virtual void Change_PlayerState(STATEKEY eKey) override;

	virtual void Set_NextStateDesc(_uint iNextState)override;

public:
	static CState_Slide* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END

