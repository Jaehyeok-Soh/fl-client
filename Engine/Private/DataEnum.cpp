#include "Engine_pch.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include "DataEnum.h"

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


//TMP
NS_BEGIN(Engine)

NLOHMANN_JSON_SERIALIZE_ENUM(EMaterialInstanceType, {
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
	{EMaterialInstanceType::CourtBlue, "CourtBlue" } }
)

NLOHMANN_JSON_SERIALIZE_ENUM(LIGHT_TYPE, {
	{LIGHT_TYPE::DIRECTIONAL, "DIRECTIONAL"},
	{LIGHT_TYPE::STATICPOINT, "STATIC_POINT"},
	{LIGHT_TYPE::DYNAMICPOINT, "DYNAMIC_POINT"} }
)

NS_END