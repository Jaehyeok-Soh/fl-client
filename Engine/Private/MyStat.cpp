#include "Engine_pch.h"
#include "MyStat.h"

#include "Engine_Utils.h"

#include "GameInstance.h"

CMyStat::CMyStat()
	: Super()
{

}

CMyStat::CMyStat(const CMyStat& rhs)
	: Super(rhs)
	, m_vHealth(rhs.m_vHealth)
	, m_vDefense(rhs.m_vDefense)
	, m_vMental(rhs.m_vMental)
	, m_FStatFlags(rhs.m_FStatFlags)
{

}

HRESULT CMyStat::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMyStat::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	STAT_DESC* pDesc	= static_cast<STAT_DESC*>(pArg);
	m_vHealth = { pDesc->fMaxHp, pDesc->fMaxHp };
	m_vDefense = { pDesc->fDefense, pDesc->fDefense };
	m_vMental = { pDesc->fMental, pDesc->fMental };

	m_fAttack = pDesc->fAttack;
	m_fSheild = pDesc->fSheild;

	m_FStatFlags = pDesc->FStatFlags;

	return S_OK;
}

void CMyStat::Add_Health(_float fHealth)
{
	if (fHealth > 0)
		Add_Hp(fHealth);

	else
		Sub_Hp(fHealth);
}

void CMyStat::Add_Stat(STAT_TYPE eType, _float fValue)
{
	switch (eType)
	{
	case STAT_TYPE::HP:
		Add_Health(fValue);
		break;

	case STAT_TYPE::DEFENSE:
		Add_Defense(fValue);
		break;

	case STAT_TYPE::MENTAL:
		Add_Mental(fValue);
		break;

	case STAT_TYPE::ATTACK:
		m_fAttack += fValue;
		if (m_fAttack < 0.f)
			m_fAttack = 0.f;
		break;

	case STAT_TYPE::SHEILD:
		m_fSheild += fValue;
		if (m_fSheild < 0.f)
			m_fSheild = 0.f;
		break;

	case STAT_TYPE::SKILL:
		Add_Mental(fValue);
		break;
	}
}

void CMyStat::Set_Stat(STAT_TYPE eType, _float fValue)
{
	switch (eType)
	{
	case STAT_TYPE::HP:
		m_vHealth.x = fValue;
		break;

	case STAT_TYPE::DEFENSE:
		m_vDefense.x = fValue;
		break;

	case STAT_TYPE::MENTAL:
		m_vMental.x = fValue;
		break;

	case STAT_TYPE::ATTACK:
		m_fAttack = fValue;
		break;

	case STAT_TYPE::SHEILD:
		m_fSheild = fValue;
		break;

	case STAT_TYPE::SKILL:
		m_fSkillAtt = fValue;
		break;
	}

	// 안에서 검사 한번 돌도록
	Add_Stat(eType, 0.f);
}

void CMyStat::Update_Stat(const _float fTimeDelta)
{
	if (Engine_Utils::Has_Flag(m_FStatFlags, StatFlags::HpUpdate))
		Update_Hp(fTimeDelta);

	if (Engine_Utils::Has_Flag(m_FStatFlags, StatFlags::DefenseUpdtae))
		Update_Defense(fTimeDelta);

	if (Engine_Utils::Has_Flag(m_FStatFlags, StatFlags::MentalUpdate))
		Update_Mental(fTimeDelta);
}

void CMyStat::Set_Flag(_uint iFlag, _bool bOn)
{
	if (bOn)
	{
		Engine_Utils::Add_Flag(m_FStatFlags, iFlag);
	}

	else
	{
		Engine_Utils::RemoveHard_Flag(m_FStatFlags, iFlag);
	}
}

void CMyStat::Add_Hp(_float fHealth)
{
	m_vHealth.x += fHealth;

	// max hp 넘었는지 검사
	if (m_vHealth.x > m_vHealth.y)
		m_vHealth.x = m_vHealth.y;
}

void CMyStat::Sub_Hp(_float fHealth)
{
	m_vHealth.x += fHealth;

	// sheild 발동중이라면 : sheild 값 더해줌
	if (Engine_Utils::Has_Flag(m_FStatFlags, StatFlags::SheildOn))
		m_vHealth.x += m_fSheild;

	// 만약 음수가 되었다면 0으로 맞추기
	if (m_vHealth.x < 0)
		m_vHealth.x = 0;
}

void CMyStat::Add_Defense(_float fValue)
{
	m_vDefense.x += fValue;

	// max hp 넘었는지 검사
	if (m_vDefense.x > m_vDefense.y)
		m_vDefense.x = m_vDefense.y;

	if (m_vDefense.x < 0.f)
		m_vDefense.x = 0.f;
}

void CMyStat::Add_Mental(_float fValue)
{
	m_vMental.x += fValue;

	// max hp 넘었는지 검사
	if (m_vMental.x > m_vMental.y)
		m_vMental.x = m_vMental.y;

	if (m_vMental.x < 0.f)
		m_vMental.x = 0.f;
}

void CMyStat::Add_SkillAtt(_float fValue)
{
	m_fSkillAtt += fValue;

	if (m_fSkillAtt < 0.f)
		m_fSkillAtt = 0.f;
}

void CMyStat::Update_Hp(const _float fTimeDelta)
{
	Add_Hp(fTimeDelta);
}

void CMyStat::Update_Defense(const _float fTimeDelta)
{
	Add_Defense(fTimeDelta);
}

void CMyStat::Update_Mental(const _float fTimeDelta)
{
	Add_Mental(fTimeDelta);
}

CMyStat* CMyStat::Create()
{
	CMyStat* pInstance = new CMyStat();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMyStat::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMyStat::Clone(void* pArg)
{
	CMyStat* pInstance = new CMyStat();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMyStat::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMyStat::Free()
{
	Super::Free();
}
