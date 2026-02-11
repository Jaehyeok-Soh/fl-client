#include "Engine_pch.h"
#include "DataStruct_AttackOverlap.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

void to_json(json& j, const HITBOX_DESC& data)
{
	j = json
	{
		{ "strName", data.strName },
		{ "eType",data.eType},
		{ "vExtents", { { "x", data.vExtents.x },{ "y", data.vExtents.y },{ "z", data.vExtents.z } } },
		{ "fRadius", data.fRadius },
		{ "vOffset", { { "x", data.vOffset.x },{ "y", data.vOffset.y },{ "z", data.vOffset.z } } },
		{ "fDuration", data.fDuration },
		{ "fTickTime", data.fTickTime },
		{ "fDamage", data.fDamage },
		{ "iMaxHit", data.iMaxHit },
		{ "eFilterLayer", data.eFilterLayer },
		{ "iFilterMask", data.iFilterMask },
	};
}

void from_json(const json& j, HITBOX_DESC& data)
{
	j.at("strName").get_to(data.strName);
	j.at("eType").get_to(data.eType);

	const auto& jext = j.at("vExtents");
	jext.at("x").get_to(data.vExtents.x);
	jext.at("y").get_to(data.vExtents.y);
	jext.at("z").get_to(data.vExtents.z);

	j.at("fRadius").get_to(data.fRadius);

	const auto& joff = j.at("vOffset");
	joff.at("x").get_to(data.vOffset.x);
	joff.at("y").get_to(data.vOffset.y);
	joff.at("z").get_to(data.vOffset.z);

	j.at("fDuration").get_to(data.fDuration);
	j.at("fTickTime").get_to(data.fTickTime);
	j.at("fDamage").get_to(data.fDamage);
	j.at("iMaxHit").get_to(data.iMaxHit);
	j.at("eFilterLayer").get_to(data.eFilterLayer);
	j.at("iFilterMask").get_to(data.iFilterMask);
}

void to_json(json& j, const ATTACKEVENT& data)
{
	j = json
	{
		{ "strDescription", data.strDescription },
		{ "fAnimIndex",data.fAnimIndex},
		{ "fStartTrackPosition", data.fStartTrackPosition },
		{ "tHitboxDesc", data.tHitboxDesc },
	};
}

void from_json(const json& j, ATTACKEVENT& data)
{
	j.at("strDescription").get_to(data.strDescription);
	j.at("fAnimIndex").get_to(data.fAnimIndex);
	j.at("fStartTrackPosition").get_to(data.fStartTrackPosition);
	j.at("tHitboxDesc").get_to(data.tHitboxDesc);
}

void to_json(json& j, const ATTACKOVERLAP_DESC& data)
{
	j = json
	{
		{ "strTag", data.strTag },
		{ "iNumPool",data.iNumPool},
		{ "attackEvents", data.attackEvents },
	};
}

void from_json(const json& j, ATTACKOVERLAP_DESC& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("iNumPool").get_to(data.iNumPool);
	j.at("attackEvents").get_to(data.attackEvents);
}

NS_END

NS_BEGIN(Engine)

json CDataStruct_AttackOverlap::ToJson() const
{
	return json(m_Data);
}

HRESULT CDataStruct_AttackOverlap::FromJson(const json& j)
{
	m_Data = j.get<DTO::ATTACKOVERLAP_DESC>();
	return S_OK;
}

NS_END