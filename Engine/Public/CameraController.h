#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCameraController final : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::CAMERACONTROLLER;
private:

};

NS_END