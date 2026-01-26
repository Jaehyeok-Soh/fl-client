#pragma once
#include "ObjectDataBase.h"


/// <summary>
/// 1. 각 Struct는 Type을 가지며 강제한다.
/// 2. Json 최상단에는 Type과 Tag가 표기되도록 한다.
/// </summary>

NS_BEGIN(DTO)

/////////////////-------------------  ObjectStruct  -------------------/////////////////
struct TExample_LightData
{
	static constexpr EMapType eType = EMapType::LIGHT;
	string strTag{ "" };

	_uint iValue{ 0 };
	_float iValue2{ 0.0f };
	_int iValue3{ -1 };
};

struct TExample_StaticModelData
{
	static constexpr EMapType eType = EMapType::STATICMODEL;
	string strTag{ "" };

	_uint iValue{ 0 };
	_float iValue2{ 0.0f };
	_int iValue3{ -1 };
};


/////////////////-------------------  to_json, from_json  -------------------/////////////////
inline void to_json(json& j, const TExample_LightData& data)
{
	j = json
	{
		{ "Type", TExample_LightData::eType },
		{ "strTag", data.strTag },
		{ "iValue", data.iValue },
		{ "iValue2", data.iValue2 },
		{ "iValue3", data.iValue3 }
	};
}

inline void from_json(const json& j, TExample_LightData& data)
{
	j.at("strTag").get_to(data.strTag);
	if (j.contains("iValue"))
		data.iValue = j["iValue"].get<_uint>();
	if (j.contains("iValue2"))
		data.iValue2 = j["iValue2"].get<_float>();
	if (j.contains("iValue3"))
		data.iValue3 = j["iValue3"].get<_int>();
}

inline void to_json(json& j, const TExample_StaticModelData& data)
{
	j = json
	{
		{ "Type", TExample_StaticModelData::eType },
		{ "strTag", data.strTag },
		{ "iValue", data.iValue },
		{ "iValue2", data.iValue2 },
		{ "iValue3", data.iValue3 }
	};
}

inline void from_json(const json& j, TExample_StaticModelData& data)
{
	j.at("strTag").get_to(data.strTag);
	if (j.contains("iValue"))
		data.iValue = j["iValue"].get<_uint>();
	if (j.contains("iValue2"))
		data.iValue2 = j["iValue2"].get<_float>();
	if (j.contains("iValue3"))
		data.iValue3 = j["iValue3"].get<_int>();
}

NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class CExample_LightData final : public DTO::IObjectDataBase
{
	using Super = DTO::IObjectDataBase;
private:
	CExample_LightData() = default;
	virtual ~CExample_LightData() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EMapType::LIGHT); }
	const string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override { return json(m_Data); }
	HRESULT FromJson(const json& j) override
	{
		m_Data = j.get<DTO::TExample_LightData>();
		return S_OK;
	}

	const DTO::TExample_LightData& Get_Data() const { return m_Data; }
	DTO::TExample_LightData& Get_Data() { return m_Data; }
private:
	DTO::TExample_LightData m_Data;
public:
	static CExample_LightData* Create() { return new CExample_LightData(); }
	virtual void Free() override { Super::Free(); }
};

class CExample_StaticModel final : public DTO::IObjectDataBase
{
	using Super = DTO::IObjectDataBase;
private:
	CExample_StaticModel() = default;
	virtual ~CExample_StaticModel() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EMapType::STATICMODEL); }
	const string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override { return json(m_Data); }
	HRESULT FromJson(const json& j) override
	{
		m_Data = j.get<DTO::TExample_StaticModelData>();
		return S_OK;
	}

	const DTO::TExample_StaticModelData& Get_Data() const { return m_Data; }
	DTO::TExample_StaticModelData& Get_Data() { return m_Data; }
private:
	DTO::TExample_StaticModelData m_Data;
public:
	static CExample_StaticModel* Create() { return new CExample_StaticModel(); }
	virtual void Free() override { Super::Free(); }
};

NS_END