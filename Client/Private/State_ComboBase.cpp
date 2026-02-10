#include "pch.h"
#include "State_ComboBase.h"

#include "PlayerControlContext.h"

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
	m_ComboTimes[3] = pDesc->vCombo_CheckTimes.w;

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

	m_bComboTime	= true;
	m_iComboCount	= 1;

	return S_OK;
}

void CState_ComboBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Count_ComboTime(fTimeDelta);

	Check_Combo();
}

HRESULT CState_ComboBase::End()
{
	if(FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_ComboBase::Change_NextCombo()
{
	if (m_iComboCount == 4)
	{
		Change_FirstCombo();
		return;
	}

	// 시간들 초기화
	m_tKeyTimer.fTimeAcc	= 0.f;
	m_fStateElapsed			= 0.f;

	// combo 관련 변수 설정
	m_iMainAnimIdx = m_iComboCount++;
	m_bNextCombo = false;

	Request_ChangeAnimation(m_vecMainAnims[m_iComboCount], true, false, true);
}

void CState_ComboBase::Change_FirstCombo()
{
	// 시간들 초기화
	m_tKeyTimer.fTimeAcc = 0.f;
	m_fStateElapsed = 0.f;

	// combo 관련 변수 설정
	m_iComboCount = 1;
	m_iMainAnimIdx = 0;
	m_bNextCombo = false;

	Request_ChangeAnimation(m_vecMainAnims[0], true, false, true);
}

void CState_ComboBase::Count_ComboTime(const _float fTimeDelta)
{
	// 만약 아직 combo check time이고, key가 눌렸다면
	if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::LATT))
		 && m_fStateElapsed <= m_ComboTimes[m_iComboCount - 1])
	{
		m_bNextCombo = true;
	}
}

void CState_ComboBase::Check_Combo()
{
	// 너무 시간이 지나면 combo end
	if (m_fStateElapsed > m_ComboTimes[m_iComboCount - 1] +1.5f)
	{
		return;
	}

	// combo change 시간 ~ end 시간 사이
	else if (m_fStateElapsed > m_ComboTimes[m_iComboCount - 1] + 1.f &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::LATT)))
	{
		Change_FirstCombo();
		return;
	}

	// combo change 시간이 다 되었는데
	else if (m_fStateElapsed > m_ComboTimes[m_iComboCount - 1] &&
		m_bNextCombo)
	{
		Change_NextCombo();
	}
}

void CState_ComboBase::Free()
{
	Super::Free();
}
