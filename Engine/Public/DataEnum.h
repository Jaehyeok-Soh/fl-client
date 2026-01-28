#pragma once
#include "json_forward.h"

/*
* Category를 제외한 아래의 EUIType 등을 각자 DataStruct선언으로 옮기세요
*/

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
		{ECategory::CAMERA, "CAMERA"},
		{ECategory::END, "END"}
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
		{EUIType::END, "END"}
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
		{EAnimationType::END, "END"}
	}
)

NS_END

//TMP
NS_BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(EMaterialInstanceType,
	{
		{EMaterialInstanceType::Default, "Default"},
		{EMaterialInstanceType::Concrete, "Concrete"},
		{EMaterialInstanceType::Mirror, "Mirror"},
		{EMaterialInstanceType::Water, "Water"},
		{EMaterialInstanceType::Dirt, "Dirt"},
		{EMaterialInstanceType::Red, "Red"},
		{EMaterialInstanceType::Blue, "Blue"},
		{EMaterialInstanceType::Green, "Green"},
		{EMaterialInstanceType::Grass, "Grass"},
		{EMaterialInstanceType::Orange, "Orange"},
		{EMaterialInstanceType::Brown, "Brown" },
		{EMaterialInstanceType::Pupple, "Pupple" },
		{EMaterialInstanceType::WinterGreen, "WinterGreen" },
		{EMaterialInstanceType::BurnishedBrown, "BurnishedBrown" },
		{EMaterialInstanceType::ConcreteLight, "ConcreteLight" },
		{EMaterialInstanceType::ConcreteMid, "ConcreteMid" },
		{EMaterialInstanceType::ConcreteWarm, "ConcreteWarm" },
		{EMaterialInstanceType::MetalCool, "MetalCool" },
		{EMaterialInstanceType::MetalDark, "MetalDark" },
		{EMaterialInstanceType::CourtBlue, "CourtBlue" }
	}
)

NLOHMANN_JSON_SERIALIZE_ENUM(LIGHT_TYPE,
	{
		{LIGHT_TYPE::DIRECTIONAL, "DIRECTIONAL"},
		{LIGHT_TYPE::STATICPOINT, "STATIC_POINT"},
		{LIGHT_TYPE::DYNAMICPOINT, "DYNAMIC_POINT"}
	}
)

NS_END