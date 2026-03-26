#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_JumpBullet final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_JumpBullet(CActionState* pOwnerComponent);
	virtual ~CState_JumpBullet() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	virtual void Set_NextStateDesc(_uint iNextState) override;
	virtual _bool Can_Captablity_Move() const override;

private:
	Vec3 m_vDir = { Vec3::Zero };

public:
	static CState_JumpBullet* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END