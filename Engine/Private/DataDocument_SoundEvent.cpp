#include "Engine_pch.h"
#include "DataDocument_SoundEvent.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include "GameInstance.h"

CDataDocument_SoundEvent::CDataDocument_SoundEvent()
{
}

HRESULT CDataDocument_SoundEvent::Initialize()
{
    return S_OK;
}

json CDataDocument_SoundEvent::ToJson() const
{
    json j;
    j["Category"] = DTO::ECategory::SOUNDEVENT;

    json jsonArray = json::array();
    for (const auto& [iType, umapTags] : m_Datas)
    {
        for (const auto& [strTag, object] : umapTags)
        {
            if (object)
                jsonArray.push_back(object->ToJson());
        }
    }

    j["Objects"] = std::move(jsonArray);
    return j;
}

HRESULT CDataDocument_SoundEvent::Upsert(const DTO::SOUND_EVENT_INFO_DESC& data)
{
    if (data.strOwnerTag.empty())
        return E_FAIL;

    auto& typeMap = m_Datas[ENUM_TO_UINT(DTO::ECategory::SOUNDEVENT)];

    auto itr = typeMap.find(data.strOwnerTag);
    if (itr != typeMap.end())
    {
        auto* pStruct = static_cast<CDataStruct_SoundEvent*>(itr->second);
        if (pStruct == nullptr)
            return E_FAIL;

        pStruct->Get_Data() = data;
        return S_OK;
    }

    IObjectDataBase* pObjectBase = Create_ObjectData(DTO::ECategory::SOUNDEVENT);
    if (pObjectBase == nullptr)
        return E_FAIL;

    static_cast<CDataStruct_SoundEvent*>(pObjectBase)->Get_Data() = data;
    m_AllTags.insert(data.strOwnerTag);
    typeMap.emplace(data.strOwnerTag, pObjectBase);
    return S_OK;
}

IObjectDataBase* CDataDocument_SoundEvent::Create_ObjectData(DTO::ECategory eType)
{
    switch (eType)
    {
    case DTO::ECategory::SOUNDEVENT:
        return CDataStruct_SoundEvent::Create();
    default:
        return CDataStruct_SoundEvent::Create();
    }
}

const DTO::SOUND_EVENT_INFO_DESC* CDataDocument_SoundEvent::Find_Data(const string& strOwnerTag) const
{
    auto typeIter = m_Datas.find(ENUM_TO_UINT(DTO::ECategory::SOUNDEVENT));
    if (typeIter == m_Datas.end())
        return nullptr;

    auto dataIter = typeIter->second.find(strOwnerTag);
    if (dataIter == typeIter->second.end())
        return nullptr;

    auto* pStruct = static_cast<CDataStruct_SoundEvent*>(dataIter->second);
    if (pStruct == nullptr)
        return nullptr;

    return &pStruct->Get_Data();
}

HRESULT CDataDocument_SoundEvent::FromJson(const json& j)
{
    Clear();

    if (!j.contains("Category"))
        return E_FAIL;
    if (j.at("Category").get<DTO::ECategory>() != DTO::ECategory::SOUNDEVENT)
        return E_FAIL;

    if (!j.contains("Objects") || !j["Objects"].is_array())
        return E_FAIL;

    for (const auto& object : j["Objects"])
    {
        DTO::SOUND_EVENT_INFO_DESC data = object.get<DTO::SOUND_EVENT_INFO_DESC>();
        if (FAILED(Upsert(data)))
            return E_FAIL;
    }

    return S_OK;
}

CDataDocument_SoundEvent* CDataDocument_SoundEvent::Create()
{
    CDataDocument_SoundEvent* pInstance = new CDataDocument_SoundEvent();

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CDataDocument_SoundEvent::Free()
{
    Super::Free();
}