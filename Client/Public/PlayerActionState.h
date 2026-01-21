#pragma once
#include "ActionState.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CPlayerActionState final : public CActionState
{
	using Super = CActionState;
private:
	CPlayerActionState();
	CPlayerActionState(const CPlayerActionState& rhs);
	virtual ~CPlayerActionState() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	static CPlayerActionState* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END