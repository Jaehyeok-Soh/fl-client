#pragma once
#include "StateBase_Player.h"

NS_BEGIN(Client)

class CState_RunShort final : public CStateBase_Player
{
	using Super = CStateBase_Player;
private:
	CState_RunShort(CActionState* pOwnerComponent);
	virtual ~CState_RunShort() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
	
private:
	_bool m_bCanRunLoop = { false };

public:
	static CState_RunShort* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END