#pragma once
#include "State_ComboBase.h"

NS_BEGIN(Client)
class CState_MoonCombo final : public CState_ComboBase
{
	using Super = CState_ComboBase;
public:
	typedef struct tagMoonComboDesc
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

	}MOONCOMBO_DESC;

private:
	CState_MoonCombo(CActionState* pOwnerComponent);
	virtual ~CState_MoonCombo() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

private:
	// TODO - 애니메이션 툴로 빼야함
	_bool m_bShakeActived{ false };

public:
	static CState_MoonCombo* Create(CActionState* pOwnerComponent, void* pArg = nullptr);
	virtual void Free() override;
};

NS_END