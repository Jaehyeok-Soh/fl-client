#include "pch.h"
#include "Client_Defines.h"
#include "RemotePlayer.h"
#include "GameInstance.h"
#include "Body.h"
#include "Model.h"
#include "CharacterSyncPacket.h"

// objects
#include "Weapon.h"
#include "EffectHandler.h"
#include "Gun.h"

// components
#include "MyStat.h"
#include "StatCom_Player.h"
#include "ActionSkill.h"
#include "SkillBase_MoonE.h"
#include "SkillBase_MoonQ.h"
#include "RenderFx.h"
#include "PlayerActionState.h"

#pragma region State
#include "State_MoonCombo.h"
#include "State_DualCombo.h"

#include "State_JumpAttStart.h"
#include "State_JumpAttEnd.h"

#include "State_Charge.h"
#include "State_MoonCharge.h"

#include "State_GunAttack.h"
#include "State_GunIdle.h"
#include "State_GunReload.h"
#include "State_GunWalk.h"

#include "State_MoonSkill.h"

// system
#include "SoundEventBinder.h"

#define ANIMTPS     24.f
#define ANIMTIC     (24.f * 1.2f)
#define ANIMTIC_3   (24.f * 1.3f)

CRemotePlayer::CRemotePlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPlayer{pDevice, pDeviceContext}
{
}

CRemotePlayer::CRemotePlayer(const CRemotePlayer& rhs)
	: CPlayer(rhs)
{
}

HRESULT CRemotePlayer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	Set_Name("Remote_Player");

    REMOTE_PLAYER_DESC* pDesc = static_cast<REMOTE_PLAYER_DESC*>(pArg);
    m_tagUserData = pDesc->tUserModel;

	if (FAILED(Ready_EffectEvent()))
		return E_FAIL;

	if (FAILED(Ready_SoundHandler()))
		return E_FAIL;

	Get_Component<CEffectHandler>()->Setup_ForOwner(this, Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>());

	return S_OK;
}

HRESULT CRemotePlayer::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(CContainerObject::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_pGameInstance->Add_Actor_Object(this);

	return S_OK;
}

HRESULT CRemotePlayer::Clear_WhenChangeLevel()
{
	Clear_Components_WhenChangeLevel();

	for (auto& pPartObj : m_vecPartObjects)
	{
		if (pPartObj)
			pPartObj->Clear_WhenChangeLevel();
	}

	Set_Active(false);
	Set_CollideEnabled(false);
	Set_Render(false);

	return S_OK;
}

void CRemotePlayer::Update_Priority(const _float fTimeDelta)
{
	CContainerObject::Update_Priority(fTimeDelta);

	CTransform* pTransform = Get_Component<CTransform>();
	if (pTransform != nullptr)
	{
		_float fLerpFactor = m_fInterpolationSpeed * fTimeDelta;
		if (fLerpFactor > 1.f)
			fLerpFactor = 1.f;

		Vec3 vCurPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
		Vec3 vNextPos = Vec3::Lerp(vCurPos, m_vServerTargetpos, fLerpFactor);
		pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vNextPos);

		Quat currentQuat = Quat::CreateFromRotationMatrix(pTransform->Get_WorldMatrix());
		Quat targetQuat = Quat::CreateFromYawPitchRoll(m_fServerTargetYaw, 0.f, 0.f);
		Quat nextQuat = Quat::Slerp(currentQuat, targetQuat, fLerpFactor);
		pTransform->Rotation(nextQuat);
	}
}

void CRemotePlayer::Update(const _float fTimeDelta)
{
	CContainerObject::Update(fTimeDelta);
}

void CRemotePlayer::Update_Late(const _float fTimeDelta)
{
	CContainerObject::Update_Late(fTimeDelta);

	Synchronize_State();
}

void CRemotePlayer::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CRemotePlayer::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CRemotePlayer::Synchronize_State()
{
	CHARACTER_SYNC_PACKET charSyncPkt = m_pGameInstance->GetUserSyncData(m_tagUserData.ClientIndex);

	m_vServerTargetpos = Vec3(charSyncPkt.PosX, charSyncPkt.PosY, charSyncPkt.PosZ);
	m_fServerTargetYaw = charSyncPkt.RotY;
}

