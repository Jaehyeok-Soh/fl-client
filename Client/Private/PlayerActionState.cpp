#include "pch.h"
#include "PlayerActionState.h"
#include "ComboContainer.h"

#include "GameObject.h"
#include "Model.h"
#include "DataStruct_AttackPreset.h"
#include "PlayerControlContext.h"
#include "StateBase_Player.h"
#include "Player.h"

#include "GameInstance.h"

#include "PlayerImguiValues.h"

CPlayerActionState::CPlayerActionState()
    : Super()
{
}

CPlayerActionState::CPlayerActionState(const CPlayerActionState& rhs)
    : Super(rhs)
    , m_tFKeyData(rhs.m_tFKeyData)
    , m_tGunCoolTimer(rhs.m_tGunCoolTimer)
    , m_bCanSpecialDash(rhs.m_bCanSpecialDash)
    , m_eBoneState(rhs.m_eBoneState)
    , m_tBoneHit(rhs.m_tBoneHit)
    , m_arrHitSoundHashes(rhs.m_arrHitSoundHashes)
{
}

HRESULT CPlayerActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

    m_tGunCoolTimer.bCountTime = false;
    m_tGunCoolTimer.bTimeReset = false;

    m_tGunCoolTimer.fMaxTime = 0.15f;
    m_tGunCoolTimer.fTimeAcc = 0.f;

    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::Wind)]          = { TO_HASH("sfx_common_player_windHit_r"),0.8f };
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::Projectile)]    = { TO_HASH("sfx_common_player_fireHit_normal_r"),1.f };
                                                                  
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::WhipNormal)]    = { TO_HASH("sfx_common_player_swordWhipHit_normal_r"),0.5f };
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::WhipHeavy)]     = { TO_HASH("sfx_common_player_swordWhipHit_heavy_r"),0.5f };
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::ElectricSamll)] = { TO_HASH("sfx_common_player_electrict_small_hit_r"),1.f };
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::ElectricBall)]  = { TO_HASH("sfx_boss_Xibi_electricBall_hit_r"),1.f };
                                                                  
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::HeavySword)]    = { TO_HASH("sfx_common_player_heavySwordHit_heavy_r"),0.8f };
    m_arrHitSoundHashes[ENUM_TO_SZET(HIT_SOUND::Fire)]          =  { TO_HASH("sfx_player_Nvzhu_fire_hit_r"),0.8f };

	return S_OK;
}

HRESULT CPlayerActionState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerActionState::Update(const _float fTImeDelta)
{
    m_tGunCoolTimer.CountTime(fTImeDelta);
    
    Super::Update(fTImeDelta);

    Update_BoneState(fTImeDelta);
}

void CPlayerActionState::My_Awake(const _uint iCurrentLevelID)
{
    Change_ActionBoneState(CPlayerActionState::BONE_STATE::NORMAL);

    m_DDialoghandle = m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
        {
            Check_DialogueBegin(iId);
        });
}

_bool CPlayerActionState::Get_KeyFlag(_uint iKeyFlag)
{
    return static_cast<CPlayerControlContext*>(m_pOwnerControlContext)->Get_KeyFlag(static_cast<CPlayerControlContext::KEYFLAGS>(iKeyFlag));
}

void CPlayerActionState::Set_Flag(Flags FActionFlag, _bool bOn)
{
    if (bOn)
    {
        Engine_Utils::Add_Flag(m_fAttackFlag, FActionFlag);
    }

    else
    {
        Engine_Utils::RemoveHard_Flag(m_fAttackFlag, FActionFlag);
    }
}

