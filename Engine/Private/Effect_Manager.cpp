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

void CEffect_Manager::Request_Effect(CEffectHandler* handler, const std::string& UniqueEffectName, const std::string& strTag, EFFECT_SPAWN_DESC& Desc)
{
    wstring wstrPrototypeTag = L"POOL_" + Engine_Utils::ToWString(strTag);

    m_pGameInstance->Request_AddObject(
        m_pGameInstance->Get_CurrentLevelIndex(),
        wstrPrototypeTag,
        m_pGameInstance->Get_CurrentLevelIndex(),
        &Desc,
        [handler, UniqueEffectName](CGameObject* pGo)
        {
            pGo->Set_Name(UniqueEffectName);
            handler->PoolObject_CallBack(pGo);
        }
    );
}

void CEffect_Manager::Request_Effect(const std::string& strTag, EFFECT_SPAWN_DESC& Desc)
{
    wstring wstrPrototypeTag = L"POOL_" + Engine_Utils::ToWString(strTag);

    m_pGameInstance->Request_AddObject(
        m_pGameInstance->Get_CurrentLevelIndex(),
        wstrPrototypeTag,
        m_pGameInstance->Get_CurrentLevelIndex(),
        &Desc
    );
}

void CEffect_Manager::Notify_EffectDespawn(_uint iEffectID)
{

}

void CEffect_Manager::Push_EffectData(_uint iHashTag, void* Desc)
{
    CEffectBase::EFFECT_CONTAINERDESC* pDesc = static_cast<CEffectBase::EFFECT_CONTAINERDESC*>(Desc);
    if (pDesc == nullptr)
    {
        MSG_BOX("Desc이 Null입니다 : EffectManager");
        return;
    }

    auto result = m_EffectDescData.emplace(iHashTag, *pDesc);
}

void* CEffect_Manager::Find_EffectData(_uint iHashTag)
{
    auto iter = m_EffectDescData.find(iHashTag);

    if (iter == m_EffectDescData.end())
    {
        static CEffectBase::EFFECT_CONTAINERDESC tEmptyDesc = {};
        MSG_BOX("Hash Tag에 맞는 EffectData가 없습니다. : EffectManager");
        return nullptr;
    }

    return &(iter->second);
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
	Safe_Release(m_pGameInstance);
    m_mEffectData.clear();
    m_EffectDescData.clear();

	Super::Free();
}
