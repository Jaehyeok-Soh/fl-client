#include "pch.h"
#include "SkillComp_MoonQ.h"

#include "Player.h"
#include "StatCom_Player.h"

CSkillComp_MoonQ::CSkillComp_MoonQ()
	:Super()
{
}

CSkillComp_MoonQ::CSkillComp_MoonQ(const CSkillComp_MoonQ& rhs)
	: Super(rhs)
	, m_fAddAttackRate(rhs.m_fAddAttackRate)
{
}

HRESULT CSkillComp_MoonQ::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillComp_MoonQ::Initialize(void* pArg)
{
	SKILLCOMP_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 12.f;
	tMyDesc.iPlayerState = ENUM_TO_UINT(CPlayer::State::SKILL2);

	SKILL_DESC tSkill = {};
	ATTACK_ELEMNETS tAttackDesc = {};
	tAttackDesc.iAttack = 0;
	tAttackDesc.iSheild = 10;

	tSkill.tAttDesc = tAttackDesc;
	tSkill.eSkillType = SKILL_TYPE::BUFF;
	tSkill.iNeedMental = 35;
	tSkill.TCoolTime = { 0.f,3.5f };

	tMyDesc.tSkillDesc = tSkill;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

void CSkillComp_MoonQ::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CSkillComp_MoonQ::Start_Skill(CStatComponent* pStatCom)
{
	Super::Start_Skill(pStatCom);

	static_cast<CStatCom_Player*>(pStatCom)->Set_Attack_AddRate(m_fAddAttackRate);

	static_cast<CStatCom_Player*>(pStatCom)->Add_State(CStatCom_Player::STAT_TYPE::MENTAL, -1.f * m_tSkillDesc.iNeedMental);

	// 충돌체 발사
}

void CSkillComp_MoonQ::Update_Skill(const _float fTimeDelta)
{

}

void CSkillComp_MoonQ::End_Skill(CStatComponent* pStatCom)
{
	Super::End_Skill(pStatCom);

	// 충돌체 회수
}

void CSkillComp_MoonQ::On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj)
{
}

CSkillComp_MoonQ* CSkillComp_MoonQ::Create()
{
	CSkillComp_MoonQ* pInstance = new CSkillComp_MoonQ();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkillComp_MoonQ::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSkillComp_MoonQ::Clone(void* pArg)
{
	CSkillComp_MoonQ* pInstance = new CSkillComp_MoonQ();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillComp_MoonQ::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillComp_MoonQ::Free()
{
	Super::Free();
}