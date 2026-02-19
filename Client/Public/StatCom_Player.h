#pragma once
#include "StatComponent.h"

/*
 플레이어의 모든 stat을 가지고 있고
 stat에 관련한 cool 타임을 카운트 한다

 현재 공격 state에 따라서 attack, sheild를 바꿔준다

 공격, 실드에 대한 수치를 다룸.
 그거를 정확하게 어떻게 바꿀지는 attack component가 다룬다

 따라서 player control key onoff를 여기서 관리한다
*/

NS_BEGIN(Client)
class CSkillComponent;

class CStatCom_Player final : public CStatComponent
{
	using Super = CStatComponent;

public:
	enum class STAT_TYPE { HP, DEFENSE, MENTAL, SKILLQ, SKILLE, MELEE, GUN }; // 값 가져오기 편하도록 enum 값 생성

	enum class TIMER_TYPE { DASH, COMBO }; // timer 타입

	enum  Attack_State : _uint  // 현재 하고 있는 공격들 : 플레이어의 state에서 on, stat에서 off
	{
		Melee = 0x0001
		, Gun = 0x0002
		, E = 0x0004
		, Q = 0x0008
	};

	typedef struct tagPlayerStatDesc :public CStatComponent::STATCOMP_DESC
	{
		SKILL_DESC tQSkill;
		SKILL_DESC tESkill;
		ATTACK_ELEMNETS tMelee;
		ATTACK_ELEMNETS tGun;

		_float fMaxMental = { 0.f };
		_float fMaxDefense = { 0.f };
		_float fDashCoolTime = { 0.f };
		_float fComboCoolTime = { 0.f };

	}PLAYER_STATCOMP_DESC;

private:
	CStatCom_Player();
	explicit CStatCom_Player(const CStatCom_Player& rhs);
	virtual ~CStatCom_Player() = default;

	virtual HRESULT Initialize_Prototype()			override;

public:
	virtual HRESULT Initialize(void* pArg)			override;
	virtual HRESULT Awake(_uint iLevelIndex)		override;
	virtual void	Update(const _float fTimeDelta) override;

	// getter setter func
public:

	// to UI 담당자 : 만약 const 때문에 귀찮다면 빼도 괜찮음.. from 플레이어 담당자
	// to UI 담당자 : getter func 필요에 따라 바꿔도 좋고, 추가해도 상관없을듯. from 플레이어 담당자
	// to UI 담당자 : cool time은 항상 0에서 1로 채우는 형식으로 갈거임.
	const SKILL_DESC& Get_Skill(STAT_TYPE eState) const {
		switch (eState)
		{
		case STAT_TYPE::SKILLQ:
			return m_tQSkill;
		case STAT_TYPE::SKILLE:
			return m_tESkill;
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
			return Vec2((_float)m_iHealth, (_float)m_iMaxHealth);
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

		default:
			return 0;
		}
	}

	_bool IsCan_SkillE();
	_bool IsCan_SkillQ();

public:
	_bool Set_AttackState(_uint iState, _bool bOn); // Attack_State 을 통해 넣을것
	void Add_ComboCount();
	void Sub_DashCount();
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
	Vec2			m_vDefense		= { 0.f,0.f }; // 추가 hp (cur defense, max defense)
	Vec2			m_vMentality	= { 0.f,0.f }; // 정신력 : 정신력을 기준으로 스킬을 사용한다 (cur mental, max mental)

	// counts
private:
	_int			m_iDashCount	= { 2 };
	_uint			m_iComboCount	= { 0 };

	// 공격 관련 
private:
	Flags			m_FAttState		= { 0 };
	_uint			m_iSkillAttack	= { 0 }; // 만약 skill 공격력이 따로 있어야 한다면.. todo : 이거는 뺄지도

	SKILL_DESC		m_tESkill;
	SKILL_DESC		m_tQSkill;
	ATTACK_ELEMNETS		m_tAttackMelee; // 근거리 무기
	ATTACK_ELEMNETS		m_tAttackGun;	// 원거리 무기

	// timers
private:
	TIME_COUNTER	m_tDashTimeCounter	= { }; // 여기는 리셋 해야함
	TIME_COUNTER	m_tComboTimeCounter	= { };

private:
	virtual void Sub_Hp(_int iHealth) override;		// 매게변수 값이 음수일때

	// time count
private:
	void Count_Dash(const _float fTimeDelta);
	void Count_Combo(const _float fTimeDelta);
	void Count_Skill(const _float fTimeDelta);

	void Count_Defense(const _float fTimeDelta);
	void Count_Mental(const _float fTimeDelta);

	// 내부 util funcs
private:
	void Set_PlayerKey(Flags FKeyFlag, _bool bOn);

public:
	static CStatCom_Player* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END