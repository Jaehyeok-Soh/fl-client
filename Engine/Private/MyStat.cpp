#include "Engine_pch.h"
#include "MyStat.h"
#include "GameInstance.h"

CMyStat::CMyStat()
	: Super()
{

}

CMyStat::CMyStat(const CMyStat& rhs)
	: Super(rhs)
	, m_fMaxHealth(rhs.m_fMaxHealth)
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
	m_fMaxHealth			= pDesc->fMaxHp;
	m_fHealth				= pDesc->fMaxHp;

	m_fAttack = pDesc->fAttack;
	m_fSheild = pDesc->fSheild;
	if (m_fSheild > 0)
		m_bSheildOn = true;

	return S_OK;
}

void CMyStat::Add_Health(_float fHealth)
{
	if (fHealth > 0)
		Add_Hp(fHealth);

	else
		Sub_Hp(fHealth);
}

void CMyStat::Add_Hp(_float fHealth)
{
	m_fHealth += fHealth;

	// max hp 넘었는지 검사
	if (m_fHealth > m_fMaxHealth)
		m_fHealth = m_fMaxHealth;
}

void CMyStat::Sub_Hp(_float fHealth)
{
	m_fHealth += fHealth;

	// sheild 발동중이라면 : sheild 값 더해줌
	if (m_bSheildOn)
		m_fHealth += m_fSheild;

	// 만약 음수가 되었다면 0으로 맞추기
	if (m_fHealth < 0)
		m_fHealth = 0;
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
	Safe_Release(m_pGameInstance);
	Super::Free();
}
