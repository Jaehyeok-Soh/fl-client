#include "Engine_pch.h"
#include "DataStruct_AttackPreset.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

NS_END

NS_BEGIN(Engine)

json CDataStruct_AttackPreset::ToJson() const
{
	json j;
	DTO::to_json(j, m_Data);
	return j;
}

HRESULT CDataStruct_AttackPreset::FromJson(const json& j)
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

NS_END