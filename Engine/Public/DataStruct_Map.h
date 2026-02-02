#pragma once

#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "Engine_Utils.h"

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
	INSTANCEMODEL,
	END
};

inline constexpr _uint g_MapObjectTypeCount{ ENUM_TO_UINT(EMapObjectType::END) };

/* String으로 자동 변환되어서 저당된다  */

NLOHMANN_JSON_SERIALIZE_ENUM(EMapObjectType,
	{
		{EMapObjectType::STATICMODEL, "STATICMODEL"},
		{EMapObjectType::INSTANCEMODEL, "INSTANCEMODEL"},
		{EMapObjectType::END, "END"},
	}
	)

	/////////////////-------------------  ObjectStruct  -------------------/////////////////

	typedef struct tagSRT_Data
{
	Vec3	vScale{0.f,0.f,0.f};
	Quat	vQuat{0.f,0.f,0.f,1.f};
	Vec3	vPosition{ 0.f,0.f,0.f };
}SRT_DATA;


typedef struct tagUsingMaterialInfo
{
	bool	isNull{ true };

	/* 참조하고 있는 Origin Mrt Material Json 파일 Path 값 */
	wstring wstrOriginMtl_JsonFile_Name{};
	wstring wstrOriginMtl_JsonFile_Path{};
	/* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
	vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
}USING_MATERIAL_INFO;


typedef struct tagUsingModelInfo
{
	wstring wstrName{};
	wstring wstrPath{};

	/* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
	wstring wstrMtl_JsonFile_Path{};

	vector<USING_MATERIAL_INFO> vecMaterialInfo{};
public:

}USING_MODEL_INFO;

struct TMap_InstanceModelData
{
	static constexpr EMapObjectType eType = EMapObjectType::STATICMODEL;

	USING_MODEL_INFO tUsingInfo{};
	vector<SRT_DATA> vecSRTData{};
};

struct TMap_StaticModelData
{
	std::string strTag{ "Map" };
	static constexpr EMapObjectType eType = EMapObjectType::INSTANCEMODEL;

	/* Model Info */
	USING_MODEL_INFO tUsingModelInfo{};
	SRT_DATA		 tSRTData{};
};


/////////////////-------------------  to_json, from_json  -------------------/////////////////


#pragma region Transform Data

inline void to_json(json& j, const SRT_DATA& tdata)
{
	Engine_Utils::write_vec3_xyz(j["Scale"], tdata.vScale);
	Engine_Utils::write_vec4_Quat(j["Quaternion"], tdata.vQuat);
	Engine_Utils::write_vec3_xyz(j["Position"], tdata.vPosition);
}

inline void from_json(const json& j, SRT_DATA& tdata)
{
	if(j.contains("Scale"))
		Engine_Utils::read_vec3_xyz(j["Scale"],tdata.vScale);

	if (j.contains("PitchYawRoll"))
		Engine_Utils::read_vec4_Quat(j["Quaternion"], tdata.vQuat);

	if (j.contains("Position"))
		Engine_Utils::read_vec3_xyz(j["Position"], tdata.vScale);
}
#pragma endregion 



#pragma region Material Info

void from_json(const json& SaveJson, USING_MATERIAL_INFO& tData)
{
	if (tData.isNull == true) return;

	tData.wstrOriginMtl_JsonFile_Name = Engine_Utils::ToWString(SaveJson.value("Name", ""));
	tData.wstrOriginMtl_JsonFile_Path = Engine_Utils::ToWString(SaveJson.value("Path", ""));

	if (tData.vecUsingTextureInfo.empty()) return;

	//for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
	//{
	//	SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
	//}
}

void to_json(json& SaveJson, const USING_MATERIAL_INFO& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrOriginMtl_JsonFile_Name);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrOriginMtl_JsonFile_Path);

	if (tData.vecUsingTextureInfo.empty()) return;

	for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
	{
		SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
	}
}

#pragma endregion


#pragma region Using Model Info

void from_json(const json& LoadJson, USING_MODEL_INFO& tData)
{
	tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Name", ""));
	tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Path", ""));

	if (LoadJson.contains("Mateiral Info"))
	{

		auto& MtlJsons = LoadJson["Mateiral Info"];
		tData.vecMaterialInfo.resize(MtlJsons.size());

		_uint iIndex{};
		for (auto& MtlJson : MtlJsons)
		{
			if (MtlJson.empty())
				tData.vecMaterialInfo[iIndex].isNull = true;
			else
				tData.vecMaterialInfo[iIndex++] = MtlJson;
		}
	}
}

void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);

	auto& Material_Json = SaveJson["Mateiral Info"];

	for (auto& Material_Info : tData.vecMaterialInfo)
	{
		json Nulljson{};
		if (!Material_Info.isNull)
			Nulljson = Material_Info;
		SaveJson.push_back(Nulljson);
	}
}

#pragma endregion

inline void to_json(json& j, const TMap_StaticModelData& data)
{
	j["SRT"] = data.tSRTData;

}

inline void from_json(const json& j, TMap_StaticModelData& data)
{
	j.at("strTag").get_to(data.strTag);

	if (j.contains("SRT"))
		data.tSRTData =  j["SRT"];
	if (j.contains("Model Info"))
		data.tUsingModelInfo = j["Model Info"];
}


inline void to_json(json& j, const TMap_InstanceModelData& data)
{

}
inline void from_json(const json& j, TMap_InstanceModelData& data)
{

}

NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)



class CData_StaticModel final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
private:
	CData_StaticModel() = default;
	virtual ~CData_StaticModel() = default;
public:

public:
	_uint Get_Type() const override;
	const string& Get_Tag() const override;
	json ToJson() const override;
	HRESULT FromJson(const json& j) override;
};


NS_END




