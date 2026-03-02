#include "pch.h"
#include "PlayerActionState.h"
#include "ComboContainer.h"

#include "Engine_Utils.h"
#include "DataStruct_AttackPreset.h"

CPlayerActionState::CPlayerActionState()
    : Super()
{
}

CPlayerActionState::CPlayerActionState(const CPlayerActionState& rhs)
    : Super(rhs)
{
}

HRESULT CPlayerActionState::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerActionState::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
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
    case DTO::EAttackPresetCategory::BossBasic:
        m_fAttackFlag |= AF_Addtive;
        break;

    case DTO::EAttackPresetCategory::BossSkill:
        m_fAttackFlag |= AF_Strong;
        m_fAttackFlag |= AF_Fly;
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
