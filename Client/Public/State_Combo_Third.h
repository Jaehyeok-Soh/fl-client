#pragma once
#include "ComboState.h"

NS_BEGIN(Client)

class CState_Combo_Third final : public CComboState
{
	using Super = CComboState;
private:
	explicit CState_Combo_Third(CActionState* pOwnerComponent);
	virtual ~CState_Combo_Third() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
	virtual _uint Get_Capabilities() const override
	{
		return	StateCapability::ATTACK
			| StateCapability::BEATTACKED;
	}
private:
	_bool m_bFirst = { false };
public:
	static CState_Combo_Third* Create(CActionState* pOwnerComponent, void* pArg);
	virtual void Free() override;
};

NS_END