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
#include "ControlContext.h"
#include "GameInstance.h"

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
	m_FWeaponChanges				= pDesc->FWeaponChanges;
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

	Reset_WhenStart();

	return S_OK;
}

void CStateBase_Player::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// 만약 이전  프레임에 weapon change key가 눌렸다면 weapon을 change
	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_NextFrame))
		Change_Weapon();

	if (Check_Collis(fTimeDelta))
		return;

	// hit 충돌 처리 먼저
	if (Check_Hit(fTimeDelta))
		return;

	// 만약 이전 애니메이션때 변화하기 싫은데 아직 preAni가 끝나지 않았다면 : key 입력 처리를 하지 않음
	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreNonEvent) &&
		!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
		return;

	if (Change_State_WhenLoopDone(fTimeDelta))
		return;
	 
	// keyCount를 하지 않거나, coolTime이 다 되었다면 : key 입력을 처리하자
	if (Check_Keys(fTimeDelta))
		return;

	// weapon key check / todo_eunbi : check key안으로 일단 넣지는 않음
	if (Check_WeaponChnage(fTimeDelta))
		return;
}

HRESULT CStateBase_Player::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	// 만약 이전  프레임에 weapon change key가 눌렸다면 weapon을 change
	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_End))
		Change_Weapon();

	return S_OK;
}

void CStateBase_Player::Change_PlayerState(STATEKEY eKey, _bool bForce)
{
	_uint iNextState = m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
	Set_NextStateDesc(iNextState);		// next state에 대한 desc 작성

	if (bForce)
		Request_Change_StateForce(iNextState, &m_tNextStateDesc);
	else
		Request_Change_State(iNextState, &m_tNextStateDesc);
}

void CStateBase_Player::Change_PlayerState(_uint iState, _bool bForce)
{
	Set_NextStateDesc(iState);

	if(bForce)
		Request_Change_StateForce(iState,  &m_tNextStateDesc);
	else
		Request_Change_State(iState, &m_tNextStateDesc);
}

void CStateBase_Player::Change_PlayerHitState(_uint iState, void* pArg)
{
	Request_Change_State(iState, pArg);
}

_bool CStateBase_Player::Check_Keys(const _float fTimeDelta)
{
	// keyCount를 하지 않거나, coolTime이 다 되었다면 : key 입력을 처리하자
	if (Can_CheckKey(fTimeDelta))
	{
		if (Check_MoveKey(fTimeDelta))
			return true;

		if (Check_JumpKey(fTimeDelta))
			return true;

		if (Check_DashKey(fTimeDelta))
			return true;

		if (Check_CtrlPressKey(fTimeDelta))
			return true;

		if (Check_CtrlUpKey(fTimeDelta))
			return true;

		if (Check_MeleeKey(fTimeDelta))
			return true;

		if (Check_RangeKey(fTimeDelta))
			return true;

		if (Check_SkillKey(fTimeDelta))
			return true;

		if (Check_FKey(fTimeDelta))
			return true;
	}

	return false;
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

	if (!m_bLoop && Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면
	{
		// move key intput이 있고, 키 맵핑이 되어 있고, 땅에 있다면
		if (Engine_Utils::Has_Flag(m_FMoves, MOVEFLAGS::LOOP_DONE) &&
			Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE))&&
			Check_OnGround(0.3f))
		{
			if (Has_ChangeState(STATEKEY::LOOPDONEMOVEKEY))
			{
				Change_PlayerState(STATEKEY::LOOPDONEMOVEKEY);
				return true;
			}

			else if (Has_ChangeState(STATEKEY::MOVE))
			{
				Change_PlayerState(STATEKEY::MOVE);
				return true;
			}

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
	if (Can_UseWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE)))
	{
		// holding 시간 체크
		if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::CHARGATT)))
		{
			m_TChargeCount.x += fTimeDelta;

			if (Has_ChangeState(STATEKEY::CHARGE))
			{
				if (m_TChargeCount.x >= m_TChargeCount.y)
				{
					Change_PlayerState(STATEKEY::CHARGE);
					m_TChargeCount.x = 0.f;
					return true;
				}
			}
		}

		// 만약에 마우스를 땠다면
		else if (MOUSE_LBUTTON_UP)
		{
			if (Has_ChangeState(STATEKEY::CHARGE))
			{
				if (m_TChargeCount.x >= m_TChargeCount.y)
				{
					Change_PlayerState(STATEKEY::CHARGE);
					return true;
				}
			}

			if (Has_ChangeState(STATEKEY::LM))
			{
				if (Check_OnGround(0.3f))
				{
					_int iCurMelee = Get_WeaponIdx(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE));

					switch (iCurMelee)
					{
					case ENUM_TO_UINT(CPlayer::MELEE::SWORD):
						Change_PlayerState(ENUM_TO_UINT(CPlayer::State::COMBO));
						break;

					case ENUM_TO_UINT(CPlayer::MELEE::DUAL):
						//Change_PlayerState(ENUM_TO_UINT(CPlayer::State::COMBO_DUAL));
						break;
					}

				}

				else
				{
					Change_PlayerState(ENUM_TO_UINT(CPlayer::State::JUMPATTSTART));
				}
				return true;
			}

			m_TChargeCount.x = 0.f;
		}
	}

	return false;
}

