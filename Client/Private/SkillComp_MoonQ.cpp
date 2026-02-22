#include "pch.h"
#include "SkillComp_MoonQ.h"

#include "Player.h"
#include "StatCom_Player.h"

CSkillComp_MoonQ::CSkillComp_MoonQ()
	:Super()
{
}

HRESULT CSkillComp_MoonQ::Initialize(void* pArg)
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

void CSkillComp_MoonQ::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

_bool CSkillComp_MoonQ::Start_Skill(CMyStat* pStatCom)
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

void CSkillComp_MoonQ::End_Skill(CMyStat* pStatCom)
{
	Super::End_Skill(pStatCom);
	static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::Q, false);

	// 충돌체 회수
}

_bool CSkillComp_MoonQ::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return false;
}

void CSkillComp_MoonQ::Update_Skill(const _float fTimeDelta)
{

}

CSkillComp_MoonQ* CSkillComp_MoonQ::Create(void* pArg)
{
	CSkillComp_MoonQ* pInstance = new CSkillComp_MoonQ();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillComp_MoonQ::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillComp_MoonQ::Free()
{
	Super::Free();
}