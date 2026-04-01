#pragma once
#include "State_ComboBase.h"

NS_BEGIN(Client)
class CState_DualCombo final : public CState_ComboBase
{
	using Super = CState_ComboBase;
public:
	typedef struct tagDualComboDesc
	{
		Vec4 vCombo_CheckTimes = Vec4::Zero;

		_int iSlideAnimIdx	= { -1 };
		_int iFirstAnimIdx	= { -1 };
		_int iSecondAnimIdx = { -1 };
		_int iThirdAnimIdx	= { -1 };
		_int iFourthAnimIdx = { -1 };

		_uint iEndStateIndex = {};

		CGun* pOwnerGun = { nullptr };

		_float fSlide_CheckTime = { 0.f };

		std::array<_float, 5> arrCombo_EndTimes;

	}DUALCOMBO_DESC;

private:
	CState_DualCombo(CActionState* pOwnerComponent);
	virtual ~CState_DualCombo() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual void Start_Third() override;
	virtual void Update_Second(const _float fTimeDelta) override;
	virtual void Update_Third(const _float fTimeDelta) override;

	virtual void End_Second() override;

private:
	_bool m_bOnce = { false };
public:
	static CState_DualCombo* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END