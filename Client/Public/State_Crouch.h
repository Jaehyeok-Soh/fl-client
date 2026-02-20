#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_Crouch final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Crouch(CActionState* pOwnerComponent);
	virtual ~CState_Crouch() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	virtual void Set_NextStateDesc(_uint iNextState)override;	// 다음 state에 따라 desc을 작성한다 : 각 state 내부에서

public:
	static CState_Crouch* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
