#include "pch.h"
#include "StateBase_Player.h"
#include "Client_EventDefine.h"

// has?
#include "Player.h"
#include "PlayerControlContext.h"
#include "Transform.h"
#include "Gun.h"
#include "PlayerActionState.h"

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
	m_FCollisions					= pDesc->FCollis;
	m_vecChangeState_ByKey			= std::move(pDesc->vecChangeState_ByKey);

	m_tKeyTimer						= pDesc->tKeyTimer;

	m_pOwnerGun = pDesc->pOwnerGun;
	//if (m_pOwnerGun)
	//	Safe_AddRef(m_pOwnerGun);

	m_iEndStateIdx = ENUM_TO_UINT(CPlayer::State::END);

	return S_OK;
}

HRESULT CStateBase_Player::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateBase_Player::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_tKeyTimer.fTimeAcc = 0.f;

	m_TFallingCount.x = 0.f;
	m_TChargeCount.x = 0.f;

	return S_OK;
}

void CStateBase_Player::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// hit 충돌 처리 먼저
	if (Check_Hit(fTimeDelta))
		return;

	// 만약 이전 애니메이션때 변화하기 싫은데 아직 preAni가 끝나지 않았다면 : key 입력 처리를 하지 않음
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreNonEvent) &&
		!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
		return;
	 
	// keyCount를 하지 않거나, coolTime이 다 되었다면 : key 입력을 처리하자
	if (Can_CheckKey(fTimeDelta))
	{
		if (!m_bLoop && Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면
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

		if (Check_MeleeKey(fTimeDelta))
			return;

		if (Check_RangeKey(fTimeDelta))
			return;

		if (Check_SkillKey(fTimeDelta))
			return;
	}

	Check_Collis(fTimeDelta);
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

void CStateBase_Player::Change_PlayerState(_uint iState)
{
	Set_NextStateDesc(iState);
	Request_Change_State(iState, &m_tNextStateDesc);
}

void CStateBase_Player::Change_PlayerHitState(_uint iState, void* pArg)
{
	Request_Change_State(iState, pArg);
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
		// move key input이 없을 때
		if (!Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
		{
			Change_PlayerState(STATEKEY::MOVE);
			return true;
		}
	}

	return false;
}

_bool CStateBase_Player::Check_JumpKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SPACE) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::JUMP)))
	{
		// 벽이랑 충돌했는지 먼저 검사
		if (!Engine_Utils::Has_Flag(m_FCollisions, C_WALL_NO) &&
			IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_SIDES))
		{
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::JUMPWALL));
			return true;
		}

		// 만약 jump double을 할거라면 한번 체크
		if (m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::SPACE)] == ENUM_TO_UINT(CPlayer::State::JUMPDOUBLE))
		{
			if (Check_Double())
			{
				Change_PlayerState(STATEKEY::SPACE);
				return true;
			}

			return false;
		}

		// 아니라면 그냥 키전환
		else
		{
			Change_PlayerState(STATEKEY::SPACE);
			return true;
		}
	}
	
	return false;
}

_bool CStateBase_Player::Check_DashKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::SHIFT) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DASH)))
	{
		m_pGameInstance->Broadcast<PLAYER_SKILL_TRIGGERED>(ENUM_TO_UINT(STATEKEY::SHIFT));
		
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

_bool CStateBase_Player::Check_MeleeKey(const _float fTimeDelta)
{
	if (Has_ChangeState(STATEKEY::CHARGE) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::CHARGATT)))
	{
		m_TChargeCount.x += fTimeDelta;

		if (m_TChargeCount.x >= m_TChargeCount.y)
		{
			m_TChargeCount.x = 0.f;
			Change_PlayerState(STATEKEY::CHARGE);
			return true;
		}
	}

	/*else */if (Has_ChangeState(STATEKEY::LM) &&
	Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::LATT)))
	{
		if (Check_OnGround(0.3f))
		{
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::COMBO));
		}

		else
		{
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::JUMPATTSTART));
		}
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_RangeKey(const _float fTimeDelta)
{
	//Prototype 이후 고칠 것
	
	if (Has_ChangeState(STATEKEY::RM) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::RATT)))
	{
		if (m_pOwnerGun)
		{
			// 공격이 가능 하다면 : attack
			if (Can_Fire())
			{
				Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNATTACK));
				return true;
			}

			// 공격은 불가능 하지만 reload는 가능 하다면 : reload
			else if (Can_Reload())
			{
				Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNRELOAD));
				return true;
			}

			return false;
		}
	}

	return false;
}

_bool CStateBase_Player::Check_SkillKey(const _float fTimeDelta)
{
	// key를 가지고 있고 &&
	// key를 눌렀고 &&
	// 스킬을 실행할 수 있다면
	if (Has_ChangeState(STATEKEY::E) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SKILL1)) &&
		static_cast<CPlayer*>(Get_OwnerObject())->Start_Attack(CPlayer::State::SKILL1))
	{
		m_pGameInstance->Broadcast<PLAYER_SKILL_TRIGGERED>(ENUM_TO_UINT(STATEKEY::E));

		Change_PlayerState(STATEKEY::E);
		return true;
	}

	else if(Has_ChangeState(STATEKEY::Q) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SKILL2)) &&
		static_cast<CPlayer*>(Get_OwnerObject())->Start_Attack(CPlayer::State::SKILL2))
	{
		m_pGameInstance->Broadcast<PLAYER_SKILL_TRIGGERED>(ENUM_TO_UINT(STATEKEY::Q));

		Change_PlayerState(STATEKEY::Q);
		return true;
	}

	return false;
}

