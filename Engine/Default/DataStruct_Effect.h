#pragma once
#include "ObjectDataBase.h"

/////////////////-------------------  EFFECT  -------------------/////////////////
enum class EEffectType : _uint
{
	END
};
inline constexpr _uint g_EffectTypeCount{ ENUM_TO_UINT(EEffectType::END) };