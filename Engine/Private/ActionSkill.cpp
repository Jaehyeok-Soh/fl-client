#include "Engine_pch.h"
#include "ActionSkill.h"

CActionSkill::CActionSkill()
{
}

CActionSkill::CActionSkill(const CActionSkill& rhs)
{
}

HRESULT CActionSkill::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActionSkill::Initialize(void* pArg)
{
	if (!pArg)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	ACTIONSKILL_DESC* pDesc = static_cast<ACTIONSKILL_DESC*>(pArg);

	m_iSkillCount	= pDesc->iSkillCount;
	m_pOwnerStat	= pDesc->pOwnerStat;

	m_vecSkills.resize(m_iSkillCount);

	return S_OK;
}

HRESULT CActionSkill::Add_Skill(_uint iIndex, CSkillBase* pSkill)
{
	if (!Check_Index(iIndex))
		return E_FAIL;

	if (!pSkill)
		return E_FAIL;

	m_vecSkills[iIndex] = pSkill;
	return S_OK;
}

CSkillBase* CActionSkill::Get_Skill(_uint iIndex)
{
	if (!Check_Index(iIndex))
		return nullptr;

	return m_vecSkills[iIndex];
}

void CActionSkill::Update_Skills(const _float fTimeDelta)
{
	for (auto& pSkill : m_vecSkills)
	{
		if(pSkill)
			pSkill->Update(fTimeDelta);
	}
}

_bool CActionSkill::Start_Skill(_uint iIndex)
{
	if (!Check_Index(iIndex))
		return false;

	return m_vecSkills[iIndex]->Start_Skill(m_pOwnerStat);
}

void CActionSkill::End_Skill(_uint iIndex)
{
	if (!Check_Index(iIndex))
		return;

	m_vecSkills[iIndex]->End_Skill(m_pOwnerStat);
}

_bool CActionSkill::Is_EndSkill(_uint iIndex)
{
	if (!Check_Index(iIndex))
		return false;

	return m_vecSkills[iIndex]->Is_EndSkill();
}

_bool CActionSkill::Is_OnSkill(_uint iIndex)
{
	if (!Check_Index(iIndex))
		return false;

	return m_vecSkills[iIndex]->Is_OnSkill();
}

void CActionSkill::Set_CountTime(_uint iIndex, _bool bCount)
{
	if (!Check_Index(iIndex))
		return;

	return m_vecSkills[iIndex]->Set_CountTime(bCount);
}

const CSkillBase::SKILL_INFO& CActionSkill::Get_SkillDesc(_uint iIndex)
{
	return m_vecSkills[iIndex]->Get_SkillDesc();
}

_bool CActionSkill::Check_Index(_uint iIndex)
{
	return (iIndex < m_iSkillCount);
}

CActionSkill* CActionSkill::Create()
{
	CActionSkill* pInstance = new CActionSkill();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CActionSkill::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CActionSkill::Clone(void* pArg)
{
	CActionSkill* pClone = new CActionSkill(*this);

	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CActionSkill::Clone, Failed");
		Safe_Release(pClone);
	}

	return pClone;
}

void CActionSkill::Free()
{
	for (auto& pSkill : m_vecSkills)
	{
		Safe_Release(pSkill);
	}

	__super::Free();
}
