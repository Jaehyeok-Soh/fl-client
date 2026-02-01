#pragma once

#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "json_forward.h"

#pragma region 최초 사용법
/*
* - 각 Category당
*	 데이터 .h, .cpp (ex. DataStruct_Exmaple)
*    CDoucmentBase 상속 객체 (ex. DataDocument_Example)
*	 Tool, Client쪽 Builder 상속 객체 (ex. Builder_Example)
* - 아래 규칙에 따라 struct, to_json, from_json, wrapper class 선언
* - 만든 Struct헤더 DataDefine_Json.h헤더에 추가
*/
#pragma endregion

/*
* 1. 각 Struct는 Type을 가지며 강제 한다.
* 2. Struct 최상단에는 Type과 Tag가 선언 한다.
* 3. 구조체와 wrapper클래스를 1대1로 만들어야 한다.
*		데이터를 담는 struct, 그걸 wrapping하고 타입을 구분하기위한 class로 역할 분리
*		WarpperClass Engine_DLL 필수
*		cpp에도 구조체, enum class 에는 DTO 네임스페이스, 엔진선언부에는 Engine 네임스페이스
*		json이 헤더에 노출된다면?
*		헤더에는 전방선언용 "json_forward.h"선언, 헤더에서 json.hpp 인클루드 금지
*/
NS_BEGIN(DTO)

/////////////////-------------------  MAP  -------------------/////////////////
enum class EMapObjectType : _uint
{
	STATICMODEL,
	LIGHT,
	END
};

inline constexpr _uint g_MapTypeCount{ ENUM_TO_UINT(EMapType::END) };

/* String으로 자동 변환되어서 저당된다  */

NLOHMANN_JSON_SERIALIZE_ENUM(EMapType,
	{
		{EMapType::STATICMODEL, "STATICMODEL"},
		{EMapType::LIGHT, "LIGHT"},
		{EMapType::END, "END"},
	}
	)

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

class ENGINE_DLL CExample_LightData final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CExample_LightData() = default;
	virtual ~CExample_LightData() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EMapType::LIGHT); }
	const string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TExample_LightData& Get_Data() const { return m_Data; }
	DTO::TExample_LightData& Get_Data() { return m_Data; }
private:
	DTO::TExample_LightData m_Data;
public:
	static CExample_LightData* Create();
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CExample_StaticModel final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CExample_StaticModel() = default;
	virtual ~CExample_StaticModel() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EMapType::STATICMODEL); }
	const string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	// 읽기 전용 ( const 참조 )
	const DTO::TExample_StaticModelData& Get_Data() const { return m_Data; }
	// 읽기 + 쓰기 전용 ( 참조 )
	DTO::TExample_StaticModelData& Get_Data() { return m_Data; }
private:
	DTO::TExample_StaticModelData m_Data;
public:
	static CExample_StaticModel* Create();
	virtual void Free() override { Super::Free(); }
};

NS_END



class CDataStruct_Map : public ObjectDataBase
{
};

