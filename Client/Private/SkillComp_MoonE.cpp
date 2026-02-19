#include "pch.h"
#include "SkillComp_MoonE.h"

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
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSkillComp_MoonE::Update(const _float fTimeDelta)
{
	if (m_bOnSkill)
		Update_Skill(fTimeDelta);
}

void CSkillComp_MoonE::Start_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = true;
}

void CSkillComp_MoonE::Update_Skill(const _float fTimeDelta)
{
}

void CSkillComp_MoonE::End_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = false;
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

void CSkillComponent::Free()
{
	Super::Free();
}