void CPlayerActionState::Set_HitDesc(const HIT_DESC& tHit)
{
    // 여기서 어떤 공격이 들어 왔는지 flag를 켜준다
    m_fAttackFlag |= AF_OnHit;  
    m_tPreHitDesc = tHit;

    // todo_eunbi : 몬스터 종류 늘어나고, 공격이 다양해지면 id 까지 검사
    DTO::EAttackPresetCategory eCategory =  tHit.attackDesc.pAttackPreset->eCategory;
    switch (eCategory)
    {
    case DTO::EAttackPresetCategory::MonsterBasic:
        /* EHitType으로 분기 처리*/
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
            case DTO::EHitType::Light:
                m_fAttackFlag |= AF_Addtive;
                m_eHitSound = HIT_SOUND::Wind;
                break;

            case DTO::EHitType::Heavy:
                m_fAttackFlag |= AF_Fly;
                m_eHitSound = HIT_SOUND::Wind;
                break;
        }

        m_fAttackFlag |= AF_Stun;
    }
        break;

    case DTO::EAttackPresetCategory::MonsterSkill:
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
        case DTO::EHitType::Light:
            m_fAttackFlag |= AF_Addtive;
            m_eHitSound = HIT_SOUND::Wind;
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Fly;
            m_eHitSound = HIT_SOUND::Wind;
            break;
        }

        m_fAttackFlag |= AF_Special;
    }
    break;


    case DTO::EAttackPresetCategory::MonsterPorjectile:
        m_fAttackFlag |= AF_Addtive;
        m_eHitSound = HIT_SOUND::Projectile;
        break;

        // boss쪽
    case DTO::EAttackPresetCategory::BossBasic:
    {
        /* xibi */
        m_fAttackFlag |= AF_Strong;
        m_fAttackFlag |= AF_Fly;

        m_eHitSound = HIT_SOUND::WhipHeavy;
        break;
    }
    case DTO::EAttackPresetCategory::BossSkill:
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
        case DTO::EHitType::Additive:
            m_fAttackFlag |= AF_Addtive;

            m_eHitSound = HIT_SOUND::ElectricSamll;
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Strong;
            m_fAttackFlag |= AF_Fly;

            m_eHitSound = HIT_SOUND::ElectricBall;
            break;
        }
    }
    break;

    case DTO::EAttackPresetCategory::BossProjectile:
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
        case DTO::EHitType::Additive:
            m_fAttackFlag |= AF_Addtive;

            m_eHitSound = HIT_SOUND::ElectricSamll;
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Strong;
            m_fAttackFlag |= AF_Fly;

            m_eHitSound = HIT_SOUND::ElectricSamll;
            break;
        }
    }
        break;

        // 만약 위에 조건에 걸리지 않았다면 일단 attck을 끄자
    default:
        m_fAttackFlag = 0;
    }

    Play_HitSound();
}

_bool CPlayerActionState::Is_OnHit()
{
    return Engine_Utils::Has_Flag(m_fAttackFlag, AttackFlag::AF_OnHit);
}

Vec3 CPlayerActionState::Get_VicPosition() const
{
    CGameObject* pVic = m_tPreHitDesc.pVictim;

    if (pVic)
    {
        CTransform* pTrans = pVic->Get_Component<CTransform>();

        if (pTrans)
        {
            return pTrans->Get_Info(TRANSFORM_INFO_STATE::POS);
        }
    }

    return Vec3::Zero;
}

_bool CPlayerActionState::Is_AttackLanded()
{
    return static_cast<CPlayerControlContext*>(m_pOwnerControlContext)->Is_AttackLanded();
}

_bool CPlayerActionState::Can_FKeyEvent()
{
    if (m_tFKeyData.bEventCheckOn &&
        KEY_BUTTON_DOWN(DIK_F))
        return true;

    return false;
}

_bool CPlayerActionState::Can_ChangeGunState()
{
    /*
    count time을 하지 않고 있거나 rate가 1.f가 되었을때
    */
    if (m_tGunCoolTimer.bCountTime == false ||
        m_tGunCoolTimer.Get_Rate() == 1.f)
        return true;

    return false;
}

void CPlayerActionState::Set_FKeyEvent(_uint iEvnet, _bool bOn)
{
    m_tFKeyData.bEventCheckOn = bOn;
    m_tFKeyData.iKeyEvent = iEvnet;
}

void CPlayerActionState::Change_ActionBoneState(BONE_STATE eState, BONESTATE_CHANGE_ARGS* pArgs)
{
    if (m_eBoneState == eState)
        return;

    // end를 하고 다음 state로 change 할 수 있다면
    if (End_BoneState(eState))
    {
        // 바꿔라
        BONE_STATE ePreState = m_eBoneState;
        m_eBoneState = eState;

        Start_BoneState(ePreState, pArgs);
    }
}

void CPlayerActionState::Start_BoneState(BONE_STATE ePreState, BONESTATE_CHANGE_ARGS* pArgs)
{
    switch (m_eBoneState)
    {
    case BONE_STATE::NORMAL:
        m_pOwnerModel->Set_MoveBone(false);
        break;

    case BONE_STATE::HITSTART:
    {
        if (pArgs)
        {
            _uint iHitFlag = pArgs->iHitType | Get_CurState_BoneHitFlag();

            switch (iHitFlag)
            {
            case BoneHitType::BHT_Front | BoneHitType::BHT_FORCE_WEAK:
                m_pOwnerModel->SEt_MoveBone_Matrix(m_tBoneHit.matFrontHit_Weak); break;

            case BoneHitType::BHT_Front | BoneHitType::BHT_FORCE_STRONG:
                m_pOwnerModel->SEt_MoveBone_Matrix(m_tBoneHit.matFrontHit_Strong); break;

            case BoneHitType::BHT_BACK | BoneHitType::BHT_FORCE_WEAK:
                m_pOwnerModel->SEt_MoveBone_Matrix(m_tBoneHit.matBackHit_Weak); break;

            case BoneHitType::BHT_BACK | BoneHitType::BHT_FORCE_STRONG:
                m_pOwnerModel->SEt_MoveBone_Matrix(m_tBoneHit.matBackHit_Strong); break;
            }
        }
    }

    case BONE_STATE::HITEND:
        m_pOwnerModel->Set_MoveBone(true);
        m_tBoneHit.fTimeAcc = 0.f;
        break;
    }
}

