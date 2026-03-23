#pragma once

namespace Engine
{
	// 어느공간에 있는가 ?
	enum class ECameraSpace
	{
		World = 0,
		Camera_Local
	};

	enum class ECameraFovMode
	{
		Delta = 0, // base Fov에 더하는 방식
		Absolute   // 목표 Fov
	};

	enum class ECameraModifierType
	{
		Shake = 0,
		Fov,
		Position_Offset,
		Rotation_Offset,
		Roll_Offset
	};

}