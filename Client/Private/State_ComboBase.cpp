#include "pch.h"
#include "State_ComboBase.h"

CState_ComboBase::CState_ComboBase(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CState_ComboBase::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_COMBOBASE_DESC* pDesc = static_cast<PLAYER_COMBOBASE_DESC*>(pArg);
	m_ComboTimes[0] = pDesc->vCombo_CheckTimes.x;
	m_ComboTimes[1] = pDesc->vCombo_CheckTimes.y;
	m_ComboTimes[2] = pDesc->vCombo_CheckTimes.z;

	return S_OK;
}

HRESULT CState_ComboBase::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_ComboBase::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_bComboTime = false;

	return S_OK;
}

void CState_ComboBase::Update(const _float fTimeDelta)
{
	Count_ComboTime(fTimeDelta);

	Super::Update(fTimeDelta);
}

HRESULT CState_ComboBase::End()
{
	if(FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_ComboBase::Change_NextCombo()
{
	// 다음 콤보 change
	m_iComboCount++;
	m_fTimeAcc = 0.f;
	Request_ChangeAnimation(m_vecMainAnims[m_iComboCount], true, false, true);
}

void CState_ComboBase::Count_ComboTime(const _float fTimeDelta)
{
	// 4번째 일때는 하지 않음
	if (m_iComboCount == 4)
		return;

	// key cool time이 다 되었는데 아직 combotime이라면
	if (m_tKeyTimer.fTimeAcc / m_tKeyTimer.fMaxTime == 1.f &&
		m_bComboTime)
	{
		// comboTime을 카운트 한다
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc >= m_ComboTimes[m_iComboCount -1])
		{
			m_bComboTime = false;
		}
	}
}

void CState_ComboBase::Change_PlayerState(STATEKEY eKey)
{
	// 만약 combo Time이 아닌데 공격 키를 눌러도 변화하지 않음
	if (STATEKEY::LM == eKey)
	{
		if (m_bComboTime)
		{
			Change_NextCombo();
			return;
		}
	}

	Super::Change_PlayerState(eKey);
}

void CState_ComboBase::Free()
{
	Super::Free();
}
