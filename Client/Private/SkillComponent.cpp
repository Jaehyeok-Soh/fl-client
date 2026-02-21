#include "pch.h"
#include "SkillComponent.h"

// has obj
#include "Player.h"

CSkillComponent::CSkillComponent()
	:Super()
{
}

CSkillComponent::CSkillComponent(const CSkillComponent& rhs)
	: Super(rhs)
	, m_bOnSkill(rhs.m_bOnSkill)
	, m_bEndSkill(rhs.m_bEndSkill)
	, m_TSkillTimer(rhs.m_TSkillTimer)
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

	SKILLCOMP_DESC* pDesc = static_cast<SKILLCOMP_DESC*>(pArg);
	
	m_tSkillDesc = pDesc->tSkillDesc;
	m_bCountTime = pDesc->bCountTime;

	if (m_bCountTime)
		m_TSkillTimer.y = pDesc->fSkillTime;

	m_iPlayerState = pDesc->iPlayerState;

	return S_OK;
}

void CSkillComponent::Update(const _float fTimeDelta)
{
	// skill이 켜졌을때 update를 돌겠다	
	if (m_bOnSkill)
	{
		if (m_bEndSkill)
		{
			static_cast<CPlayer*>(Get_Owner())->End_Attack(static_cast<CPlayer::State>(m_iPlayerState));
			return;
		}

		// skill update
		Update_Skill(fTimeDelta);

		// 만약 timer를 기준으로 돌고 싶다면
		Count_SkillTime(fTimeDelta);
	}

}

void CSkillComponent::Start_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = true;
	m_bEndSkill = false;
}

void CSkillComponent::Update_Skill(const _float fTimeDelta)
{
}

void CSkillComponent::End_Skill(CStatComponent* pStatCom)
{
	m_bOnSkill = false;
	m_bEndSkill = true;
}

void CSkillComponent::On_Collision_Monster(const _float fTimeDelta, CGameObject* pObj)
{
}

void CSkillComponent::Count_SkillTime(const _float fTimeDelta)
{
	// 타이머를 카운트 할거고, skill이 끝나지 않았고
	if (m_bCountTime && !m_bEndSkill)
	{
		if (m_TSkillTimer.x >= m_TSkillTimer.y)
		{
			m_TSkillTimer.x = 0;
			m_bEndSkill		= true;
			return;
		}

		m_TSkillTimer.x += fTimeDelta;
	}
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