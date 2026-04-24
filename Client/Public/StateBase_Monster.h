#pragma once
#include <set>
#include "StateBase.h"
#include "DTO_MonsterState.h"

NS_BEGIN(Client)

class CStateBase_Monster abstract : public CStateBase
{
public:
	typedef struct tagMonsterState_Node
	{

	}MONSTERSTATE_NODE;

	enum MOVEFLAGS : Flags
	{
		NORMAL = 0x0001 // 8방향 움직임
		, PRESS_CHANGE = 0x0002 // state change : press
		, UP_CHANGE = 0x0004 // state change : up
		, OWN = 0x0008 // 자신만의 움직임
	};
private:
	// For. Transition
	typedef struct tagBoundCondition
	{
		std::function<_bool(const DTO::STATE_PARAM&)> func;
		DTO::STATE_PARAM tParam{};
	}BOUND_CONDITION;

	// For. ConditionFeature
	typedef struct tagBoundConditionFeature
	{
		std::function<_bool(const DTO::STATE_PARAM&)> condition;
		std::function<void(const _float&, const DTO::STATE_PARAM&)> feature;

		_bool bIsOnce = { false };
		_bool bIsExecuted = { false };

		DTO::STATE_PARAM condParam;
		DTO::STATE_PARAM featParam;
	}BOUND_CONDFEATURE;

	// For. Feature
	typedef struct tagBoundFeature
	{
		std::function<void(const _float&, const DTO::STATE_PARAM&)> func;
		DTO::STATE_PARAM tParam{};

		_bool bIsOnce = { false };
		_bool bIsExecuted = { false };
	}BOUND_FEATURE;
protected:
	using Super = CStateBase;

protected:
	CStateBase_Monster(CActionState* pOwnerComponent, const string& strName, _uint iStateIndex);
	virtual ~CStateBase_Monster() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	void Update_Time(TIME_COUNTER timer, _float fTimeDelta);
	void Update_CooldownTime(_float fTimeDelta, _bool bEntryStart);

	virtual void Change_MonsterState(_int eKey);	// change 랩핑 함수 : 필요시 오버라이드
	_bool IsCancellation() { return m_pDesc->bCancellation; }
	_bool IsOverLifeTime() { return m_tStateLifeTime.fMinTime <= m_fStateElapsed; }
	_bool IsCooldownTimeSatisfy();

protected:
	STATE_START_DESC		m_tNextStateDesc = {};

	TIME_COUNTER			m_tStateLifeTime = {};
	TIME_COUNTER			m_tStateCoolDownTime = {};

	// state가 변환 했다면 true
protected:
	_bool Check_MoveKey(const _float fTimeDelta) { return false; }
	_bool Check_JumpKey(const _float fTimeDelta) { return false; }
	_bool Check_DashKey(const _float fTimeDelta) { return false; }
	_bool Check_CtrlPressKey(const _float fTimeDelta) { return false; }
	_bool Check_CtrlUpKey(const _float fTimeDelta) { return false; }
	_bool Check_MeleeKey(const _float fTimeDelta) { return false; }
	_bool Check_RangeKey(const _float fTimeDelta) { return false; }
	_bool Check_SkillKey(const _float fTimeDelta) { return false; }

protected:
	virtual void OwnMove(const _float fTimeDelta) {};		// state 내부에서 알아서 움직일때
	virtual void Set_NextStateDesc(_uint iNextState) {};	// 다음 state에 따라 desc을 작성한다 : 각 state 내부에서
	virtual void CheckAni_WhenStart() {};					// 만약 자체에서 로직을 통해 바꾸고 싶다면
protected:
	_uint m_iEndStateIdx = { 0 };			// CPlayer::State::END 캐싱 해둠 : 만약 END면 state change x

protected:
	_bool Has_ChangeState(_int eKey);

	HRESULT Bind_State();
	HRESULT Bind_PreAnims();
	HRESULT Bind_MainAnims();
	HRESULT Bind_Transition(vector<DTO::STATE_TRANSITION> &transition);
	HRESULT Bind_Feature();
	HRESULT Bind_ConditionFeature();
	HRESULT Bind_StartConditionFeature();
	HRESULT Bind_EndConditionFeature();
	_bool Check_Transition(vector<DTO::STATE_TRANSITION>& transition);
protected:
	DTO::MONSTER_STATEBASE_DESC* m_pDesc = { nullptr };

	unordered_map<string, _int> m_umapState;
	vector<BOUND_CONDITION> m_vecCondition;
	vector<BOUND_FEATURE> m_vecFeature;
	vector<BOUND_CONDFEATURE> m_vecConditionFeature;

	// Start
	vector<BOUND_CONDFEATURE> m_vecStartConditionFeature;
	// End
	vector<BOUND_CONDFEATURE> m_vecEndConditionFeature;

	_uint m_iThisStateIndex = {};
public:
	virtual void Free() override;

	friend CMonsterState_Factory;
};

NS_END