_bool CPlayerActionState::End_BoneState(BONE_STATE eNextState)
{
    _bool bCanChange = false;

    // true 조건 만 on 해준다
    switch (m_eBoneState)
    {
    case BONE_STATE::NORMAL:
        if (eNextState == BONE_STATE::HITSTART &&
            Has_Capability(Get_CurrentCapabilities(), Engine::StateCapability::MOVE))
        {
            bCanChange = true;
        }

        break;

    case BONE_STATE::HITSTART:
        if (m_tBoneHit.fTimeAcc == m_tBoneHit.fTimeAcc)
        {
            bCanChange = true;
        }
        break;

    case BONE_STATE::HITEND:
        if (eNextState == BONE_STATE::NORMAL &&
            m_tBoneHit.fTimeAcc == m_tBoneHit.fTimeAcc)
        {
            bCanChange = true;
        }
        break;

    case BONE_STATE::END:
        bCanChange = true;
    }

    return bCanChange;
}

void CPlayerActionState::Update_BoneState(const _float fTimeDelta)
{
    switch (m_eBoneState)
    {
    case BONE_STATE::NORMAL:
        Update_Normal(fTimeDelta);
        break;

    case BONE_STATE::HITSTART:
        Update_HitStart(fTimeDelta);
        break;

    case BONE_STATE::HITEND:
        Update_HitEnd(fTimeDelta);
        break;
    }
}

void CPlayerActionState::Update_Normal(const _float fTimeDelta)
{
}

void CPlayerActionState::Update_HitStart(const _float fTimeDelta)
{
    _bool bEnd = false;

    // 시간 누적
    m_tBoneHit.fTimeAcc += fTimeDelta;
    if (m_tBoneHit.fTimeAcc >= m_tBoneHit.fLerpHalfTime)
    {
        m_tBoneHit.fTimeAcc = m_tBoneHit.fLerpHalfTime;
        bEnd = true;
    }

    // ratio 값 model에게 전달
    m_pOwnerModel->Set_MoveBone_Ratio(m_tBoneHit.fTimeAcc / m_tBoneHit.fLerpHalfTime);

    // lerp half time 지나면 -> hit end
    if (bEnd)
    {
        Change_ActionBoneState(BONE_STATE::HITEND);
    }
}

void CPlayerActionState::Update_HitEnd(const _float fTimeDelta)
{
    _bool bEnd = false;

    // 시간 누적
    m_tBoneHit.fTimeAcc += fTimeDelta;
    if (m_tBoneHit.fTimeAcc >= m_tBoneHit.fLerpHalfTime)
    {
        m_tBoneHit.fTimeAcc = m_tBoneHit.fLerpHalfTime;
        bEnd = true;
    }

    // ratio 값 model에게 전달
    m_pOwnerModel->Set_MoveBone_Ratio((m_tBoneHit.fLerpHalfTime - m_tBoneHit.fTimeAcc) / m_tBoneHit.fLerpHalfTime);

    // lerp half time 지나면 -> hit normal
    if (bEnd)
    {
        Change_ActionBoneState(BONE_STATE::NORMAL);
    }
}

_uint CPlayerActionState::Get_CurState_BoneHitFlag() const
{
    CStateBase_Player* pState = static_cast<CStateBase_Player*>(m_vecStates[m_iCurrentState]);
    return pState->Get_BoneHitFlag();
}

void CPlayerActionState::Check_DialogueBegin(_int iId)
{
    Change_State(ENUM_TO_UINT(CPlayer::State::NPCTALK));
}

void CPlayerActionState::Play_HitSound()
{
    // 방어 코드
    if (m_fAttackFlag == 0 || m_eHitSound == HIT_SOUND::END)
        return;

    // 캐싱해둔 hash 값으로 재생
    m_pGameInstance->Play_OneShot(0, m_arrHitSoundHashes[ENUM_TO_SZET(m_eHitSound)].iSoundHash, m_arrHitSoundHashes[ENUM_TO_SZET(m_eHitSound)].fVolum);

    // sound 값 리셋
    m_eHitSound = HIT_SOUND::END;
}

CPlayerActionState* CPlayerActionState::Create()
{
    CPlayerActionState* pInsatnce = new CPlayerActionState();
    if (FAILED(pInsatnce->Initialize_Prototype()))
    {
        MSG_BOX("CPlayerActionState::Create, Failed");
        Safe_Release(pInsatnce);
    }
    return pInsatnce;
}

CComponent* CPlayerActionState::Clone(void* pArg)
{
    CPlayerActionState* pClone = new CPlayerActionState(*this);
    if (FAILED(pClone->Initialize(pArg)))
    {
        MSG_BOX("CPlayerActionState::Clone, Failed");
        Safe_Release(pClone);
    }
    return pClone;
}

void CPlayerActionState::Free()
{
    Super::Free();
     
    m_pGameInstance->Unsubscribe<DIALOGUE_BEGIN>(m_DDialoghandle);
}
