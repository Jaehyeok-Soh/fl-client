#include "Engine_pch.h"
#include "DataDocument_CameraControlEvent.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include "GameInstance.h"

CDataDocument_CameraControlEvent::CDataDocument_CameraControlEvent()
    : Super()
{
}



json CDataDocument_CameraControlEvent::ToJson() const
{
    json j;
    j["Category"] = DTO::ECategory::CAMERACONTROLEVENT;

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

HRESULT CDataDocument_CameraControlEvent::FromJson(const json& j)
{
    Clear();

    if (!j.contains("Category"))
        return E_FAIL;
    if (j.at("Category").get<DTO::ECategory>() != DTO::ECategory::CAMERACONTROLEVENT)
        return E_FAIL;

    if (!j.contains("Objects") || !j["Objects"].is_array())
        return E_FAIL;

    for (const auto& object : j["Objects"])
    {
        DTO::CAMERACONTROL_EVENT_INFO_DESC data = object.get<DTO::CAMERACONTROL_EVENT_INFO_DESC>();
        if (FAILED(Upsert(data)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CDataDocument_CameraControlEvent::Upsert(const DTO::CAMERACONTROL_EVENT_INFO_DESC& data)
{
    if (data.strOwnerTag.empty())
        return E_FAIL;

    auto& typeMap = m_Datas[ENUM_TO_UINT(DTO::ECategory::CAMERACONTROLEVENT)];

    auto itr = typeMap.find(data.strOwnerTag);
    if (itr != typeMap.end())
    {
        auto* pStruct = static_cast<CDataStruct_CameraControlEvent*>(itr->second);
        if (pStruct == nullptr)
            return E_FAIL;

        pStruct->Get_Data() = data;
        return S_OK;
    }

    IObjectDataBase* pObjectBase = Create_ObjectData();
    if (pObjectBase == nullptr)
        return E_FAIL;

    static_cast<CDataStruct_CameraControlEvent*>(pObjectBase)->Get_Data() = data;
    m_AllTags.insert(data.strOwnerTag);
    typeMap.emplace(data.strOwnerTag, pObjectBase);
    return S_OK;
}

const DTO::CAMERACONTROL_EVENT_INFO_DESC* CDataDocument_CameraControlEvent::Find_Data(const string& strOwnerTag) const
{
    auto typeIter = m_Datas.find(ENUM_TO_UINT(DTO::ECategory::CAMERACONTROLEVENT));
    if (typeIter == m_Datas.end())
        return nullptr;

    auto dataIter = typeIter->second.find(strOwnerTag);
    if (dataIter == typeIter->second.end())
        return nullptr;

    auto* pStruct = static_cast<CDataStruct_CameraControlEvent*>(dataIter->second);
    if (pStruct == nullptr)
        return nullptr;

    return &pStruct->Get_Data();
}

IObjectDataBase* CDataDocument_CameraControlEvent::Create_ObjectData()
{
    return CDataStruct_CameraControlEvent::Create();
}

CDataDocument_CameraControlEvent* CDataDocument_CameraControlEvent::Create()
{
    CDataDocument_CameraControlEvent* pInstance = new CDataDocument_CameraControlEvent();

    if (FAILED(pInstance->Initialize()))
        Safe_Release(pInstance);

    return pInstance;
}

void CDataDocument_CameraControlEvent::Free()
{
    Super::Free();
}