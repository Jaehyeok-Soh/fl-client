#include "GameObject.h"
#include "GameInstance.h"
#include "Component.h"

CComponent::CComponent()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_bClone(false)
{
	Safe_AddRef(m_pGameInstance);
}

CComponent::CComponent(const CComponent& rhs)
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_bClone(true)
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComponent::Set_Owner(CGameObject* pOwner)
{
	if (!pOwner)
		return E_FAIL;

	m_pOwner = pOwner;
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
