#pragma once
#include "StateBase_Player.h"

/* combo는 4개의 애니메이션을 다 들고 있고 */
/* crounch combo1, normal combo1, combo2, combo3, combo4 */

NS_BEGIN(Client)
class CState_ComboBase abstract : public CStateBase_Player
{
	using Super = CStateBase_Player;

public:
	typedef struct tagComboStateDesc : public CStateBase_Player::PLAYER_STATEBASE_DESC
	{
		Vec3 vCombo_CheckTimes = Vec3::Zero;
	}PLAYER_COMBOBASE_DESC;

protected:
	CState_ComboBase(CActionState* pOwnerComponent, const string& strName);
	virtual ~CState_ComboBase() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	void Change_NextCombo();

private:
	std::array<_float, 3>		m_ComboTimes; // 4번째 콤보에서는 다음 콤보를 받지 않음
	_float		m_fTimeAcc		= { 0.f };
	_bool		m_bComboTime	= { false };

	_uint		m_iComboCount		= { 1 };

private:
	void			Count_ComboTime(const _float fTimeDelta);
	virtual void	Change_PlayerState(STATEKEY eKey) override;	// change 랩핑 함수 : 필요시 오버라이드

public:
	virtual void Free() override;
};

NS_END