_bool CStateBase_Player::Check_Hit(const _float fTimeDelta)
{
	CPlayerActionState* pActionState = static_cast<CPlayerActionState*>(m_pOwnerStateComp);
	// hit 검사 할건디
	// action state에서 hit 되었는지 체크를 한번 한다
	if (Can_BeAttacked() && pActionState->Is_OnHit())
	{
		Flags fAttackFlag = pActionState->Get_AttackFlag();

		HITSTATE_START_DESC tStartDesc = {};
		tStartDesc.vHitDir = pActionState->Get_HitNormal();
		tStartDesc.vVicPos = pActionState->Get_VicPosition();


		// 해당 hit collision을 체크 할 거고 : state 권한
		// 해당 hit가 들어왔다면 : action state에서 처리 -> 아님 플레이어?

				// strong은 이제 지면 충돌 검사 후 fly, strong 전환 
		if (Engine_Utils::Has_Flag(m_FCollisions, COLLISIONFLAGS::C_Strong) &&
			Engine_Utils::Has_Flag(fAttackFlag, CPlayerActionState::AttackFlag::AF_Strong))
		{
			// 충돌 방향 desc 넘겨줌
			if (Check_OnGround(0.3f))
			{
				Change_PlayerHitState(ENUM_TO_UINT(CPlayer::State::HITSTRONG), &tStartDesc);
			}

			else
			{
				Change_PlayerHitState(ENUM_TO_UINT(CPlayer::State::HITFLYSTART), &tStartDesc);
			}

			return true;
		}
		
		if (Engine_Utils::Has_Flag(m_FCollisions, COLLISIONFLAGS::C_Fly) &&
			Engine_Utils::Has_Flag(fAttackFlag, CPlayerActionState::AttackFlag::AF_Fly))
		{
			// 충돌 방향 desc 넘겨줌
			Change_PlayerHitState(ENUM_TO_UINT(CPlayer::State::HITFLYSTART) ,&tStartDesc);

			return true;
		}



		if (Engine_Utils::Has_Flag(m_FCollisions, COLLISIONFLAGS::C_Addtive) &&
			Engine_Utils::Has_Flag(fAttackFlag, CPlayerActionState::AttackFlag::AF_Addtive))
		{
			Change_PlayerHitState(ENUM_TO_UINT(CPlayer::State::HITADDTIVE));
			return true;
		}
	}

	return false;
}

_bool CStateBase_Player::Check_Collis(const _float fTimeDelta)
{
	// 플래그 먼저 확인
	if (Engine_Utils::Has_Flag(m_FCollisions, COLLISIONFLAGS::C_DOWN))
	{
		// 충돌 검사 및 시간 누적
		if (Check_OnGround())
			m_TFallingCount.x = 0.f;
		else
			m_TFallingCount.x += fTimeDelta;

		if(m_TFallingCount.x > m_TFallingCount.y)
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
	}

	return false;
}

_bool CStateBase_Player::Check_OnGround(_float fMaxDist)
{
	return static_cast<CPlayer*>(Get_OwnerObject())->Check_OnGround(fMaxDist);
}

void CStateBase_Player::Check_Monster()
{
	// 몬스터랑 출동 했다면
	if (Check_ColliWithMonster())
	{
		// 몬스터를 향하게 turn

		// combo 카운트 업
		Count_Combo();
	}
}

void CStateBase_Player::Change_Weapon(_uint iPart, _uint iState)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if (pPlayer)
	{
		pPlayer->Change_Weapon(iPart, iState);
	}
}

_bool CStateBase_Player::Start_Att(_uint iPlayerState)
{
	return static_cast<CPlayer*>(Get_OwnerObject())->Start_Attack(static_cast<CPlayer::State>(iPlayerState));
}

void CStateBase_Player::End_Att(_uint iPlayerState)
{
	static_cast<CPlayer*>(Get_OwnerObject())->End_Attack(static_cast<CPlayer::State>(iPlayerState));
}

void CStateBase_Player::Set_RootMotion_Apply(_bool bApply)
{
	static_cast<CPlayer*>(Get_OwnerObject())->Set_RootMotion_Apply(bApply);
}

void CStateBase_Player::Set_DoubleJumpCount(_bool bCount)
{
	static_cast<CPlayer*>(Get_OwnerObject())->Set_DoubleJumpCount(bCount);
}

_bool CStateBase_Player::Check_Double()
{
	return static_cast<CPlayer*>(Get_OwnerObject())->Check_DoubleJump();
}

_bool CStateBase_Player::Can_Fire()
{
	if (!m_pOwnerGun)
		return false;

	return m_pOwnerGun->Get_CanFire();
}

_bool CStateBase_Player::Can_Reload()
{
	if (!m_pOwnerGun)
		return false;

	return m_pOwnerGun->Get_CanReleod();
}

void CStateBase_Player::Set_GunTimer(_bool bOn)
{
	m_pOwnerGun->Set_FireTimer(bOn);
}

void CStateBase_Player::Reset_GunTimer()
{
	m_pOwnerGun->Reset_FireTimer();
}

void CStateBase_Player::Reload_Gun()
{
	m_pOwnerGun->Reload_Bullet();
}

_bool CStateBase_Player::Can_CheckKey(const _float fTimeDelta)
{
	return (!(m_tKeyTimer.bCountTime) ||
		m_tKeyTimer.CountTime(fTimeDelta) == 1.f);
}

_bool CStateBase_Player::Has_ChangeState(STATEKEY eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndStateIdx != m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
}

_bool CStateBase_Player::Check_ColliWithMonster()
{
	return static_cast<CPlayer*>(Get_OwnerObject())->Check_ColliWithMonster();
}

void CStateBase_Player::Count_Combo()
{
	static_cast<CPlayer*>(Get_OwnerObject())->Count_Combo();
}

void CStateBase_Player::Free()
{
	//Safe_Release(m_pOwnerGun);
	Super::Free();
}