#include "pch.h"
#include "SkillComp_MoonE.h"

#include "Player.h"
#include "StatCom_Player.h"

CSkillComp_MoonE::CSkillComp_MoonE()
	:Super()
{
}

CSkillComp_MoonE::CSkillComp_MoonE(const CSkillComp_MoonE& rhs)
	: Super(rhs)
{
}

HRESULT CSkillComp_MoonE::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillComp_MoonE::Initialize(void* pArg)
{
	SKILLCOMP_DESC tMyDesc = {};

	tMyDesc.bCountTime = true;
	tMyDesc.fSkillTime = 5.f;
	tMyDesc.iPlayerState = ENUM_TO_UINT(CPlayer::State::SKILL1);

	SKILL_DESC tSkill = {};
	ATTACK_ELEMNETS tAttackDesc = {};
	tAttackDesc.iAttack = 10;
	tAttackDesc.iSheild = 0;

	tSkill.tAttDesc = tAttackDesc;
	tSkill.eSkillType = SKILL_TYPE::DAMAGE;
	tSkill.iNeedMental = 15;
	tSkill.TCoolTime = { 0.f,0.f };

	tMyDesc.tSkillDesc = tSkill;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

void CSkillComp_MoonE::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CSkillComp_MoonE::Start_Skill(CStatComponent* pStatCom)
{
	Super::Start_Skill(pStatCom);

	static_cast<CStatCom_Player*>(pStatCom)->Add_State(CStatCom_Player::STAT_TYPE::MENTAL, -1.f * m_tSkillDesc.iNeedMental);

	// 충돌체 2개 발사
}

void CSkillComp_MoonE::Update_Skill(const _float fTimeDelta)
{
	Super::Update_Skill(fTimeDelta);
}

void CSkillComp_MoonE::End_Skill(CStatComponent* pStatCom)
{
	Super::End_Skill(pStatCom);

	// 충돌체 회수
}

void CSkillComp_MoonE::On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj)
{
}

CSkillComp_MoonE* CSkillComp_MoonE::Create()
{
	CSkillComp_MoonE* pInstance = new CSkillComp_MoonE();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkillComp_MoonE::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSkillComp_MoonE::Clone(void* pArg)
{
	CSkillComp_MoonE* pInstance = new CSkillComp_MoonE();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillComp_MoonE::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillComp_MoonE::Free()
{
	Super::Free();
}