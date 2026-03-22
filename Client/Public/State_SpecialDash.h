#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_SpecialDash final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_SpecialDash(CActionState* pOwnerComponent);
	virtual ~CState_SpecialDash() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	virtual _bool Can_CheckKey(const _float fTimeDelta);

private:
	Vec3 m_vPivot = { Vec3::Zero };

	Vec3 m_vDir = { Vec3::Zero };

	_float m_fDeSpeed = { 8.f };

	array<Vec3, 4> m_arrPoses;

public:
	static CState_SpecialDash* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
