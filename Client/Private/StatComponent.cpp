#include "Client_Defines.h"
#include "GameInstance.h"
#include "StatComponent.h"

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

	STATCOMP_DESC* pDesc = static_cast<STATCOMP_DESC*>(pArg);
	m_iMaxHealth = pDesc->iHealth;
	m_iHealth = pDesc->iHealth;

	return S_OK;
}

void CStatComponent::Update(const _float fTimeDelta)
{
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
