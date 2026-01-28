#pragma once

/////////////////-------------------  EFFECT  -------------------/////////////////
enum class EEffectType : _uint
{
	END
};
inline constexpr _uint g_EffectTypeCount{ ENUM_TO_UINT(EEffectType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EEffectType,
	{
		{EEffectType::END, "END"}
	}
)