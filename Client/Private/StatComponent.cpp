#include "pch.h"
#include "StatComponent.h"
#include "GameInstance.h"

CStatComponent::CStatComponent()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CStatComponent::CStatComponent(const CStatComponent& rhs)
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CStatComponent::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatComponent::Awake(_uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatComponent::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	STATCOMP_DESC* pDesc	= static_cast<STATCOMP_DESC*>(pArg);
	m_iMaxHealth			= pDesc->iMaxHp;
	m_iHealth				= pDesc->iMaxHp;

	m_iMaxHealth			= pDesc->iAttack;
	m_iHealth				= pDesc->iSheild;
	m_iHealth				= pDesc->fShildRate;

	return S_OK;
}

void CStatComponent::Update(const _float fTimeDelta)
{
}

void CStatComponent::Add_Health(_int iHealth)
{
	if (iHealth > 0)
		Add_Hp(iHealth);

	else
		Sub_Hp(iHealth);
}

void CStatComponent::Add_Hp(_int iHealth)
{
	m_iHealth += iHealth;

	// max hp 넘었는지 검사
	if (m_iHealth > m_iMaxHealth)
		m_iHealth = m_iMaxHealth;
}

void CStatComponent::Sub_Hp(_int iHealth)
{
	m_iHealth += iHealth;

	// sheild 발동중이라면 : sheild 값 더해줌
	if (m_bSheildOn)
		m_iHealth += m_iSheild;

	// 만약 음수가 되었다면 0으로 맞추기
	if (m_iHealth < 0)
		m_iHealth = 0;
}

CStatComponent* CStatComponent::Create()
{
	CStatComponent* pInstance = new CStatComponent();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CStatComponent::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CStatComponent::Clone(void* pArg)
{
	CStatComponent* pInstance = new CStatComponent();
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStatComponent::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStatComponent::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
