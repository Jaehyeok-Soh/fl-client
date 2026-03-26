#include "Engine_pch.h"
#include "CitizenData.h"



NS_BEGIN(DTO)


#pragma region NPC Data
void CITIZEN_DATA::from_Json(const json& LoadJson)
{
	// "Atlas Datas" 키가 존재하고, 그 값이 배열인지 확인
	if (LoadJson.contains("Atlas Datas") && LoadJson["Atlas Datas"].is_array())
	{
		const auto& AltasDataArray_Json = LoadJson["Atlas Datas"];

		// 실제 가지고 있는 array 크기와 Json 배열 크기 중 작은 값을 기준으로 루프 
		// (혹시 Json에 데이터가 더 많아도 배열 범위 초과 에러 방지)
		size_t loopCount = std::min(arrayNpcAtlasData.size(), AltasDataArray_Json.size());

		for (size_t i = 0; i < loopCount; ++i)
		{
			// 각 인덱스에 맞는 구조체의 from_Json 호출
			arrayNpcAtlasData[i].from_Json(AltasDataArray_Json[i]);
		}
	}
	if (LoadJson.contains("Cloth RGBA Color"))
	{
		auto& RGBColor_LoadJson = LoadJson["Cloth RGBA Color"];

		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["R"], this->tClothRGBColor.vColorR);
		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["G"], this->tClothRGBColor.vColorG);
		Engine_Utils::read_vec4_xyzw(RGBColor_LoadJson["B"], this->tClothRGBColor.vColorB);

		this->isUseClothColorMapping = true;
	}
	else
		this->isUseClothColorMapping = false;

	if (LoadJson.contains("Parts Names"))
	{
		auto& PartName_LoadJson = LoadJson["Parts Names"];
		for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
		{
			string strCitizenPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));
			if (PartName_LoadJson.contains(strCitizenPartName))
			{
				this->arrayPartDatas[i].strName = PartName_LoadJson[strCitizenPartName]["Name"];
				Engine_Utils::read_vec4_xyzw(PartName_LoadJson[strCitizenPartName]["Color"], this->arrayPartDatas[i].vColor);
			}
		}
	}
	if (LoadJson.contains("Loop Animation Name"))
	{
		this->strLoopAnimationName = LoadJson["Loop Animation Name"];
	}
	if (LoadJson.contains("Model Name"))
	{
		this->strModelName = LoadJson["Model Name"];
	}
}

void CITIZEN_DATA::to_Json(json& SaveJson)
{

	auto& AltasDataArray_Json = SaveJson["Atlas Datas"];
	for (auto& AtlasData : arrayNpcAtlasData)
	{
		json AtlasData_json = json::object();
		AtlasData.to_Json(AtlasData_json);
		AltasDataArray_Json.push_back(AtlasData_json);
	}

	SaveJson["Model Name"] = this->strModelName;

	if (this->isUseClothColorMapping)
	{
		auto& RGBColor_SaveJson = SaveJson["Cloth RGBA Color"];

		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["R"], this->tClothRGBColor.vColorR);
		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["G"], this->tClothRGBColor.vColorG);
		Engine_Utils::write_vec4_xyzw(RGBColor_SaveJson["B"], this->tClothRGBColor.vColorB);

	}

	auto& Part_SaveJson = SaveJson["Parts Names"];
	for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
	{
		string strPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));

		auto& PartName_SaveJson = Part_SaveJson[strPartName];
		Engine_Utils::write_vec4_xyzw(PartName_SaveJson["Color"], this->arrayPartDatas[i].vColor);
		PartName_SaveJson["Name"] = this->arrayPartDatas[i].strName;
	}

	SaveJson["Loop Animation Name"] = this->strLoopAnimationName;
}
#pragma endregion


#pragma region NPC 아틸라스 정보

void CITIZEN_ATLAS_DATA::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Use Atlas"))
		this->isUseAtlas = LoadJson["Use Atlas"];

	if (LoadJson.contains("Max Colum"))
		this->iMaxColumn = LoadJson["Max Colum"];

	if (LoadJson.contains("Max Row"))
		this->iMaxRow = LoadJson["Max Row"];

	if (LoadJson.contains("Select Row"))
		this->iSelectRow = LoadJson["Select Row"];
}

void CITIZEN_ATLAS_DATA::to_Json(json& SaveJson)
{
	SaveJson["Use Atlas"] = this->isUseAtlas;
	SaveJson["Max Colum"] = this->iMaxColumn;
	SaveJson["Max Row"] = this->iMaxRow;
	SaveJson["Select Row"] = this->iSelectRow;
}
#pragma endregion


#pragma region PartData

void CITIZEN_PART_DATA::from_Json(const json& LoadJson)
{
}

void CITIZEN_PART_DATA::to_Json(json& SaveJson)
{
}

#pragma endregion

NS_END

