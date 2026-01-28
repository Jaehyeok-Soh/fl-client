#include "Engine_pch.h"
#include "DataStruct_Example.h"

// Engine
NS_BEGIN(Engine)

json CExample_LightData::ToJson() const
{
    json j;
    DTO::to_json(j, m_Data);
    return j;
}

HRESULT CExample_LightData::FromJson(const json& j)
{
    try
    {
        DTO::from_json(j, m_Data);
        return S_OK;
    }
    catch (...)
    {
        return E_FAIL;
    }
}

CExample_LightData* CExample_LightData::Create()
{
	return new CExample_LightData();
}

json CExample_StaticModel::ToJson() const
{
    json j;
    DTO::to_json(j, m_Data);
    return j;
}

HRESULT CExample_StaticModel::FromJson(const json& j)
{
    try
    {
        DTO::from_json(j, m_Data);
        return S_OK;
    }
    catch (...)
    {
        return E_FAIL;
    }
}

CExample_StaticModel* CExample_StaticModel::Create()
{
	return new CExample_StaticModel();
}


NS_END