_bool CStateBase_Player::Check_RangeKey(const _float fTimeDelta)
{
	if (Can_UseWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE))&&
		Has_ChangeState(STATEKEY::RM) &&
		Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::RATT))&&
		static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Can_ChangeGunState()
		)
	{
		if (m_pOwnerGun)
		{
			// 공격이 가능 하다면 : attack
			if (Can_Fire())
			{
				Change_PlayerState(ENUM_TO_UINT(CPlayer::State::GUNATTACK));
				//Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNATTACK));
				return true;
			}

			// 공격은 불가능 하지만 reload는 가능 하다면 : reload
			else if (Can_Reload())
			{
				Change_PlayerState(ENUM_TO_UINT(CPlayer::State::GUNRELOAD));
				//Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNRELOAD));
				return true;
			}

			return false;
		}
	}

	return false;
}

_bool CStateBase_Player::Check_SkillKey(const _float fTimeDelta)
{
	if (Can_UseWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL)))
	{
		// key를 가지고 있고 &&
		// key를 눌렀고 &&
		// 스킬을 실행할 수 있다면
		if (Has_ChangeState(STATEKEY::E) &&
			Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SKILL1)) &&
			static_cast<CPlayer*>(Get_OwnerObject())->Start_Attack(CPlayer::State::SKILL1))
		{
			m_pGameInstance->Broadcast<PLAYER_SKILL_TRIGGERED>(ENUM_TO_UINT(STATEKEY::E));

			Change_PlayerState(STATEKEY::E, true);
			return true;
		}

		else if (Has_ChangeState(STATEKEY::Q) &&
			Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SKILL2)) &&
			static_cast<CPlayer*>(Get_OwnerObject())->Start_Attack(CPlayer::State::SKILL2))
		{
			m_pGameInstance->Broadcast<PLAYER_SKILL_TRIGGERED>(ENUM_TO_UINT(STATEKEY::Q));

			Change_PlayerState(STATEKEY::Q, true);
			return true;
		}
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

_bool CStateBase_Player::Check_FKey(const _float fTimeDelta)
{
	if (static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Can_FKeyEvent() &&												// event가 켜졌는디
		Engine_Utils::Has_Flag(m_FCollisions, COLLISIONFLAGS::C_CheckF) &&			// 이번 state에서 f키 check 할건디
		KEY_BUTTON_DOWN(DIK_F))														// f키를 눌렀다면
	{
		//todo iKeyEvent 검사후 state 관리
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::CONDEMN));
		return true;
	}

	else if (KEY_BUTTON_DOWN(DIK_F))
		int a = 0;

	return false;
}

_bool CStateBase_Player::Check_WeaponChnage(const _float fTimeDelta)
{
	// key check만 한다
	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_Check))
	{
		if (KEY_BUTTON_DOWN(DIK_Z))
		{
			m_FWeaponChanges |= WEAPONCHANGEFLAGS::Change_Melee;

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

		if (m_TFallingCount.x > m_TFallingCount.y)
		{
			Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
			return true;
		}
	}

	return false;
}

void CStateBase_Player::Jump_Impuls(_float fOffset)
{
	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	_float moveps = pPlayerTrans->Get_MovePerSec(); // 속도

	Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));
	vUp.Normalize();

	Vec3 accelation = vUp * moveps * fOffset; //  방향 * 속도

	SetCCTImpuls(accelation);
}

void CStateBase_Player::Look_Impuls(_float fOffset)
{
	CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
	_float moveps = pPlayerTrans->Get_MovePerSec(); // 속도

	Vec3 vLook= (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));
	vLook.Normalize();

	Vec3 accelation = vLook * moveps * fOffset; //  방향 * 속도

	SetCCTImpuls(accelation);
}

void CStateBase_Player::LookAt_Monser()
{
	Vec3 vMonsterPos = Vec3::Zero;
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if (pPlayer == nullptr)
		return;

	CTransform* pPlayerTransform = pPlayer->Get_Component<CTransform>();
	if (pPlayerTransform == nullptr)
		return;

	vMonsterPos = pPlayer->Get_CollidedMonster_Position();
	if (vMonsterPos != Vec3::Zero)
	{
		pPlayerTransform->Look_At_XZ(vMonsterPos);
	}
}

