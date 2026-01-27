#include "Engine_pch.h"
#include "DataStruct_Effect.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

// DTO
NS_BEGIN(DTO)

NLOHMANN_JSON_SERIALIZE_ENUM(EEffectType,
	{
		{EEffectType::END, "END"}
	}
)

NS_END