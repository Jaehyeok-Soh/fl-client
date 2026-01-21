#pragma once
#include "ComboState.h"

NS_BEGIN(Client)

class CState_Combo_Fourth final : public CComboState
{
	using Super = CComboState;
private:
	explicit CState_Combo_Fourth(CActionState* pOwnerComponent);
	virtual ~CState_Combo_Fourth() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
	virtual _uint Get_Capabilities() const override
	{
		return	ToMask(StateCapability::ATTACK);
	}
private:
	_bool m_bFirst = { false };
	_bool m_bSecond = { false };
	_bool m_bThird = { false };
public:
	static CState_Combo_Fourth* Create(CActionState* pOwnerComponent, void* pArg);
	virtual void Free() override;
};

NS_END