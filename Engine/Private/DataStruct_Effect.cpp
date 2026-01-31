#include "Engine_pch.h"
#include "DataStruct_Effect.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

inline void to_json(json& j, const TEFFECT_ContainerData& data)
{
	j = json
	{
		{ "Type", TEFFECT_ContainerData::eType },
		{ "strTag", data.strTag },
	};
}
inline void from_json(const json& j, TEFFECT_ContainerData& data)
{
	j.at("strTag").get_to(data.strTag);
}
inline void to_json(json& j, const TEFFECT_ContainerData& data)
{
	j = json
	{
		{ "Type", TEFFECT_ContainerData::eType },
		{ "strTag", data.strTag },
	};
}
inline void from_json(const json& j, TEFFECT_PartsData& data)
{
	j.at("strTag").get_to(data.strTag);
}

inline void to_json(json& j, const TEFFECT_PartsData& data)
{
	j = json
	{
		{ "Type", TEFFECT_PartsData::eType },
		{ "strTag", data.strTag },
	};
}

inline void from_json(const json& j, TEFFECT_PartsData& data)
{
	j.at("strTag").get_to(data.strTag);
}

NS_END


NS_BEGIN(Engine)

json CEFFECT_CONTAINER::ToJson() const
{
	return json(m_Data);
}

HRESULT CEFFECT_CONTAINER::FromJson(const json& j)
{
	m_Data = j.get<DTO::TEFFECT_ContainerData>();
	return S_OK;
}

json CEffect_PARTS::ToJson() const
{
	return json(m_Data);
}

HRESULT CEffect_PARTS::FromJson(const json& j)
{
	m_Data = j.get<DTO::TEFFECT_PartsData>();
	return S_OK;
}
NS_END;