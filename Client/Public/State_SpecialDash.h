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

public:
	virtual _uint	Get_Capabilities() const override
	{
		return	0; // ¹«Àû
	}

private:
	_bool m_bOnce = { false };

	Vec3 m_vPivot = { Vec3::Zero };

	Vec3 m_vDir = { Vec3::Zero };

	_float m_fDistance = { 0.f };

public:
	static CState_SpecialDash* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END
