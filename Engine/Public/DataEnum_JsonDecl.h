#pragma once
#include "json_forward.h"
#include "DataEnum.h"

NS_BEGIN(DTO)

ENGINE_DLL void to_json(json& j, const ECategory& e);
ENGINE_DLL void from_json(const json& j, ECategory& e);

NS_END

