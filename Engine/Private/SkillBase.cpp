#include "Engine_pch.h"
#include "SkillBase.h"

#include "Engine_Utils.h"
#include "MyStat.h"

CSkillBase::CSkillBase()
	:Super()
{
}

HRESULT CSkillBase::Initialize(void* pArg)
{
	SKILL_DESC* pDesc = static_cast<SKILL_DESC*>(pArg);

	m_tSkillInfo = pDesc->tSkillInfo;
	m_bCountTime = pDesc->bCountTime;

	// 만약 스킬 지속 시간을 카운팅 할거라면 -> skill timer 값 셋팅
	if (m_bCountTime)
		m_TSkillTimer.y = pDesc->fSkillTime;

	m_FSkillFlags = pDesc->FSkillFlags;

	// 처음에는 max로 맞춤 -> 바로 스킬을 사용할 수 있도록 하기 위해
	m_tSkillInfo.tCoolTimer.fTimeAcc = m_tSkillInfo.tCoolTimer.fMaxTime;
	// reset을 하지 않음. 그래야 cool time 체크가 됨
	m_tSkillInfo.tCoolTimer.bTimeReset = false;
	// 처음에는 당연히 count를 하지 않음.
	m_tSkillInfo.tCoolTimer.bCountTime = false;

	return S_OK;
}

void CSkillBase::Update(const _float fTimeDelta)
{
	// skill이 on이 되었고 end가 나지 않았을때
	if (m_bOnSkill && !m_bEndSkill)
	{
		// skill update
		Update_Skill(fTimeDelta);

		// 만약 skill end를 지속 타임을 기준으로 하고 싶다면
		Count_SkillTime(fTimeDelta);
	}

	Count_NextCoolTime(fTimeDelta);
}

_bool CSkillBase::Can_StartSkill(CMyStat* pStatCom)
{
	// skill cool time이 다 되고
	// stat에 mental이 충분할때
	if (m_tSkillInfo.tCoolTimer.Get_Rate() == 1.f &&
		pStatCom &&
		pStatCom->Get_Stat_Vec2(CMyStat::STAT_TYPE::MENTAL).x >= m_tSkillInfo.fNeedMental)
		return true;

	return false;
}

_bool CSkillBase::Start_Skill(CMyStat* pStatCom)
{
	if (Can_StartSkill(pStatCom))
	{
		m_bOnSkill = true;
		m_bEndSkill = false;

		m_tSkillInfo.tCoolTimer.bCountTime = true;
		m_tSkillInfo.tCoolTimer.fTimeAcc = 0.f;

		m_TSkillTimer.x = 0.f;

		m_iOnSkillCount++;
		
		Check_StartFlag(pStatCom);

		return true;
	}

	return false;

}

void CSkillBase::End_Skill(CMyStat* pStatCom)
{
	m_bOnSkill = false;
	m_bEndSkill = true;

	Check_EndFlag(pStatCom);

	m_iOnSkillCount--;
}

_bool CSkillBase::On_Collision(const _float fTimeDelta, CGameObject* pObj)
{
	return true;
}

void CSkillBase::Update_Skill(const _float fTimeDelta)
{
}

void CSkillBase::Count_SkillTime(const _float fTimeDelta)
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

void CSkillBase::Count_NextCoolTime(const _float fTimeDelta)
{
	// skill cooltime을 카운트 할때
	// 만약 1.f가 되었다면
	// false로 바꿔라
	if (m_tSkillInfo.tCoolTimer.bCountTime &&
		m_tSkillInfo.tCoolTimer.CountTime(fTimeDelta) == 1.f)
	{
		m_tSkillInfo.tCoolTimer.bCountTime = false;
	}
}

void CSkillBase::Check_StartFlag(CMyStat* pStatCom)
{
	if (pStatCom == nullptr)
		return;

	// add
	if (Engine_Utils::Has_Flag(m_FSkillFlags, Attack_Add))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::ATTACK, m_tSkillInfo.fStatAttack);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, Sheild_Add))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::SHEILD, m_tSkillInfo.fStatSheild);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, SkillAtt_Add))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::SKILL, m_tSkillInfo.fSkillAttack);

	// set
	if (Engine_Utils::Has_Flag(m_FSkillFlags, Attack_Set))
		pStatCom->Set_Stat(CMyStat::STAT_TYPE::ATTACK, m_tSkillInfo.fStatAttack);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, Sheild_Set))
		pStatCom->Set_Stat(CMyStat::STAT_TYPE::SHEILD, m_tSkillInfo.fStatSheild);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, SkillAtt_Set))
		pStatCom->Set_Stat(CMyStat::STAT_TYPE::SKILL, m_tSkillInfo.fSkillAttack);

	// mental
	if (Engine_Utils::Has_Flag(m_FSkillFlags, Mental_Sub))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::MENTAL, m_tSkillInfo.fNeedMental * -1.f);
}

void CSkillBase::Check_EndFlag(CMyStat* pStatCom)
{
	if (pStatCom == nullptr)
		return;

	// sub
	if (Engine_Utils::Has_Flag(m_FSkillFlags, Attack_Sub))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::ATTACK, m_tSkillInfo.fStatAttack * -1.f);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, Sheild_Sub))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::SHEILD, m_tSkillInfo.fStatSheild * -1.f);

	if (Engine_Utils::Has_Flag(m_FSkillFlags, SkillAtt_Sub))
		pStatCom->Add_Stat(CMyStat::STAT_TYPE::SKILL, m_tSkillInfo.fSkillAttack * -1.f);
}

CSkillBase* CSkillBase::Create(void* pArg)
{
	CSkillBase* pInstance = new CSkillBase();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CSkillBase::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkillBase::Free()
{
	Super::Free();
}