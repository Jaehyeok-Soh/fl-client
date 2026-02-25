#include "Engine_pch.h"

#include "Effect_Manager.h"
#include "GameInstance.h"

CEffect_Manager::CEffect_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEffect_Manager::Initialize()
{
	return S_OK;
}

void CEffect_Manager::Spawn_Effect(const std::string& strTag, const Matrix& matWorld, _float fDuration, _bool bIsLocal, void* pTargetBone)
{
    EFFECT_SPAWN_DESC tEngineDesc = {};
    tEngineDesc.matWorld = matWorld;
    tEngineDesc.fDuration = fDuration;
    tEngineDesc.iSimulationType = bIsLocal ? 1 : 0;
    tEngineDesc.pTargetBoneMatrix = (const Matrix*)pTargetBone;

    wstring wstrPrototypeTag = L"POOL_" + Engine_Utils::ToWString(strTag);

    m_pGameInstance->Request_AddObject(
        m_pGameInstance->Get_CurrentLevelIndex(),
        wstrPrototypeTag,
        m_pGameInstance->Get_CurrentLevelIndex(),
        &tEngineDesc
    );
}

CEffect_Manager* CEffect_Manager::Create()
{
    CEffect_Manager* pInstance = new CEffect_Manager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("CEffect_Manager::Create, Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}


void CEffect_Manager::Free()
{
	Super::Free();
	Safe_Release(m_pGameInstance);
}