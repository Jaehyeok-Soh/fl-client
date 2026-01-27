#include "Engine_pch.h"
#include "DataEnum.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

NLOHMANN_JSON_SERIALIZE_ENUM(ECategory,
	{
		{ECategory::MAP, "MAP"},
		{ECategory::EFFECT, "EFFECT"},
		{ECategory::UI, "UI"},
		{ECategory::ANIMATION, "ANIMATION"},
		{ECategory::CAMERA, "CAMERA"},
		{ECategory::END, "END"}
	}
)

NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		{EUIType::END, "END"}
	}
)

NLOHMANN_JSON_SERIALIZE_ENUM(EAnimationType,
	{
		{EAnimationType::END, "END"}
	}
)

NS_END