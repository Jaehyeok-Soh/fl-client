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

HRESULT CDataDocument_EffectEvent::FromJson(const json& j)
{
    Clear();

    if (j.contains("Category"))
    {
        const DTO::ECategory eCategory = j.at("Category").get<DTO::ECategory>();
        if (eCategory != DTO::ECategory::EFFECTEVENT)
            return E_FAIL;
    }
    else
        return E_FAIL;

    if ((j.contains("Objects") == false) || (j["Objects"].is_array() == false))
        return E_FAIL;

    for (const auto& object : j["Objects"])
    {
        if (object.contains("Type") == false)
            return E_FAIL;

        const DTO::ECategory eType = object.at("Type").get<DTO::ECategory>();

        IObjectDataBase* pObjectDataBase = Create_ObjectData(eType);
        if (pObjectDataBase == nullptr)
            return E_FAIL;

        if (FAILED(pObjectDataBase->FromJson(object)))
        {
            Safe_Release(pObjectDataBase);
            return E_FAIL;
        }

        if (FAILED(Try_Add(pObjectDataBase)))
        {
            Safe_Release(pObjectDataBase);
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CDataDocument_EffectEvent::Try_Add(const DTO::ANIM_EVENT_INFO_DESC& data)
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
        return nullptr;
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