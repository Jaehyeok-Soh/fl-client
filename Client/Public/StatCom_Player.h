#pragma once
#include "MyStat.h"
#include "SkillBase.h"

/*
 플레이어의 모든 stat을 가지고 있고
 stat에 관련한 cool 타임을 카운트 한다

 현재 공격 state에 따라서 attack, sheild를 바꿔준다

 공격, 실드에 대한 수치를 다룸.
 그거를 정확하게 어떻게 바꿀지는 attack component가 다룬다

 따라서 player control key onoff를 여기서 관리한다
*/

NS_BEGIN(Client)

class CStatCom_Player final : public CMyStat
{
	using Super = CMyStat;

public:
	enum class TIMER_TYPE { DASH, COMBO }; // timer 타입

	enum  Attack_State : _uint  // 현재 하고 있는 공격들 : 플레이어의 state에서 on, stat에서 off
	{
		Melee = 0x0001
		, Gun = 0x0002
		, E = 0x0004
		, Q = 0x0008
	};

	typedef struct tagPlayerStatDesc :public CMyStat::STAT_DESC
	{
		CSkillBase* pQSkill = { nullptr };
		CSkillBase* pESkill = { nullptr };

		_float fMeleeAttack = { 0.f };
		_float fGunAttack	= { 0.f };
		
		_float fDashCoolTime = { 0.f };
		_float fComboCoolTime = { 0.f };

		vector<_uint> vecExtraComputeOrder;

		_float fCriticalRate = { 0.f }; // 0~1
		_float fCriticalAttack = { 0.f }; // 크리티컬 발생시 더해줄 값

	}PLAYER_STATCOMP_DESC;

private:
	CStatCom_Player();
	explicit CStatCom_Player(const CStatCom_Player& rhs);
	virtual ~CStatCom_Player() = default;

	virtual HRESULT Initialize_Prototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;

public:
	virtual void	Update_Stat(const _float fTimeDelta) override;

	// getter setter func
public:

	// to UI 담당자 : 만약 const 때문에 귀찮다면 빼도 괜찮음.. from 플레이어 담당자
	// to UI 담당자 : getter func 필요에 따라 바꿔도 좋고, 추가해도 상관없을듯. from 플레이어 담당자
	// to UI 담당자 : cool time은 항상 0에서 1로 채우는 형식으로 갈거임.
	// 0에서 1? 0에서 Max? -> 0에서 max
	const CSkillBase::SKILL_INFO& Get_Skill(Attack_State iAttState)  const {
		switch (iAttState)
		{
		case Attack_State::E:
			return m_pESkillBase->Get_SkillDesc();
		case Attack_State::Q:
			return m_pQSkillBase->Get_SkillDesc();
		default:
			return m_pQSkillBase->Get_SkillDesc();
		}
	};

	CSkillBase* Get_Skill_Ptr(Attack_State iAttState)  const {
		switch (iAttState)
		{
		case Attack_State::E:
			return m_pESkillBase;
		case Attack_State::Q:
			return m_pQSkillBase;
		default:
			return nullptr;
		}
	};

	const TIME_COUNTER& Get_Timer(TIMER_TYPE eTimerType) const
	{
		switch (eTimerType)
		{
		case TIMER_TYPE::DASH:
			return m_tDashTimeCounter;
		case TIMER_TYPE::COMBO:
			return m_tComboTimeCounter;
		default:
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

	_uint Get_AttState()const { return m_FAttState; }

	const EXTRA_ATTACK_DESC& Get_ExtraAttack_Desc();

public:
	_bool Set_AttackState(_uint iState, _bool bOn); // Attack_State 을 통해 넣을것

	void Reset_ComboCount();
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

	void Set_Attack_AddRate(_float fRate) { m_fAttack = m_fAttack *(1.f + fRate); }
	void Set_Critical_AddRate(_float fRate) { m_fCirticalRate_Add = fRate; }

	// counts
private:
	_int			m_iDashCount	= { 2 };
	_uint			m_iComboCount	= { 0 };

	// 공격 관련 
private:
	Flags						m_FAttState		= { 0 };
	_uint						m_iSkillAttack	= { 0 };			// 만약 skill 공격력이 따로 있어야 한다면.. todo : 이거는 뺄지도


	_float						m_fCriticalRate		= { 0.f }; // 0 ~ 1
	_float						m_fCirticalRate_Add = { 0.f }; // 0 ~ 1
	_float						m_fCirticalAttack	= { 0.f };
	Engine::EXTRA_ATTACK_DESC	m_tExtra_AttackDesc = {};

	//CSkillBase::SKILL_INFO		m_tESkill;
	//CSkillBase::SKILL_INFO		m_tQSkill;
	_float						m_fMeleeAtt;			// 근거리 무기
	_float						m_fGunAtt;				// 원거리 무기

	CSkillBase* m_pESkillBase = { nullptr };
	CSkillBase* m_pQSkillBase = { nullptr };

	// timers
private:
	TIME_COUNTER	m_tDashTimeCounter	= { };			// 여기는 리셋 해야함
	TIME_COUNTER	m_tComboTimeCounter	= { };

private:
	virtual void Sub_Hp(_float iHealth) override;		// 매게변수 값이 음수일때

	// time count
private:
	void Count_Dash(const _float fTimeDelta);
	void Count_Combo(const _float fTimeDelta);

	// 내부 util funcs
private:
	void Set_PlayerKey(Flags FKeyFlag, _bool bOn);

public:
	static CStatCom_Player* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END