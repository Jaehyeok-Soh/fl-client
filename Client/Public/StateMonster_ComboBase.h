#pragma once
#include "StateBase_Monster.h"
class CStateMonster_ComboBase abstract : public CStateBase_Monster
{
	using Super = CStateBase_Monster;

public:
	typedef struct tagComboStateDesc : public CStateBase_Monster::MONSTER_STATEBASE_DESC
	{
		_uint iNumCombo = {};
		vector<_float> vecCombo_CheckTimes;
	}MONSTER_COMBOBASE_DESC;

protected:
	CStateMonster_ComboBase(CActionState* pOwnerComponent, const string& strName);
	virtual ~CStateMonster_ComboBase() = default;

	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

protected:
	PLAYER_COMBOBASE_DESC	m_tDesc = {};
	vector<_float>			m_ComboTimes;
	_bool		m_bComboTime = { false };
	_bool		m_bNextCombo = { false };
	_uint		m_iComboCount = { 1 };

protected:
	void Change_NextCombo();
	void Change_FirstCombo();
	void Check_Combo();

private:
	void Count_ComboTime(const _float fTimeDelta);

public:
	virtual void Free() override;
};