HRESULT CRemotePlayer::Ready_Ability()
{
    CSkillBase* pESkill = CSkillBase_MoonE::Create();
    CSkillBase* pQSkill = CSkillBase_MoonQ::Create();

    // stat
    {
        CStatCom_Player::PLAYER_STATCOMP_DESC desc = {};
        desc.fMaxHp = 8430.f;
        desc.fDefense = 3200.f;
        desc.fMental = 620.f; // 105
        desc.fHpUpdate_Rate = 2.f;
        desc.FStatFlags = CStatCom_Player::StatFlags::DefenseUpdtae | CStatCom_Player::StatFlags::MentalUpdate | CStatCom_Player::StatFlags::HpUpdate;

        desc.fComboCoolTime = 7.f;
        desc.fDashCoolTime = 2.f;

        desc.fMeleeAttack = 1000.f;
        desc.fGunAttack = 150.f;

        desc.pESkill = pESkill;
        desc.pQSkill = pQSkill;

        vector<_uint> vecComputeOrder;
        {
            vecComputeOrder.resize(4);
            vecComputeOrder[0] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Add);
            vecComputeOrder[1] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Add);
            vecComputeOrder[2] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Normal_Rate);
            vecComputeOrder[3] = ENUM_TO_UINT(EXTRA_ATTACK_DESC::ComputeOrder::Random_Rate);
        }

        desc.vecExtraComputeOrder = vecComputeOrder;
        desc.fCriticalAttack = 30.f;
        desc.fCriticalRate = 0.1f;

        if (FAILED(Add_Component<CMyStat>(0/* STATIC */, L"Prototype_Component_Stat_Player", &desc)))
            return E_FAIL;
    }

    // skill
    {
        CActionSkill::ACTIONSKILL_DESC desc = {};
        desc.iSkillCount = 2;
        desc.pOwnerStat = Get_Component<CMyStat>();

        if (FAILED(Add_Component<CActionSkill>(0/* STATIC */, L"Prototype_Component_ActionSkill", &desc)))
            return E_FAIL;

        CActionSkill* pActionSkill = Get_Component<CActionSkill>();

        if (FAILED(pActionSkill->Add_Skill(Skill::MoonE, pESkill)))
            return E_FAIL;
        if (FAILED(pActionSkill->Add_Skill(Skill::MoonQ, pQSkill)))
            return E_FAIL;
    }

    Start_Attack(State::COMBO);

    return S_OK;
}

