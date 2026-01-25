#pragma once
#include "StateBase.h"

// player의 state들이 공통적으로 처리해야하는 것들을 다루기 위해
// 부모 statebase를 하나 생성

NS_BEGIN(Client)

class CStateBase_Player abstract : public CStateBase
{
	using Super = CStateBase;

public:
	enum class MOVETYPE { NORMAL, CHANGE, OWN, NON }; // 8방향 움직임, state change, own moving, dont move
	enum class STATEKEY : _uint {MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, END};

	typedef struct tagPlayerStateDesc : public CStateBase::STATE_DESC
	{
		MOVETYPE				eMoveType = { MOVETYPE::NORMAL };
		_uint					iNextState = { 0 };			// loop이면 키 입력이 없을때 바뀔 state, no loop라면 animation 끝나고 바뀔 state
		vector<_uint>			vecChangeState_ByKey;			// 키 입력에 따라 어떻게 바꿀지 담는 벡터

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
	MOVETYPE				m_eMoveType = { MOVETYPE::NORMAL };
	_uint					m_iNextState = { 0 };
	vector<_uint>	m_vecChangeState_ByKey;
	_uint					m_iEndState		= { 0 };

protected:
	virtual void OwnMove(const _float fTimeDelta) {}; // state 내부에서 알아서 움직일때

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