#pragma once
#include "Engine_Define.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

enum class ECategory : _uint
{
	MAP,
	EFFECT,
	UI,
	ANIMATION,
	CAMERA,
	END
};
inline constexpr _uint g_CategoryCount{ ENUM_TO_UINT(ECategory::END) };
NLOHMANN_JSON_SERIALIZE_ENUM(ECategory,
	{
		{ECategory::MAP, "MAP"},
		{ECategory::EFFECT, "EFFECT"},
		{ECategory::UI, "UI"},
		{ECategory::ANIMATION, "ANIMATION"},
		{ECategory::CAMERA, "CAMERA"}
	}
)

/////////////////-------------------  MAP  -------------------/////////////////
enum class EMapType : _uint
{
	STATICMODEL,
	LIGHT,
	END
};
inline constexpr _uint g_MapTypeCount{ ENUM_TO_UINT(EMapType::END) };
NLOHMANN_JSON_SERIALIZE_ENUM(EMapType,
	{
		{EMapType::STATICMODEL, "STATICMODEL"},
		{EMapType::LIGHT, "LIGHT"}
	}
)


/////////////////-------------------  UI  -------------------/////////////////
enum class EUIType : _uint
{
	END
};
inline constexpr _uint g_UITypeCount{ ENUM_TO_UINT(EUIType::END) };
NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		
	}
)


/////////////////-------------------  EFFECT  -------------------/////////////////
enum class EEffectType : _uint
{
	END
};
inline constexpr _uint g_EffectTypeCount{ ENUM_TO_UINT(EEffectType::END) };
NLOHMANN_JSON_SERIALIZE_ENUM(EEffectType,
	{

	}
)


/////////////////-------------------  ANIMATION  -------------------/////////////////
enum class EAnimationType : _uint
{
	END
};
inline constexpr _uint g_AnimationTypeCount{ ENUM_TO_UINT(EAnimationType::END) };
NLOHMANN_JSON_SERIALIZE_ENUM(EAnimationType,
	{

	}
)
NS_END