_bool CStateBase_Player::Check_OnGround(_float fMaxDist)
{
	return static_cast<CPlayer*>(Get_OwnerObject())->Check_OnGround(fMaxDist);
}

void CStateBase_Player::Change_WeaponState(_uint iPart, _uint iState)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if(pPlayer)
		pPlayer->Change_WeaponState(iPart, iState);
}

_bool CStateBase_Player::Change_Weapon()
{
	// change 플래그가 켜졌다면
	// next로 바뀔수 있다면 weapon index 바꾸고
	// 해당 weapon hold state로 변경
	_bool bChange = false;
	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_Melee) &&
		Can_ChangeNextWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE)))
	{
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));
		bChange = true;
	}

	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_Gun) &&
		Can_ChangeNextWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE)))
	{
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HOLD));
		bChange = true;
	}

	if (Engine_Utils::Has_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Change_Skill) &&
		Can_ChangeNextWeapon(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL)))
	{
		// skill 검은 hold를 하지 않음
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL), ENUM_TO_UINT(CWeapon::State::NONE));
		bChange = true;
	}

	// 플래그 아예 꺼주기
	Engine_Utils::RemoveHard_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Mask_ChangeWeapons);

	return bChange;
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

_int CStateBase_Player::Get_WeaponIdx(_uint iWeaponType)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if (pPlayer)
		return pPlayer->Get_CurWeaponIdx(iWeaponType);

	return -1;
}

_bool CStateBase_Player::Can_UseWeapon(_uint iWeaponType)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if (pPlayer)
		return pPlayer->Can_UseWeapon(iWeaponType);

	return false;
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

_bool CStateBase_Player::Change_State_WhenLoopDone(const _float fTimeDelta)
{
	if (!m_bLoop && Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면
	{
		if (Check_Collis(fTimeDelta))
			return true;

		if (Check_Keys(fTimeDelta))
			return true;

		Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
		return true;
	}

	return false;
}

void CStateBase_Player::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::JUMPATTSTART):
	case ENUM_TO_UINT(CPlayer::State::JUMPATTEND):
	case ENUM_TO_UINT(CPlayer::State::CHARGE):
		m_tNextStateDesc.iMainAnimIdx = Get_WeaponIdx(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE)); break;

	case ENUM_TO_UINT(CPlayer::State::GUNATTACK):
	case ENUM_TO_UINT(CPlayer::State::GUNRELOAD):
		m_tNextStateDesc.iMainAnimIdx = Get_WeaponIdx(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE)); break;

	case ENUM_TO_UINT(CPlayer::State::SKILL1):
	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		m_tNextStateDesc.iMainAnimIdx = Get_WeaponIdx(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL)); break;

	defualt:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}

_bool CStateBase_Player::Can_CheckKey(const _float fTimeDelta)
{
	return (!(m_tKeyTimer.bCountTime) ||
		m_tKeyTimer.CountTime(fTimeDelta) == 1.f);
}

void CStateBase_Player::Reset_WhenStart()
{
	m_tKeyTimer.fTimeAcc = 0.f;

	m_TFallingCount.x = 0.f;
	m_TChargeCount.x = 0.f;

	Engine_Utils::RemoveHard_Flag(m_FWeaponChanges, WEAPONCHANGEFLAGS::Mask_ChangeWeapons);
}

HRESULT CStateBase_Player::Start_AttackState(void* pArg)
{
	// attack은 pre state 기준으로 애니메이션 재생 x
	// 이전 state에서 현재 weapon 을 기준으로 설정해준다

	STATE_START_DESC* pDesc = static_cast<STATE_START_DESC*>(pArg);
	m_iMainAnimIdx = pDesc->iMainAnimIdx;
	
	if (m_iMainAnimIdx < 0)
		return E_FAIL;

	if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_HasPreAni))
	{
		Engine_Utils::RemoveHard_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone);
		Request_ChangeAnimation(m_vecPreAnims[m_iMainAnimIdx].iAnimationIdex, true, false, true); // 무조건 loop : false
	}

	else
	{
		Engine_Utils::Add_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone);
		Request_ChangeAnimation((size_t)m_vecMainAnims[m_iMainAnimIdx], m_bBlend, m_bLoop);
	}

	Reset_WhenStart();

	return S_OK;
}

_bool CStateBase_Player::Has_ChangeState(STATEKEY eKey)
{
	// state end 이면 state change를 안 한다
	return m_iEndStateIdx != m_vecChangeState_ByKey[ENUM_TO_UINT(eKey)];
}

_bool CStateBase_Player::Can_ChangeNextWeapon(_uint iWeaponType)
{
	CPlayer* pPlayer = static_cast<CPlayer*>(Get_OwnerObject());
	if (pPlayer)
		return pPlayer->Change_MainWeaponNext(iWeaponType);

	return false;
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