#include "pch.h"
#include "StatCom_Player.h"

#include "Player.h"
#include "PlayerControlContext.h"
#include "Engine_Utils.h"

CStatCom_Player::CStatCom_Player()
	:Super()
{
}

CStatCom_Player::CStatCom_Player(const CStatCom_Player& rhs)
	:Super(rhs)
	, m_vDefense(rhs.m_vDefense)
	, m_vMentality(rhs.m_vMentality)
	, m_iDashCount(rhs.m_iDashCount)
	, m_iComboCount(rhs.m_iComboCount)
	, m_tDashTimeCounter(rhs.m_tDashTimeCounter)
	, m_tComboTimeCounter(rhs.m_tComboTimeCounter)
	, m_FAttState(rhs.m_FAttState)
	, m_iSkillAttack(rhs.m_iSkillAttack)
{
}

HRESULT CStatCom_Player::Initialize_Prototype()
{
	if (Super::Initialize_Prototype())
		return E_FAIL;

	return S_OK;
}

HRESULT CStatCom_Player::Awake(_uint iLevelIndex)
{
	if (Super::Awake(iLevelIndex))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatCom_Player::Initialize(void* pArg)
{
	PLAYER_STATCOMP_DESC* pDesc = static_cast<PLAYER_STATCOMP_DESC*>(pArg);

	m_tDashTimeCounter.bCountTime	= true;
	m_tDashTimeCounter.bTimeReset	= true;
	m_tComboTimeCounter.bCountTime	= false;
	m_tComboTimeCounter.bTimeReset	= true;

	m_tESkill		= pDesc->tESkill;
	m_tQSkill		= pDesc->tQSkill;
	m_tAttackMelee	= pDesc->tMelee;
	m_tAttackGun	= pDesc->tGun;

	// skill cool time 일단 max로 맞춰두기
	m_tESkill.TCoolTime.x = m_tESkill.TCoolTime.y;
	m_tQSkill.TCoolTime.x = m_tQSkill.TCoolTime.y;

	m_vDefense		= { pDesc->fMaxDefense, pDesc->fMaxDefense };
	m_vMentality	= { pDesc->fMaxMental, pDesc->fMaxMental };

	m_tDashTimeCounter.fMaxTime		= pDesc->fDashCoolTime;
	m_tComboTimeCounter.fMaxTime	= pDesc->fComboCoolTime;

	// 초기는 우선 근접 무기로 설정해둠
	m_FAttState = Attack_State::Melee;
	pDesc->iAttack = m_tAttackMelee.iAttack;
	pDesc->iSheild = m_tAttackMelee.iSheild;

	if (Super::Initialize(pDesc))
		return E_FAIL;

	return S_OK;
}

void CStatCom_Player::Update(const _float fTimeDelta)
{
	Count_Dash(fTimeDelta);
	Count_Combo(fTimeDelta);
	Count_Skill(fTimeDelta);

	Count_Defense(fTimeDelta);
	Count_Mental(fTimeDelta);
}

_bool CStatCom_Player::IsCan_SkillE()
{
	return (m_vMentality.x >= m_tESkill.iNeedMental) && (m_tESkill.TCoolTime.x == m_tESkill.TCoolTime.y);
}

_bool CStatCom_Player::IsCan_SkillQ()
{
	return (m_vMentality.x >= m_tQSkill.iNeedMental) && (m_tQSkill.TCoolTime.x == m_tQSkill.TCoolTime.y);
}

_bool CStatCom_Player::Set_AttackState(_uint iState, _bool bOn)
{
	// flag를 키거나 꺼줌
	if (bOn)
	{
		// skill energy 체크 및 cool time 셋팅
		switch (iState)
		{
		case Attack_State::E:
		{
			if (!IsCan_SkillE())
				return false;

			m_tESkill.TCoolTime.x = 0.f; break;

			// skill start player에게 요청
			//static_cast<CPlayer*>(Get_Owner())->Start_Skill(CPlayer::State::SKILL1);
		}

		case Attack_State::Q:
		{
			if (!IsCan_SkillQ())
				return false;

			m_tQSkill.TCoolTime.x = 0.f; break;
		}

		}

		Engine_Utils::Add_Flag(m_FAttState, iState);
	}

	else
	{
		// skill energy 체크 및 cool time 셋팅
		switch (iState)
		{
		case Attack_State::E:
		{
			m_tESkill.TCoolTime.x = m_tESkill.TCoolTime.y; break;
		}

		case Attack_State::Q:
		{
			m_tQSkill.TCoolTime.x = m_tESkill.TCoolTime.y; break;
		}

		}

		Engine_Utils::RemoveHard_Flag(m_FAttState, iState);
	}


	// attack, sheild 다시 셋팅
	m_iAttack = 0;
	m_iSheild = 0;
	m_bSheildOn = false;
	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Melee))
	{
		m_iAttack += m_tAttackMelee.iAttack;
		m_iSheild += m_tAttackMelee.iSheild;
	}

	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Gun))
	{
		m_iAttack += m_tAttackGun.iAttack;
		m_iSheild += m_tAttackGun.iSheild;
	}

	// todo : skill쪽은 아예 빼서... skill component가 전반적으로 관리 하도록 변경 할 수도
	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::E))
	{
		m_iAttack += m_tESkill.tAttDesc.iAttack;
		m_iSheild += m_tESkill.tAttDesc.iSheild;
	}

	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Q))
	{
		m_iAttack += m_tQSkill.tAttDesc.iAttack;
		m_iSheild += m_tQSkill.tAttDesc.iSheild;
	}

	if (m_iSheild > 0)
		m_bSheildOn = true;

	return true;
}

