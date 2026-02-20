#pragma once
#include "ActionState.h"

NS_BEGIN(Client)

class CMonsterActionState final : public CActionState
{
	using Super = CActionState;
private:
	CMonsterActionState();
	CMonsterActionState(const CMonsterActionState& rhs);
	virtual ~CMonsterActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	CControlContext* GetOwnerControlContext() { return m_pOwnerControlContext; }

public:
	static CMonsterActionState* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

	friend CMonsterState_Factory;
};

NS_END