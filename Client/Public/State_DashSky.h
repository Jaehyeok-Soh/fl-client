#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_DashSky final : public CStateBase_Player
{
	using Super = CStateBase_Player;

public:
	enum class ANI {FRONT, BACK};

private:
	CState_DashSky(CActionState* pOwnerComponent);
	virtual ~CState_DashSky() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	virtual void Set_NextStateDesc(_uint iNextState) override;
	virtual void CheckAni_WhenStart() override;

public:
	static CState_DashSky* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END