#pragma once
#include "StateBase_Player.h"

/* state end 조건을 잘 줘야 할듯 : 우선 animation 재생만 */

NS_BEGIN(Client)
class CState_Stun final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_Stun(CActionState* pOwnerComponent);
	virtual ~CState_Stun() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual _uint	Get_Capabilities() const override { return ENUM_TO_UINT(Engine::StateCapability::BEATTACKED); };

protected:
	virtual _bool Can_CheckKey(const _float fTimeDelta) override;

private:
	const _float m_fStunCoolTime = { 3.f };

public:
	static CState_Stun* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END