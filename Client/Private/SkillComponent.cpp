#include "pch.h"
#include "SkillComponent.h"

CSkillComponent::CSkillComponent()
	:Super()
{
}

CSkillComponent::CSkillComponent(const CSkillComponent& rhs)
	: Super(rhs)
	, m_bOnSkill(rhs.m_bOnSkill)
{
}

HRESULT CSkillComponent::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillComponent::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSkillComponent::Update(const _float fTimeDelta)
{
	if (m_bOnSkill)
		Update_Skill(fTimeDelta);
}

void CSkillComponent::Start_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = true;
}

void CSkillComponent::Update_Skill(const _float fTimeDelta)
{
}

void CSkillComponent::End_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = false;
}

void CSkillComponent::On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj)
{
}

CSkillComponent* CSkillComponent::Create()
{
	CSkillComponent* pInstance = new CSkillComponent();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkillComponent::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSkillComponent::Clone(void* pArg)
{
	CSkillComponent* pInstance = new CSkillComponent();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillComponent::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillComponent::Free()
{
	Super::Free();
}