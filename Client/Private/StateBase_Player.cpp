#include "pch.h"
#include "StateBase_Player.h"

// has?
#include "Player.h"

// manager
#include "GameInstance.h"
#include "ControlContext.h"

CStateBase_Player::CStateBase_Player(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CStateBase_Player::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_STATEBASE_DESC* pDesc = static_cast<PLAYER_STATEBASE_DESC*>(pArg);

	m_eMoveType = pDesc->eMoveType;
	m_iNextState = pDesc->iNextState;
	m_vecChangeState_ByKey = std::move(pDesc->vecChangeState_ByKey);

	m_tKeyTimer = pDesc->tKeyTimer;

	return S_OK;
}

HRESULT CStateBase_Player::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	m_iEndState = ENUM_TO_UINT(CPlayer::State::END);

	return S_OK;
}

HRESULT CStateBase_Player::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateBase_Player::Update(const _float fTimeDelta)
{
#ifdef _DEBUG
	std::wstring msg = L"State: ";
	std::wstring ws(m_strName.begin(), m_strName.end());
	msg += ws;
	SetWindowText(g_hWnd, msg.c_str());
#endif

	Super::Update(fTimeDelta);

	// keyCount를 하지 않거나, coolTime이 다 되었다면
	if (!(m_tKeyTimer.bCountTime) ||
		m_tKeyTimer.CountTime(fTimeDelta) == 1.f)
	{
		if (Check_MoveKey(fTimeDelta))
			return;

		if (Check_JumpKey(fTimeDelta))
			return;

		if (Check_DashKey(fTimeDelta))
			return;

		if (Check_CtrlPressKey(fTimeDelta))
			return;

		if (Check_CtrlUpKey(fTimeDelta))
			return;
	}
}

HRESULT CStateBase_Player::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_tKeyTimer.fTimeAcc = 0.f;

	return S_OK;
}

_bool CStateBase_Player::Check_MoveKey(const _float fTimeDelta)
{
	//	enum class MOVETYPE { NORMAL, CHANGE, OWN, NON }; // 8방향 움직임, state change, own moving, dont move
	switch (m_eMoveType)
	{
	case MOVETYPE::NORMAL:
		if (Align_Movement(fTimeDelta) == false)	// 8방향 움직임 
		{
			Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::MOVE)]); // 움직임이 없다면 실행할 
			return true;
		}

		break;

	case MOVETYPE::CHANGE: // 키가 눌렸다면 해당 state로 change
		return (Align_Move(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::MOVE)]));

	case MOVETYPE::OWN:		// 내 움직임
		OwnMove(fTimeDelta); 
		break;

	case MOVETYPE::NON:		// 움직이지 않음
		break;
	}

	if (!m_bLoop && Is_MainAnimFinished()) // loop가 아닌데 애니메이션이 끝났다면 : pre animation이랑 잘 해야될듯..?
	{
		Request_Change_State(m_iNextState);		// 다음 state로 change
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_JumpKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SPACE) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::JUMP)))
	{
		Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::SPACE)]);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_DashKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SHIFT) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DASH)))
	{
		Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::SHIFT)]);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_CtrlPressKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::LCRTL_PRESS) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SPECIALMV)))
	{
		Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::LCRTL_PRESS)]);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_CtrlUpKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::LCRTL_UP) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DODGE)))
	{
		Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::LCRTL_UP)]);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Has_ChangeState(STATEKEY eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndState != m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
}

void CStateBase_Player::Free()
{
	Super::Free();
}