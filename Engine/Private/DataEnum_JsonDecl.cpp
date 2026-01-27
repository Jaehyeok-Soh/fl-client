#include "Engine_pch.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")
#include "DataEnum_JsonDecl.h"

NS_BEGIN(DTO)

void to_json(json& j, const ECategory& e)
{
	switch (e)
	{
	case ECategory::MAP:
		j = "MAP";
		break;
	case ECategory::EFFECT:
		j = "EFFECT";
		break;
	case ECategory::UI:
		j = "UI";
		break;
	case ECategory::ANIMATION:
		j = "ANIMATION";
		break;
	case ECategory::CAMERA:
		j = "CAMERA";
		break;
	default:
		j = "END";
		break;
	}
}

void from_json(const json& j, ECategory& e)
{
	const std::string s = j.get<std::string>();
	if (s == "MAP")
		e = ECategory::MAP;
	else if (s == "EFFECT")
		e = ECategory::EFFECT;
	else if (s == "UI")
		e = ECategory::UI;
	else if (s == "ANIMATION")
		e = ECategory::ANIMATION;
	else if (s == "CAMERA")
		e = ECategory::CAMERA;
	else if(s == "END")
		e = ECategory::END;
}

NS_END