#include "Engine_pch.h"
#include "Skill.h"

CSkill::CSkill()
	:Super()
{
}

CSkill::CSkill(const CSkill& rhs)
	: Super(rhs)
	, m_bOnSkill(rhs.m_bOnSkill)
	, m_bEndSkill(rhs.m_bEndSkill)
	, m_TSkillTimer(rhs.m_TSkillTimer)
{
}

HRESULT CSkill::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	SKILLCOMP_DESC* pDesc = static_cast<SKILLCOMP_DESC*>(pArg);

	m_tSkillDesc = pDesc->tSkillDesc;
	m_bCountTime = pDesc->bCountTime;

	if (m_bCountTime)
		m_TSkillTimer.y = pDesc->fSkillTime;

	return S_OK;
}

void CSkill::Update_Component(const _float fTimeDelta)
{
	// skill이 켜졌을때 update를 돌겠다	
	if (m_bOnSkill)
	{
		if (m_bEndSkill)
		{
			return;
		}

		// skill update
		Update_Skill(fTimeDelta);

		// 만약 timer를 기준으로 돌고 싶다면
		Count_SkillTime(fTimeDelta);
	}
}

void CSkill::Start_Skill(CMyStat* pStatCom)
{
	m_bOnSkill = true;
	m_bEndSkill = false;
}

void CSkill::Update_Skill(const _float fTimeDelta)
{
}

void CSkill::End_Skill(CMyStat* pStatCom)
{
	m_bOnSkill = false;
	m_bEndSkill = true;
}

void CSkill::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
}

void CSkill::Count_SkillTime(const _float fTimeDelta)
{
	// 타이머를 카운트 할거고, skill이 끝나지 않았고
	if (m_bCountTime && !m_bEndSkill)
	{
		if (m_TSkillTimer.x >= m_TSkillTimer.y)
		{
			m_TSkillTimer.x = 0;
			m_bEndSkill = true;
			return;
		}

		m_TSkillTimer.x += fTimeDelta;
	}
}

CSkill* CSkill::Create()
{
	CSkill* pInstance = new CSkill();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CSkill::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CSkill::Clone(void* pArg)
{
	CSkill* pInstance = new CSkill();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkill::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill::Free()
{
	Super::Free();
}