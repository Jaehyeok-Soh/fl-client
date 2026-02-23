#include "pch.h"
#include "SkillComp_MoonE.h"

#include "Player.h"
#include "StatCom_Player.h"

CSkillComp_MoonE::CSkillComp_MoonE()
	:Super()
{
}

HRESULT CSkillComp_MoonE::Initialize(void* pArg)
{
	// 10 0
	SKILL_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 5.f;
	tMyDesc.FSkillFlags =  Attack_Add | Mental_Sub | Attack_Sub;

	SKILL_INFO tSkill = {};
	{
		tSkill.fStatAttack = 10.f;
		tSkill.fStatSheild = 0.f;
		tSkill.eSkillType = SKILL_TYPE::DAMAGE;
		tSkill.fNeedMental = 15.f;

		TIME_COUNTER tCoolTimer = {};
		{
			tCoolTimer.bTimeReset = false;
			tCoolTimer.fMaxTime = 0.f;
		}
		tSkill.tCoolTimer = tCoolTimer;
	}
	tMyDesc.tSkillInfo = tSkill;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

void CSkillComp_MoonE::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

_bool CSkillComp_MoonE::Start_Skill(CMyStat* pStatCom)
{
	if (Super::Start_Skill(pStatCom))
	{
		static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::E, true);

		// 충돌체 2개 발사

		return true;
	}


	return false;
}

void CSkillComp_MoonE::End_Skill(CMyStat* pStatCom)
{
	Super::End_Skill(pStatCom);

	static_cast<CStatCom_Player*>(pStatCom)->Set_AttackState(CStatCom_Player::Attack_State::E, false);

	// 충돌체 회수
}

_bool CSkillComp_MoonE::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return false;
}

void CSkillComp_MoonE::Update_Skill(const _float fTimeDelta)
{
	Super::Update_Skill(fTimeDelta);
}

CSkillComp_MoonE* CSkillComp_MoonE::Create(void* pArg)
{
	CSkillComp_MoonE* pInstance = new CSkillComp_MoonE();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillComp_MoonE::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillComp_MoonE::Free()
{
	Super::Free();
}