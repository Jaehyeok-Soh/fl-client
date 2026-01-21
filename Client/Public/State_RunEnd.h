#pragma once
#include "StateBase.h"

NS_BEGIN(Client)

class CState_RunEnd final : public CStateBase
{
	using Super = CStateBase;
private:
	CState_RunEnd(CActionState* pOwnerComponent);
	virtual ~CState_RunEnd() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
public:
	static CState_RunEnd* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END

