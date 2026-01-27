#pragma once
#include "Base.h"
#include "DataEnum.h"
#include "json_forward.h"

NS_BEGIN(Engine)

class ENGINE_DLL IObjectDataBase abstract : public Engine::CBase
{
	using Super = Engine::CBase;
protected:
	IObjectDataBase() {};
	virtual ~IObjectDataBase() = default;
public:
	virtual _uint Get_Type() const PURE;
	virtual const string &Get_Tag() const PURE;

	virtual json ToJson() const PURE;
	virtual HRESULT FromJson(const json& j) PURE;

	virtual void Free() override { Super::Free(); }
};

NS_END