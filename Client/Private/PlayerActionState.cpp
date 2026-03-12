#include "pch.h"
#include "PlayerActionState.h"
#include "ComboContainer.h"

#include "GameObject.h"

#include "DataStruct_AttackPreset.h"

#include "GameInstance.h"

CPlayerActionState::CPlayerActionState()
    : Super()
{
}

CPlayerActionState::CPlayerActionState(const CPlayerActionState& rhs)
    : Super(rhs)
    , m_tFKeyData(rhs.m_tFKeyData)
    , m_tGunCoolTimer(rhs.m_tGunCoolTimer)
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
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Fly;
            break;
        }
    }
        break;

    case DTO::EAttackPresetCategory::MonsterSkill:
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
        case DTO::EHitType::Light:
            m_fAttackFlag |= AF_Addtive;
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Fly;
            break;
        }
    }
    break;


    case DTO::EAttackPresetCategory::MonsterPorjectile:
        m_fAttackFlag |= AF_Addtive;
        break;

        // boss쪽
    case DTO::EAttackPresetCategory::BossBasic:
    {
        m_fAttackFlag |= AF_Strong;
        m_fAttackFlag |= AF_Fly;
        break;
    }
    case DTO::EAttackPresetCategory::BossSkill:
    case DTO::EAttackPresetCategory::BossProjectile:
    {
        switch (tHit.attackDesc.pAttackPreset->tCombat.eHitType)
        {
        case DTO::EHitType::Additive:
            m_fAttackFlag |= AF_Addtive;
            break;

        case DTO::EHitType::Heavy:
            m_fAttackFlag |= AF_Strong;
            m_fAttackFlag |= AF_Fly;
            break;
        }
    }
        break;

        // 만약 위에 조건에 걸리지 않았다면 일단 attck을 끄자
    default:
        m_fAttackFlag = 0;
    }
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
}
