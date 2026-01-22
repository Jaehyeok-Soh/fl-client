#include "Prototype_Manager.h"
#include "Component.h"
#include "GameObject.h"
#include "Layer.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iLevelCount)
{
    m_iLevelCount = iLevelCount;

    m_pPrototypes = vector<map<const wstring, CBase*>>{m_iLevelCount};

    return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag, CBase* pPrototype)
{
    if (Find_Prototype(iLevelIndex, wstrPrototypeTag))
        return E_FAIL;

    m_pPrototypes[iLevelIndex].insert(map<const wstring, CBase*>::value_type(wstrPrototypeTag, pPrototype));
    return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(EPrototypeType ePrototypeID, _uint iLevelIndex, const wstring& wstrPrototypeTag, void* pArg)
{
    CBase* pReturn = { nullptr };

    if (CBase* pFinded = Find_Prototype(iLevelIndex, wstrPrototypeTag))
    {
        switch (ePrototypeID)
        {
        case Engine::EPrototypeType::GAMEOBJECT:
            pReturn = dynamic_cast<CGameObject*>(pFinded)->Clone(pArg);
            break;
        case Engine::EPrototypeType::COMPONENT:
            pReturn = dynamic_cast<CComponent*>(pFinded)->Clone(pArg);
            break;
        }
    }

    return pReturn;
}

void CPrototype_Manager::Clear(_uint iLevelIndex)
{
    for (auto &Pair: m_pPrototypes[iLevelIndex])
    {
        Safe_Release(Pair.second);
    }
    m_pPrototypes[iLevelIndex].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iLevelIndex, const wstring& wstrPrototypeTag)
{
    if (iLevelIndex >= m_iLevelCount)
        return nullptr;

    auto itr = m_pPrototypes[iLevelIndex].find(wstrPrototypeTag);
    if (itr == m_pPrototypes[iLevelIndex].end())
        return nullptr;

    return itr->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iLayerCount)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(iLayerCount)))
    {
        MSG_BOX("CPrototype_Manager::Create, Failed");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPrototype_Manager::Free()
{
    for (map<const wstring, CBase*>& Element : m_pPrototypes)
    {
        for (auto& Pair : Element)
        {
            Safe_Release(Pair.second);
        }
        Element.clear();
    }
    m_pPrototypes.clear();

    Super::Free();
}
