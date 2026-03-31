#include "pch.h"
#include "StatCom_Player.h"

#include "Player.h"
#include "PlayerControlContext.h"

#include "GameInstance.h"

CStatCom_Player::CStatCom_Player()
	:Super()
{
}

CStatCom_Player::CStatCom_Player(const CStatCom_Player& rhs)
	:Super(rhs)
	, m_iDashCount(rhs.m_iDashCount)
	, m_iComboCount(rhs.m_iComboCount)
	, m_tDashTimeCounter(rhs.m_tDashTimeCounter)
	, m_tComboTimeCounter(rhs.m_tComboTimeCounter)
	, m_FAttState(rhs.m_FAttState)
	, m_iSkillAttack(rhs.m_iSkillAttack)
		, m_pESkillBase(rhs.m_pESkillBase)
		, m_pQSkillBase(rhs.m_pQSkillBase)
		, m_tExtra_AttackDesc(rhs.m_tExtra_AttackDesc)
		, m_bInvincible(rhs.m_bInvincible)
	{
	}

	HRESULT CStatCom_Player::Initialize_Prototype()
	{
		if (FAILED(Super::Initialize_Prototype()))
			return E_FAIL;

		return S_OK;
	}

	HRESULT CStatCom_Player::Initialize(void* pArg)
	{
		if (FAILED(Super::Initialize(pArg)))
			return E_FAIL;

		PLAYER_STATCOMP_DESC* pDesc = static_cast<PLAYER_STATCOMP_DESC*>(pArg);

		m_fMeleeAtt = pDesc->fMeleeAttack;
		m_fGunAtt = pDesc->fGunAttack;
		m_tComboTimeCounter.fMaxTime = pDesc->fComboCoolTime;
		m_tDashTimeCounter.fMaxTime = pDesc->fDashCoolTime;

		m_tDashTimeCounter.bCountTime = true;
		m_tDashTimeCounter.bTimeReset = true;
		m_tComboTimeCounter.bCountTime = false;
		m_tComboTimeCounter.bTimeReset = true;

		m_pESkillBase = pDesc->pESkill;
		m_pQSkillBase = pDesc->pQSkill;
		Safe_AddRef(m_pESkillBase);
		Safe_AddRef(m_pQSkillBase);

		m_tExtra_AttackDesc.vecCompute_Order = std::move(pDesc->vecExtraComputeOrder);

		m_fCriticalRate = pDesc->fCriticalRate;
		m_fCirticalAttack = pDesc->fCriticalAttack;

		// 초기는 우선 근접 무기로 설정해둠
		m_FAttState = Attack_State::Melee;
		pDesc->fAttack = m_fMeleeAtt;
		return S_OK;
	}

	void CStatCom_Player::Update_Stat(const _float fTimeDelta)
	{
		Super::Update_Stat(fTimeDelta);

		Count_Dash(fTimeDelta);
		Count_Combo(fTimeDelta);

		if (m_bInvincible)
		{
			Fill_StatFull(STAT_TYPE::DEFENSE);
			Fill_StatFull(STAT_TYPE::HP);
			Fill_StatFull(STAT_TYPE::MENTAL);
		}
	}

	const EXTRA_ATTACK_DESC& CStatCom_Player::Get_ExtraAttack_Desc()
	{
		// 상황에 맞게 값을 설정해서 내보자

		/* 값 리셋 */
		m_tExtra_AttackDesc.fAddDamage = 0.f;
		m_tExtra_AttackDesc.fAddRate = 0.f;
		m_tExtra_AttackDesc.fRandomAdd_Rate = 0.f;
		m_tExtra_AttackDesc.fRandomMul_Rate = 0.f;
		m_tExtra_AttackDesc.vFinalDamege_MinMax = Vec2::Zero;
		m_tExtra_AttackDesc.vRandomAdd_MinMax = Vec2::Zero;
		m_tExtra_AttackDesc.vRandomMul_MinMax = Vec2::Zero;

		m_tExtra_AttackDesc.iDamageFlag = 0;

		// skill에서 set으로 값을 먼저 설정하고
		// skill 이 켜져 있다면 안에서 값 수정 하도록 설정
		if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::E))
		{
			m_pESkillBase->Set_ExtraAttack_Desc(m_tExtra_AttackDesc, this);
			m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::SKILLE);
		}
		if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Q))
		{
			m_pQSkillBase->Set_ExtraAttack_Desc(m_tExtra_AttackDesc, this);
			m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::SKILLQ);
		}

		if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Gun))
		{
			m_pQSkillBase->Set_ExtraAttack_Desc(m_tExtra_AttackDesc, this);
			m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::GUN);
		}

		// 근접일때에는 종류에 따라 flag 넘겨줌
		if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Melee))
		{
			_int iCurMeleeIdx = static_cast<CPlayer*>(Get_Owner())->Get_CurWeaponIdx(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE));
			switch (iCurMeleeIdx)
			{
			case static_cast<_int>(CPlayer::MELEE::SWORD):
				m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::SWORD);
				break;
			case static_cast<_int>(CPlayer::MELEE::DUAL):
				m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::DUAL);
				break;
			}
		}

		// critical이나 다른 값들 그 다음에 추가 한다
		// critical 정보 처리 : test용으로 일단 무조건 criticla
		if (m_fCriticalRate + m_fCirticalRate_Add > 1.f ||
			m_fCriticalRate + m_fCirticalRate_Add >= m_pGameInstance->Rand_Float(0.f, 1.f))
		{
			m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::CRITICAL);

			// critical은 일단 더하기로 하는걸로
			// 현재 공격력 *  0.1 ~ 0.3
			m_tExtra_AttackDesc.fAddDamage = m_fAttack * m_pGameInstance->Rand_Float(0.3f, 0.6f);
		}

		else
		{
			m_tExtra_AttackDesc.iDamageFlag |= ENUM_TO_UINT(EPlayerAttackFlag::NORMAL);
		}

		m_tExtra_AttackDesc.fRandomAdd_Rate += 0.5f;
		m_tExtra_AttackDesc.vRandomAdd_MinMax = { -100.f,100.f };

		// 연산 순서 : 우선은 stat 복사 생성시 desc으로 받도록 하자
		// 좀 복잡해진다면 flag mask 검사후 order 지정

		return m_tExtra_AttackDesc;
	}

	_bool CStatCom_Player::Set_AttackState(_uint iState, _bool bOn)
{
	// flag를 키거나 꺼줌
	if (bOn)
	{
		// skill energy 체크 및 cool time 셋팅
		switch (iState)
		{
		case Attack_State::Melee:
		{
			Engine_Utils::RemoveHard_Flag(m_FAttState, Attack_State::Gun);
			break;
		}
		case Attack_State::Gun:
		{
			Engine_Utils::RemoveHard_Flag(m_FAttState, Attack_State::Melee);
			break;
		}

		}

		Engine_Utils::Add_Flag(m_FAttState, iState);
	}

	else
	{
		// skill energy 체크 및 cool time 셋팅
		switch (iState)
		{
		case Attack_State::Melee:
		{
			m_tComboTimeCounter.fTimeAcc = 0.f;
			m_tComboTimeCounter.bCountTime = true; 
			break;
		}

		}

		Engine_Utils::RemoveHard_Flag(m_FAttState, iState);
	}


	// attack, sheild 다시 셋팅
	m_fAttack = 0;
	m_fSheild = 0;
	Engine_Utils::RemoveHard_Flag(m_FStatFlags, StatFlags::SheildOn);

	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Melee))
	{
		m_fAttack += m_fMeleeAtt;
	}

	if (Engine_Utils::Has_Flag(m_FAttState, Attack_State::Gun))
	{
		m_fAttack += m_fGunAtt;
	}

	if (m_fSheild > 0)
		Engine_Utils::Add_Flag(m_FStatFlags, StatFlags::SheildOn);

	return true;
}

void CStatCom_Player::Reset_ComboCount()
{
	m_iComboCount = 0;
	m_tComboTimeCounter.fTimeAcc = 0.f;
	m_tComboTimeCounter.bCountTime = false;
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

void CStatCom_Player::Sub_Hp(_float iHealth)
{
	// 쉴드 온이면 쉴드 값을 더해줌
	if (Engine_Utils::Has_Flag(m_FStatFlags, StatFlags::SheildOn))
		iHealth += m_fSheild;

	// 그래도 health가 음수일때
	if (iHealth < 0)
	{
		// 우선 디펜스 먼저 sub를 해줌
		m_vDefense.x += iHealth;

		// 디펜스가 음수가 되었다면 이제서야 health를 뺌
		if (m_vDefense.x < 0)
		{
			m_vHealth.x += m_vDefense.x;

			// UI가 바꿔둠
			if (m_vHealth.x <= 0.f)
				m_vHealth.x = 0.f;

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
			m_tDashTimeCounter.fTimeAcc = 0.f;

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

	if (IsClone())
	{
		Safe_Release(m_pESkillBase);
		Safe_Release(m_pQSkillBase);
	}
}
