#include "Engine_pch.h"
#include "GameDataManager.h"
#include "GameInstance.h"

CGameDataManager::CGameDataManager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGameDataManager::Initialize()
{
    return S_OK;
}

CGameDataManager* CGameDataManager::Create()
{
    CGameDataManager* pInstance = new CGameDataManager;
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CGameDataManager::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CGameDataManager::Free()
{
    Super::Free();
    Safe_Release(m_pGameInstance);
}
