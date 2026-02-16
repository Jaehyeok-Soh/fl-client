#pragma once
#include "StatComponent.h"

/*
 플레이어의 모든 stat을 가지고 있고
 stat에 관련한 cool 타임을 카운트 한다

 따라서 player control key onoff를 여기서 관리한다

 현재 공격 state에 따라서 attack, sheild를 바꿔준다
*/

NS_BEGIN(Client)
class CStatCom_Player final : public CStatComponent
{
	using Super = CStatComponent;

public:
	enum class SKILL_TYPE { DAMAGE, BUFF, SUMMON, CURE, DEFENSE }; // skill의 타입

	enum class STAT_TYPE { HP, DEFENSE, MENTAL, SKILLQ, SKILLE, MELEE, GUN }; // 값 가져오기 편하도록 enum 값 생성

	enum class TIMER_TYPE { DASH, COMBO }; // timer 타입

	enum  Attack_State : _uint  // 현재 하고 있는 공격들 : 플레이어의 state에서 on, stat에서 off
	{
		Melee = 0x0001
		, Gun = 0x0002
		, Q = 0x0004
		, E = 0x0008
	};

	typedef struct tagSkillDesc
	{
		SKILL_TYPE	eSkillType = { SKILL_TYPE::DAMAGE };
		TimeCount	fCoolTime = { 0.f,0.f };	// 다음 공격까지 cooltime
		TimeCount	fAttackTime = { 0.f,0.f };	// attack time

		_uint		iNeedMental = { 0 };		// 공격하기 위한 정신력 정도

		_uint		iAttack = {};			// 공격력
		_uint		iSheild = {};			// 방어력
		_bool		bSheild = { false };	// 방어 onOff
		_float		fRange = { 0.f };		// 공격범위
	}SKILL_DESC;

	typedef struct tagAttackDesc
	{
		_uint		iAttack = { 0 };			// 공격력
		_float		fRange = { 0.f };			// 공격 범위..
	}ATTACK_DESC;



private:
	CStatCom_Player();
	explicit CStatCom_Player(const CStatCom_Player& rhs);
	virtual ~CStatCom_Player() = default;

	virtual HRESULT Initialize_Prototype()			override;

public:
	virtual HRESULT Awake(_uint iLevelIndex)		override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void	Update(const _float fTimeDelta) override;

	// getter setter func
public:

	// to UI 담당자 : 만약 const 때문에 귀찮다면 빼도 괜찮음.. from 플레이어 담당자
	const SKILL_DESC& Get_Skill(STAT_TYPE eState) const {
		switch (eState)
		{
		case STAT_TYPE::SKILLQ:
			return m_tQSkill;
		case STAT_TYPE::SKILLE:
			return m_tESkill;
		default:
			return;
		}
	};

	const Vec2& Get_Stat(STAT_TYPE eState) const // x : 현재 값, y : max 값
	{
		switch (eState)
		{
		case STAT_TYPE::HP:
			return m_vDefense;
		case STAT_TYPE::DEFENSE:
			return m_vMentality;
		case STAT_TYPE::MENTAL:
			return Vec2(m_iHealth, m_iMaxHealth);
		default:
			return;
		}
	}

	const TIME_COUNTER& Get_Timer(TIMER_TYPE eTimerType) const
	{
		switch (eTimerType)
		{
		case TIMER_TYPE::DASH:
			return m_tDashTimeCounter;
		case TIMER_TYPE::COMBO:
			return m_tComboTimeCounter;
		}
	}

	const _uint Get_Count(TIMER_TYPE eTimerType) const 
	{
		switch (eTimerType)
		{
		case TIMER_TYPE::DASH:
			return (_uint)m_iDashCount;
		case TIMER_TYPE::COMBO:
			return (_uint)m_iComboCount;
		}
	}

public:
	void Add_Combo() { m_iComboCount++; }
	void Sub_Dash() { m_iDashCount--; if (m_iDashCount < 0) m_iDashCount = 0; }
	void Set_Timer(TIMER_TYPE eTimerType, _bool bTimerOn) {
		switch (eTimerType)
		{
		case TIMER_TYPE::DASH:
			m_tDashTimeCounter.bCountTime = bTimerOn;
			break;
		case TIMER_TYPE::COMBO:
			m_tComboTimeCounter.bCountTime = bTimerOn;
			break;
		}
	}

	// stats
private:
	Vec2		m_vDefense = { 0.f,0.f }; // 추가 hp (cur defense, max defense)
	Vec2		m_vMentality = { 0.f,0.f }; // 정신력 : 정신력을 기준으로 스킬을 사용한다 (cur mental, max mental)

	// counts
private:
	_int		m_iDashCount = { 2 };
	_uint		m_iComboCount = { 0 };

	// 공격 관련 
private:
	SKILL_DESC	m_tQSkill;
	SKILL_DESC	m_tESkill;
	ATTACK_DESC m_tAttackMelee; // 근거리 무기
	ATTACK_DESC m_tAttackGun;	// 원거리 무기

	// timers
private:
	TIME_COUNTER	m_tDashTimeCounter		= { 0.f,1.f }; // 여기는 리셋 하지마
	TIME_COUNTER	m_tComboTimeCounter		= { 0.f,1.f };

public:
	static CStatCom_Player* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END