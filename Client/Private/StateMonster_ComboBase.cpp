#include "pch.h"
#include "StateMonster_ComboBase.h"

#include "Monster_Base.h"
#include "MonsterControlContext.h"
// TODO:
// #include "Monster_Weapon_Base.h"

CStateMonster_ComboBase::CStateMonster_ComboBase(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CStateMonster_ComboBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MONSTER_COMBOBASE_DESC* pDesc = static_cast<MONSTER_COMBOBASE_DESC*>(pArg);

	m_ComboTimes.reserve(pDesc->vecCombo_CheckTimes.size());
	for (auto& time : pDesc->vecCombo_CheckTimes)
	{
		m_ComboTimes.push_back(time);
	}

	return S_OK;
}

HRESULT CStateMonster_ComboBase::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_ComboBase::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_bComboTime = true;
	m_iComboCount = 1;

	// TODO:
	//static_cast<CMonster_Base*>(Get_OwnerObject())->Change_Weapon(CMonster_Base::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CStateMonster_ComboBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Count_ComboTime(fTimeDelta);

	Check_Combo();
}

HRESULT CStateMonster_ComboBase::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	// TODO:
	//static_cast<CMonster_Base*>(Get_OwnerObject())->Change_Weapon(CMonster_Base::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

void CStateMonster_ComboBase::Change_NextCombo()
{
}

void CStateMonster_ComboBase::Change_FirstCombo()
{
}

void CStateMonster_ComboBase::Check_Combo()
{
}

void CStateMonster_ComboBase::Count_ComboTime(const _float fTimeDelta)
{
}

void CStateMonster_ComboBase::Free()
{
}
