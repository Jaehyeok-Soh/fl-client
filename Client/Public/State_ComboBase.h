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
		Vec4 vCombo_CheckTimes = Vec4::Zero;
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
	std::array<_float, 4>		m_ComboTimes; // 4번째 콤보에서는 다음 콤보를 받지 않음
	_bool		m_bComboTime	= { false };
	_bool		m_bNextCombo	= { false };

	_uint		m_iComboCount	= { 1 };

protected:
	virtual _bool Can_CheckKey(const _float fTimeDelta) override;;

protected:
	void Change_NextCombo();
	void Change_FirstCombo();
	void Check_Combo();

private:
	void			Count_ComboTime(const _float fTimeDelta);

public:
	virtual void Free() override;
};

NS_END