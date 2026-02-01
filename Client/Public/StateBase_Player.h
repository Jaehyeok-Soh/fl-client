#pragma once
#include "StateBase.h"

// player의 state들이 공통적으로 처리해야하는 것들을 다루기 위해
// 부모 statebase를 하나 생성

NS_BEGIN(Client)

class CStateBase_Player abstract : public CStateBase
{
	using Super = CStateBase;

public:
	// wasd가 눌렸을때 어떻게 할건지
	enum MOVEFLAGS : Flags
	{
			NORMAL			= 0x0001 // 8방향 움직임
		,	PRESS_CHANGE	= 0x0002 // state change : press
		,	UP_CHANGE		= 0x0004 // state change : up
		,	OWN				= 0x0008 // 자신만의 움직임
	};

	enum class STATEKEY : _uint {MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, LOOPDONE , END}; //END에는 키가 없을떄 바꿀 state를 넣자

	typedef struct tagPlayerStateDesc : public CStateBase::STATE_DESC
	{
		Flags					FMoves		= { 0 };
		vector<_uint>			vecChangeState_ByKey;			// 키 입력에 따라 어떻게 바꿀지 담는 벡터

		TIME_COUNTER			tKeyTimer = {};
	}PLAYER_STATEBASE_DESC;

protected:
	CStateBase_Player(CActionState* pOwnerComponent, const string& strName);
	virtual ~CStateBase_Player() = default;

	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	virtual HRESULT Start(void* pArg, _bool bForce = false) override;
	virtual void	Update(const _float fTimeDelta) override;
	virtual HRESULT End() override;
	
protected:
	Flags					m_FMoves		= { 0 };
	vector<_uint>			m_vecChangeState_ByKey;

	TIME_COUNTER			m_tKeyTimer		= {};

	STATE_START_DESC		m_tNextStateDesc = {};

protected:
	virtual void OwnMove(const _float fTimeDelta) {};		// state 내부에서 알아서 움직일때
	virtual void Set_NextStateDesc(_uint iNextState) {};	// 다음 state에 따라 desc을 작성한다 : 각 state 내부에서

	virtual void CheckAni_WhenStart() {};					// 만약 자체에서 로직을 통해 바꾸고 싶다면
	virtual void Change_State(STATEKEY eKey);				// change 랩핑 함수 : 필요시 오버라이드

private:
	_uint					m_iEndStateIdx = { 0 };

	// state가 변환 했다면 true
private:
	_bool Check_MoveKey(const _float fTimeDelta);
	_bool Check_JumpKey(const _float fTimeDelta);
	_bool Check_DashKey(const _float fTimeDelta);
	_bool Check_CtrlPressKey(const _float fTimeDelta);
	_bool Check_CtrlUpKey(const _float fTimeDelta);

	_bool Has_ChangeState(STATEKEY eKey);

public:
	virtual void Free() override;
};

NS_END