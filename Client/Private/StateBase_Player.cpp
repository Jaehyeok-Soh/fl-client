#include "pch.h"
#include "StateBase_Player.h"

// has?
#include "Player.h"

// manager
#include "GameInstance.h"
#include "ControlContext.h"
#include "Engine_Utils.h"

CStateBase_Player::CStateBase_Player(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CStateBase_Player::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	PLAYER_STATEBASE_DESC* pDesc	= static_cast<PLAYER_STATEBASE_DESC*>(pArg);

	m_FMoves						= pDesc->FMoves;
	m_vecChangeState_ByKey			= std::move(pDesc->vecChangeState_ByKey);

	m_tKeyTimer						= pDesc->tKeyTimer;

	return S_OK;
}

HRESULT CStateBase_Player::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	m_iEndStateIdx = ENUM_TO_UINT(CPlayer::State::END);

	return S_OK;
}

HRESULT CStateBase_Player::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_tKeyTimer.fTimeAcc = 0.f;

	return S_OK;
}

void CStateBase_Player::Update(const _float fTimeDelta)
{
//#ifdef _DEBUG
//	//WINDOW_DEBUG
//	std::wstring msg = L"State: ";
//	std::wstring ws(m_strName.begin(), m_strName.end());
//	msg += ws;
//
//	msg += L" / AniIdx: ";
//	msg += std::to_wstring(m_iMainAnimIdx);
//	SetWindowText(g_hWnd, msg.c_str());
//#endif

	Super::Update(fTimeDelta);

	// 만약 이전 애니메이션때 변화하기 싫은데 아직 preAni가 끝나지 않았다면 : key 입력 처리를 하지 않음
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreNonEvent) &&
		!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
		return;
	 
	// keyCount를 하지 않거나, coolTime이 다 되었다면 : key 입력을 처리하자
	if (!(m_tKeyTimer.bCountTime) ||
		m_tKeyTimer.CountTime(fTimeDelta) == 1.f)
	{
		if (!m_bLoop && Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면 : pre animation이랑 잘 해야될듯..?
		{
			Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
			return;
		}

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

	return S_OK;
}

void CStateBase_Player::Change_PlayerState(STATEKEY eKey)
{
	_uint iNextState = m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
	Set_NextStateDesc(iNextState);		// next state에 대한 desc 작성
	Request_Change_State(iNextState, &m_tNextStateDesc);	

	/* 플레이어가 이런 state를 이런 애니메이션으로 바꿨다 */
}

_bool CStateBase_Player::Check_MoveKey(const _float fTimeDelta)
{
	if (Engine_Utils::Has_Flag(m_FMoves, MOVEFLAGS::OWN))
	{
		OwnMove(fTimeDelta);
	}

	if (Engine_Utils::Has_Flag(m_FMoves, MOVEFLAGS::PRESS_CHANGE))
	{
		Set_NextStateDesc(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::MOVE)]);
		return (Align_Move(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::MOVE)]));
	}

	if (Engine_Utils::Has_Flag(m_FMoves, MOVEFLAGS::NORMAL))
	{
		if (Align_Movement(fTimeDelta) == false								// 8방향 움직임을 하는데 입력이 없고
			&&
			Has_ChangeState(STATEKEY::MOVE))								// change move 키가 있다면
				
		{
			// change state
			Change_PlayerState(STATEKEY::MOVE);
			return true;
		}
	}

	if (Engine_Utils::Has_Flag(m_FMoves, MOVEFLAGS::UP_CHANGE))
	{
		
	}

	return false;
}

_bool CStateBase_Player::Check_JumpKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SPACE) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::JUMP)))
	{
		Change_PlayerState(STATEKEY::SPACE);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_DashKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SHIFT) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DASH)))
	{
		Change_PlayerState(STATEKEY::SHIFT);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_CtrlPressKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::LCRTL_PRESS) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SPECIALMV)))
	{
		Change_PlayerState(STATEKEY::LCRTL_PRESS);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_CtrlUpKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::LCRTL_UP) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DODGE)))
	{
		Change_PlayerState(STATEKEY::LCRTL_UP);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Has_ChangeState(STATEKEY eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndStateIdx != m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
}

void CStateBase_Player::Free()
{
	Super::Free();
}