#include "Engine_pch.h"
#include "DataDocument_EffectEvent.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(Engine)

CDataDocument_EffectEvent::CDataDocument_EffectEvent()
{
}

HRESULT CDataDocument_EffectEvent::Initialize()
{
    return S_OK;
}

json CDataDocument_EffectEvent::ToJson() const
{
    json j;
    j["Category"] = DTO::ECategory::EFFECTEVENT;

    json jsonArray = json::array();
    for (const auto& [iType, umapTags] : m_Datas)
    {
        for (const auto& [strTag, object] : umapTags)
        {
            if (nullptr != object)
                jsonArray.push_back(object->ToJson());
        }
    }

    j["Objects"] = std::move(jsonArray);
    return j;
}

HRESULT CDataDocument_EffectEvent::Try_Add(const DTO::EFFECT_EVENT_INFO_DESC& data)
{
    IObjectDataBase* pObjectBase = Create_ObjectData(DTO::ECategory::EFFECTEVENT);
    if (nullptr == pObjectBase)
        return E_FAIL;

    static_cast<CDataStruct_EffectEvent*>(pObjectBase)->Get_Data() = data;

    return Try_Add(pObjectBase);
}

IObjectDataBase* CDataDocument_EffectEvent::Create_ObjectData(DTO::ECategory eType)
{
    switch (eType)
    {

    case DTO::ECategory::EFFECTEVENT:
        return CDataStruct_EffectEvent::Create();
    default:
        return CDataStruct_EffectEvent::Create();
    }
}

HRESULT CDataDocument_EffectEvent::Try_Add(IObjectDataBase* pObject)
{
    if (nullptr == pObject)
        return E_FAIL;

    const string& strTag = pObject->Get_Tag();
    if (strTag.empty())
    {
        Safe_Release(pObject);
        return E_FAIL;
    }

    if (m_AllTags.find(strTag) != m_AllTags.end())
    {
        Safe_Release(pObject);
        return E_FAIL;
    }

    m_AllTags.insert(strTag);

    m_Datas[pObject->Get_Type()].emplace(strTag, pObject);

    return S_OK;
}

HRESULT CDataDocument_EffectEvent::FromJson(const json& j)
{
    Clear();

    if (!j.contains("Category")) return E_FAIL;
    if (j.at("Category").get<DTO::ECategory>() != DTO::ECategory::EFFECTEVENT)
        return E_FAIL;

    if (!j.contains("Objects") || !j["Objects"].is_array())
        return E_FAIL;

    for (const auto& object : j["Objects"])
    {
        DTO::ECategory eType = DTO::ECategory::EFFECTEVENT;
        if (object.contains("Type"))
            eType = static_cast<DTO::ECategory>(object.at("Type").get<_uint>());

        if (!object.contains("strOwnerTag")) continue;
        string strTag = object.at("strOwnerTag").get<string>();

        IObjectDataBase* pDataStruct = nullptr;

        auto& typeMap = m_Datas[(_uint)eType];
        auto iter = typeMap.find(strTag);

        if (iter != typeMap.end())
        {
            pDataStruct = iter->second;
        }
        else
        {
            pDataStruct = Create_ObjectData(eType);
            if (nullptr == pDataStruct) return E_FAIL;

            m_AllTags.insert(strTag);
            typeMap.emplace(strTag, pDataStruct);
        }

        if (FAILED(pDataStruct->FromJson(object)))
        {
            return E_FAIL;
        }
    }

    return S_OK;
}

CDataDocument_EffectEvent* CDataDocument_EffectEvent::Create()
{
    CDataDocument_EffectEvent* pInstance = new CDataDocument_EffectEvent();

    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDataDocument_EffectEvent::Free()
{
    Super::Free();
}

NS_END