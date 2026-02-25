#include "Engine_pch.h"

#include "Effect_Manager.h"
#include "GameObject.h"
#include "EffectHandler.h"
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

void CEffect_Manager::Spawn_PoolEffect(CEffectHandler* handler, const std::string& UniqueEffectName, const std::string& strTag, const Matrix& matWorld, _float fDuration, _uint bIsLocal, _uint iFlag, const Matrix* pTargetBone, const Matrix* pTransMatrix)
{
    EFFECT_SPAWN_DESC tEngineDesc = {};
    tEngineDesc.matWorld = matWorld;
    tEngineDesc.fDuration = fDuration;
    tEngineDesc.iSimulationType = bIsLocal;
    tEngineDesc.pTargetBoneMatrix = &pTargetBone;
    tEngineDesc.pTransformMatrix = &pTransMatrix;
    tEngineDesc.iFlag = iFlag;

    wstring wstrPrototypeTag = L"POOL_" + Engine_Utils::ToWString(strTag);

    m_pGameInstance->Request_AddObject(
        m_pGameInstance->Get_CurrentLevelIndex(),
        wstrPrototypeTag,
        m_pGameInstance->Get_CurrentLevelIndex(),
        &tEngineDesc,
        [handler, UniqueEffectName](CGameObject* pGo)
        {
            pGo->Set_Name(UniqueEffectName);
            handler->PoolObject_CallBack(pGo);
        }
    );
}

void CEffect_Manager::Spawn_PoolEffect(const std::string& strTag, const Matrix& matWorld, _float fDuration, _uint bIsLocal, _uint iFlag, const Matrix* pTargetBone, const Matrix* pTransMatrix)
{
    EFFECT_SPAWN_DESC tEngineDesc = {};
    tEngineDesc.matWorld = matWorld;
    tEngineDesc.fDuration = fDuration;
    tEngineDesc.iSimulationType = bIsLocal;
    tEngineDesc.pTargetBoneMatrix = &pTargetBone;
    tEngineDesc.pTransformMatrix = &pTransMatrix;
    tEngineDesc.iFlag = iFlag;

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
