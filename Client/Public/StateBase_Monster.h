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

protected:
	using Super = CStateBase;

protected:
	CStateBase_Monster(CActionState* pOwnerComponent, const string& strName);
	virtual ~CStateBase_Monster() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;

public:
	virtual void Change_MonsterState(_int eKey);	// change 랩핑 함수 : 필요시 오버라이드
	_bool IsCancellation() { return m_pDesc->bCancellation; }

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
	HRESULT Bind_Transition(vector<DTO::STATE_TRANSITION>& transition);
	HRESULT Bind_Condition(vector<string> conds);
	HRESULT Bind_Feature();

	_bool Check_Transition(vector<DTO::STATE_TRANSITION>& transition);

	DTO::MONSTER_STATEBASE_DESC* m_pDesc = { nullptr };

	unordered_map<string, _int> m_umapState;

	unordered_map<string, _int> m_umapCondition;
	vector<std::function<_bool()>> m_vecCondition;

	unordered_map<string, _int> m_umapFeature;
	vector<std::function<void(const _float& fTimeDelta)>> m_vecFeature;

public:
	virtual void Free() override;

	friend CMonsterState_Factory;
};

NS_END