void CStatCom_Player::Add_ComboCount()
{
	m_iComboCount++;
	m_tComboTimeCounter.fTimeAcc = 0.f;
	m_tComboTimeCounter.bCountTime = false;
}

void CStatCom_Player::Sub_DashCount()
{
	m_iDashCount--;

	// 만약 음수나 0이 되었다면
	if (m_iDashCount <= 0)
	{
		m_iDashCount = 0;

		// control에게 dash 키 못 받도록함
		Set_PlayerKey(CPlayerControlContext::DASH, false);
	}
}

void CStatCom_Player::Sub_Hp(_int iHealth)
{
	// 쉴드 온이면 쉴드 값을 더해줌
	if (m_bSheildOn)
		iHealth += m_iSheild;

	// 그래도 health가 음수일때
	if (iHealth < 0)
	{
		// 우선 디펜스 먼저 sub를 해줌
		m_vDefense.x += iHealth;

		// 디펜스가 음수가 되었다면 이제서야 health를 뺌
		if (m_vDefense.x < 0)
		{
			m_iHealth += (_uint)m_vDefense.x;

			m_vDefense.x = 0;
		}
	}
}

void CStatCom_Player::Count_Dash(const _float fTimeDelta)
{
	// dash가 2보다 적을 때
	if (m_iDashCount < 2)
	{
		if (m_tDashTimeCounter.CountTime(fTimeDelta) == 1.f)
		{
			m_iDashCount++;

			// control에게 dash 키 받도록함
			Set_PlayerKey(CPlayerControlContext::DASH, true);
		}
	}
}

void CStatCom_Player::Count_Combo(const _float fTimeDelta)
{
	if (m_tComboTimeCounter.bCountTime)
	{
		if (m_tComboTimeCounter.CountTime(fTimeDelta) == 1.f)
		{
			m_tComboTimeCounter.bCountTime = false;
			m_iComboCount = 0;
		}
	}
}

void CStatCom_Player::Count_Skill(const _float fTimeDelta)
{
	// E Skill
	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::E))
	{
		// cool timer
		if (m_tESkill.TCoolTime.x < m_tESkill.TCoolTime.y)
		{
			m_tESkill.TCoolTime.x += fTimeDelta;

			if (m_tESkill.TCoolTime.x >= m_tESkill.TCoolTime.y)
			{
				m_tESkill.TCoolTime.x = m_tESkill.TCoolTime.y;
				Set_PlayerKey(CPlayerControlContext::SKILL1, true);
			}
		}
	}

	// Q skill
	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Q))
	{
		// cool timer
		if (m_tQSkill.TCoolTime.x < m_tQSkill.TCoolTime.y)
		{
			m_tQSkill.TCoolTime.x += fTimeDelta;

			if (m_tQSkill.TCoolTime.x >= m_tQSkill.TCoolTime.y)
			{
				m_tQSkill.TCoolTime.x = m_tQSkill.TCoolTime.y;
				Set_PlayerKey(CPlayerControlContext::SKILL2, true);
			}
		}
	}
}

void CStatCom_Player::Count_Defense(const _float fTimeDelta)
{
	// 만약 max치 보다 적다면
	if (m_vDefense.x < m_vDefense.y)
	{
		// 더해줌
		m_vDefense.x += fTimeDelta;

		// max 넘어가지 않게 조정
		if (m_vDefense.x > m_vDefense.y)
			m_vDefense.x = m_vDefense.y;
	}
}

void CStatCom_Player::Count_Mental(const _float fTimeDelta)
{
	// 만약 max치 보다 적다면
	if (m_vMentality.x < m_vMentality.y)
	{
		// 더해줌
		m_vMentality.x += fTimeDelta;

		// max 넘어가지 않게 조정
		if (m_vMentality.x > m_vMentality.y)
			m_vMentality.x = m_vMentality.y;
	}
}

void CStatCom_Player::Set_PlayerKey(Flags FKeyFlag, _bool bOn)
{
	static_cast<CPlayerControlContext*>(Get_Owner()->Get_Component<CControlContext>())->
		Set_CheckKey(static_cast<CPlayerControlContext::KEYFLAGS>(FKeyFlag), bOn);
}

CStatCom_Player* CStatCom_Player::Create()
{
	CStatCom_Player* pInstance = new CStatCom_Player();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CStatCom_Player::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CStatCom_Player::Clone(void* pArg)
{
	CStatCom_Player* pInstance = new CStatCom_Player(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStatCom_Player::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatCom_Player::Free()
{
	__super::Free();
}