HRESULT CRemotePlayer::Ready_EffectEvent()
{
	if (FAILED(Add_Component<CEffectHandler>(0, L"Prototype_Component_EffectHandler_PlayerMoon", nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRemotePlayer::Ready_SoundHandler()
{
	_uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
	CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;
	CModel* pAnimModel = pBody->Get_Component<CModel>();
	if (pAnimModel == nullptr)
		return E_FAIL;
	// 내부에서 Add_Component 해줌
	CSoundEventBinder* pResult = CSoundEventBinder::Create(iLevelID, this, pAnimModel, L"../../Resources/Data/SoundAnimationData/Player.json");
	if (pResult == nullptr)
		return E_FAIL;
	Safe_Release(pResult);
	return S_OK;
}

HRESULT CRemotePlayer::Ready_AttackStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    CComputeShader* pAnimMixCS = static_cast<CBody*>(Get_Part<CBody>(ENUM_TO_UINT(Part::BODY)))->Get_AnimMixCS();

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    CGun* pMyGun = static_cast<CGun*>(Get_Part<CWeapon>(ENUM_TO_UINT(Part::GUN)));
    if (!pMyGun)
        return E_FAIL;

    vector<_uint> vecChangeState_ByKey{};
    vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), ENUM_TO_UINT(State::END));

    TIME_COUNTER tKeyTimer = {};
    tKeyTimer.bCountTime = false;
    tKeyTimer.bTimeReset = false;

    // combo state : moon
    {
        CState_MoonCombo::MOONCOMBO_DESC tDesc = {};
        tDesc.vCombo_CheckTimes = Vec4{ 15.f / ANIMTIC_3 ,15.f / ANIMTIC_3,20.f / ANIMTIC_3 ,25.f / ANIMTIC_3 };
        tDesc.fSlide_CheckTime = 15.f / ANIMTIC_3;

        _int iSlide = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_SlideAttack");
        _int iCombo1 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_01");
        _int iCombo2 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_02");
        _int iCombo3 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_03");
        _int iCombo4 = Get_AnimationIndex(L"Animation_PlayerMoon_Sword_RunAttack_04");

        tDesc.arrCombo_EndTimes = { 50.f / ANIMTIC_3 ,33.f / ANIMTIC_3,39.f / ANIMTIC_3 ,60.f / ANIMTIC_3 ,70.f / ANIMTIC_3 };

        tDesc.iSlideAnimIdx = iSlide;
        tDesc.iFirstAnimIdx = iCombo1;
        tDesc.iSecondAnimIdx = iCombo2;
        tDesc.iThirdAnimIdx = iCombo3;
        tDesc.iFourthAnimIdx = iCombo4;
        tDesc.iEndStateIndex = ENUM_TO_UINT(State::END);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::COMBO), CState_MoonCombo::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // combo state : dual
    {
        CState_DualCombo::DUALCOMBO_DESC tDesc = {};
        tDesc.vCombo_CheckTimes = Vec4{ 17.f / (ANIMTPS * 1.9f) ,15.f / ANIMTIC_3, 10.f / ANIMTIC_3 ,24.f / ANIMTIC_3 };
        tDesc.fSlide_CheckTime = 17.f / (ANIMTPS * 1.5f);

        _int iSlide = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_SlideAttack");
        _int iCombo1 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_01");
        _int iCombo2 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_02");
        _int iCombo3 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_03");
        _int iCombo4 = Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_RunAttack_04");

        tDesc.arrCombo_EndTimes = { 55.f / (ANIMTPS * 1.9f) ,51.f / (ANIMTPS * 1.5f),50.f / ANIMTIC_3 ,60.f / ANIMTIC_3 ,75.f / ANIMTIC_3 };

        tDesc.iSlideAnimIdx = iSlide;
        tDesc.iFirstAnimIdx = iCombo1;
        tDesc.iSecondAnimIdx = iCombo2;
        tDesc.iThirdAnimIdx = iCombo3;
        tDesc.iFourthAnimIdx = iCombo4;
        tDesc.iEndStateIndex = ENUM_TO_UINT(State::END);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::COMBO_DUAL), CState_DualCombo::Create(pActionState, &tDesc))))
            return E_FAIL;
    }


    // JumpAttStart
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = {
                                {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Start")},
                                {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_Start")}
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_Loop") , Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_Loop") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = false;
        desc.bLoop = true;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        desc.FCollis = 0;

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::None;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTSTART), CState_JumpAttStart::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JumpAttEnd
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_FallAttack_End"), Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_FallAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF
            | CStateBase_Player::COLLISIONFLAGS::C_StunHit
            ;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis = 0;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)] = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)] = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 19.f / ANIMTIC_3;//0.55f ;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_End;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPATTEND), CState_JumpAttEnd::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Charge
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni | CStateBase::STATEANI_FLAG::SA_PreNonEvent;
        desc.vecPreAnims = {
            {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_Start")}
            ,  {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_Start")}

        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Sword_HeavyAttack_End"),Get_AnimationIndex(L"Animation_PlayerMoon_Dualblade_HeavyAttack_End") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis = 0;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)] = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)] = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 1.f;
        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_End;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CHARGE), CState_MoonCharge::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // skill1
    {
        CState_SkillBase::Skill_DESC tDesc = {};
        tDesc.bKeyInput = true;
        tDesc.fKeyCoolTime = 21.f / ANIMTIC;
        tDesc.iAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Light_Skill01");
        tDesc.iPlayerState = ENUM_TO_UINT(State::SKILL1);

        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SKILL1), CState_MoonSkill::Create(pActionState, "SkillE", &tDesc))))
            return E_FAIL;
    }

    // skill2
    {
        CState_SkillBase::Skill_DESC tDesc = {};
        tDesc.bKeyInput = true;
        tDesc.fKeyCoolTime = 60.f / ANIMTIC;
        tDesc.iAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Light_Skill02_Red");
        tDesc.iPlayerState = ENUM_TO_UINT(State::SKILL2);
        tDesc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SKILL2), CState_MoonSkill::Create(pActionState, "SkillQ", &tDesc))))
            return E_FAIL;
    }

    // gun idle
    {
        CStateBase_Player::PLAYER_STATE_SPECIFICDESC tDesc = {};
        tDesc.bBlend = true;
        tDesc.bLoop = true;
        tDesc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Holding_Loop") };
        tDesc.pOwnerGun = pMyGun;

        tDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF;



        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNIDLE), CState_GunIdle::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // gun walk
    {
        CStateBase_Player::PLAYER_STATE_SPECIFICDESC tDesc = {};
        tDesc.bBlend = true;
        tDesc.bLoop = true;
        tDesc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Holding_Run_Loop") };
        tDesc.pOwnerGun = pMyGun;

        tDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF;



        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNWALK), CState_GunWalk::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    array<_uint, ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::END)> arrMix;
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::F)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_F");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::B)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_B");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::L)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_l");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::R)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_R");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::LF)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_LF");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::LB)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_LB");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::RF)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_RF");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::RB)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Run_Loop_RB");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::JUMP)] = Get_AnimationIndex(L"Animation_PlayerMoon_FirstJump_InplaceStart");
    arrMix[ENUM_TO_SZET(CState_GunBase::Douwn_MixAnim::FALL)] = Get_AnimationIndex(L"Animation_PlayerMoon_Jump_FallLoop");

    array<_uint, ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::END)> arrAimMix;
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::DOWN)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MD");
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::MIDDLE)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MM");
    arrAimMix[ENUM_TO_SZET(CState_GunBase::Aim_MixAnim::UP)] = Get_AnimationIndex(L"Animation_PlayerMoon_Shotgun_Aim_MU");


    vector<CModel::DATA_ANIMIX> vecDownMix = { {304,true,1.f},{329,true,1.f},{378,true,1.f} };//, { 204,true,1.f } // 넣을 거면 mix도 blend 필요
    for (auto& MixAnim : arrMix)
    {
        pModel->Make_MixRatio(MixAnim, vecDownMix, pAnimMixCS);
    }

    // gun attack
    {
        CState_GunBase::GUN_STATEBASE_DESC tDesc = {};

        tDesc.arrMixAnims = arrMix;
        tDesc.arrAimAnims = arrAimMix;

        tDesc.bLoop = true;
        tDesc.pOwnerGun = pMyGun;
        tDesc.iMainAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Machinegun01_Shooting_Loop");



        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNATTACK), CState_GunAttack::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    // gun reload
    {
        CState_GunBase::GUN_STATEBASE_DESC tDesc = {};

        tDesc.arrMixAnims = arrMix;
        tDesc.arrAimAnims = arrAimMix;

        tDesc.bLoop = false;
        tDesc.pOwnerGun = pMyGun;
        tDesc.iMainAnimIdx = Get_AnimationIndex(L"Animation_PlayerMoon_Machinegun01_Reload");

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::GUNRELOAD), CState_GunReload::Create(pActionState, &tDesc))))
            return E_FAIL;
    }

    return S_OK;
}

CRemotePlayer* CRemotePlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRemotePlayer* pInstance = new CRemotePlayer(pDevice, pDeviceContext);
	
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created Prototype : CRemotePlayer");
	}

	return pInstance;
}

CGameObject* CRemotePlayer::Clone(void* pArg)
{
	CRemotePlayer* pInstance = new CRemotePlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned instance : CRemotePlayer");
	}

	return pInstance;
}

void CRemotePlayer::Free()
{
	Super::Free();
}
