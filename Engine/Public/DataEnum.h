#pragma once

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


/////////////////-------------------  ANIMATION  -------------------/////////////////
enum class EAnimationType : _uint
{
	END
};
inline constexpr _uint g_AnimationTypeCount{ ENUM_TO_UINT(EAnimationType::END) };

NS_END