#include "Engine_pch.h"
#include "DataStruct_Example.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

// DTO
NS_BEGIN(DTO)

NLOHMANN_JSON_SERIALIZE_ENUM(EMapType,
	{
		{EMapType::STATICMODEL, "STATICMODEL"},
		{EMapType::LIGHT, "LIGHT"},
		{EMapType::END, "END"}
	}
)

void to_json(json& j, const EMapType& e)
{
	switch (e)
	{
	case EMapType::STATICMODEL:
		j = "STATICMODEL";
		break;
	case EMapType::LIGHT:
		j = "LIGHT";
		break;
	default:
		j = "END";
		break;
	}
}

void from_json(const json& j, EMapType& e)
{
	const std::string s = j.get<std::string>();
	if (s == "STATICMODEL")
		e = EMapType::STATICMODEL;
	else if (s == "LIGHT")
		e = EMapType::LIGHT;
	else if (s == "END")
		e = EMapType::END;
}

void to_json(json& j, const TExample_LightData& data)
{
	j = json
	{
		{ "Type", TExample_LightData::eType },
		{ "strTag", data.strTag },
		{ "iValue", data.iValue },
		{ "iValue2", data.iValue2 },
		{ "iValue3", data.iValue3 }
	};
}
void from_json(const json& j, TExample_LightData& data)
{
	j.at("strTag").get_to(data.strTag);
	if (j.contains("iValue"))
		data.iValue = j["iValue"].get<_uint>();
	if (j.contains("iValue2"))
		data.iValue2 = j["iValue2"].get<_float>();
	if (j.contains("iValue3"))
		data.iValue3 = j["iValue3"].get<_int>();
}
void to_json(json& j, const TExample_StaticModelData& data)
{
	j = json
	{
		{ "Type", TExample_StaticModelData::eType },
		{ "strTag", data.strTag },
		{ "iValue", data.iValue },
		{ "iValue2", data.iValue2 },
		{ "iValue3", data.iValue3 }
	};
}
void from_json(const json& j, TExample_StaticModelData& data)
{
	j.at("strTag").get_to(data.strTag);
	if (j.contains("iValue"))
		data.iValue = j["iValue"].get<_uint>();
	if (j.contains("iValue2"))
		data.iValue2 = j["iValue2"].get<_float>();
	if (j.contains("iValue3"))
		data.iValue3 = j["iValue3"].get<_int>();
}

NS_END


// Engine
NS_BEGIN(Engine)

json CExample_LightData::ToJson() const
{
	return json(m_Data);
}

HRESULT CExample_LightData::FromJson(const json& j)
{
	m_Data = j.get<DTO::TExample_LightData>();
	return S_OK;
}

json CExample_StaticModel::ToJson() const
{
	return json(m_Data);
}

HRESULT CExample_StaticModel::FromJson(const json& j)
{
	m_Data = j.get<DTO::TExample_StaticModelData>();
	return S_OK;
}

NS_END