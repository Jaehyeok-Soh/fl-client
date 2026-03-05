#include "pch.h"
#include "SkillBase_MoonQ.h"

#include "Player.h"
#include "StatCom_Player.h"

CSkillBase_MoonQ::CSkillBase_MoonQ()
	:Super()
{
}

HRESULT CSkillBase_MoonQ::Initialize(void* pArg)
{
	SKILL_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 12.f;
	tMyDesc.FSkillFlags = Sheild_Add | SkillAtt_Add | Mental_Sub  | Sheild_Sub | SkillAtt_Sub;

	SKILL_INFO tSkill = {};
	{
		tSkill.fSkillAttack = 10.f;

		tSkill.fStatAttack = 0.f;
		tSkill.fStatSheild = 10.f;
		tSkill.eSkillType = SKILL_TYPE::BUFF;
		tSkill.fNeedMental = 35.f;

		TIME_COUNTER tCoolTimer = {};
		{
			tCoolTimer.bTimeReset = false;
			tCoolTimer.fMaxTime = 3.5f;
		}
		tSkill.tCoolTimer = tCoolTimer;
	}
	tMyDesc.tSkillInfo = tSkill;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

void CSkillBase_MoonQ::Update(const _float fTimeDelta, CMyStat* pStatCom )
{
	Super::Update(fTimeDelta, pStatCom);
}

_bool CSkillBase_MoonQ::Start_Skill(CMyStat* pStatCom)
{
	if (Super::Start_Skill(pStatCom))
	{
		static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::Q, true);
		static_cast<CStatCom_Player*>(pStatCom)->Set_Attack_AddRate(m_fAddAttackRate);
		// 충돌체 발사

		return true;
	}

	return false;
}

void CSkillBase_MoonQ::End_Skill(CMyStat* pStatCom)
{
	Super::End_Skill(pStatCom);
	static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::Q, false);

	// 충돌체 회수
}

_bool CSkillBase_MoonQ::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return false;
}

void CSkillBase_MoonQ::Set_ExtraAttack_Desc(EXTRA_ATTACK_DESC& tStat_ExtraDesc, CMyStat* pOwnerStat)
{
	tStat_ExtraDesc.fAddRate = 0.15f;
}

void CSkillBase_MoonQ::Update_Skill(const _float fTimeDelta)
{

}

CSkillBase_MoonQ* CSkillBase_MoonQ::Create(void* pArg)
{
	CSkillBase_MoonQ* pInstance = new CSkillBase_MoonQ();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillBase_MoonQ::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBase_MoonQ::Free()
{
	Super::Free();
}