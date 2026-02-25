#include "Engine_pch.h"
#include "BuilderSystem.h"
#include "BuilderBase.h"
#include "GameInstance.h"

CBuilderSystem::CBuilderSystem()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	m_arrBuilders.fill(nullptr);
}

HRESULT CBuilderSystem::Initialize()
{
	return S_OK;
}

HRESULT CBuilderSystem::Ready_Builder(DTO::ECategory eCategory, CBuilderBase* pBuilder)
{
	if (pBuilder == nullptr)
		return E_FAIL;

	if (Has_Builder(eCategory) == false)
		m_arrBuilders[ENUM_TO_UINT(eCategory)] = pBuilder;
	else
	{
		MSG_BOX("CBuilderSystem::Ready_Builder, Already have builder");
	}
	
	return S_OK;
}

HRESULT CBuilderSystem::Build_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey)
{
	if (Has_Builder(eCategory) == false)
		return E_FAIL;

	const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory, strFileKey);
	if (pBase == nullptr)
		return E_FAIL;

	return m_arrBuilders[ENUM_TO_UINT(eCategory)]->Build(*pBase);
}

HRESULT CBuilderSystem::Rebuilde_File(_uint iLevelID, DTO::ECategory eCategory, string strFileKey)
{
	if (Has_Builder(eCategory) == true)
		Safe_Release(m_arrBuilders[ENUM_TO_UINT(eCategory)]);

	const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory, strFileKey);
	if (pBase == nullptr)
		return E_FAIL;

	return m_arrBuilders[ENUM_TO_UINT(eCategory)]->Build(*pBase);
}

CBuilderSystem* CBuilderSystem::Create()
{
	CBuilderSystem* pInstance = new CBuilderSystem();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilderSystem::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuilderSystem::Free()
{
	for (CBuilderBase* pBuilder : m_arrBuilders)
		Safe_Release(pBuilder);
	m_arrBuilders.fill(nullptr);

	Safe_Release(m_pGameInstance);
	Super::Free();
}
