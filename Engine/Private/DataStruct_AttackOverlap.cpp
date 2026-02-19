#include "Engine_pch.h"
#include "DataStruct_AttackOverlap.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

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