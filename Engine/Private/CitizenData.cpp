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

	if (LoadJson.contains("Parts Datas"))
	{
		auto& PartDatas_LoadJson = LoadJson["Parts Datas"];
		
		for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
		{
			string strPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));
			if (PartDatas_LoadJson.contains(strPartName))
			{
				this->arrayPartDatas[i].from_Json(PartDatas_LoadJson[strPartName]);
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

	auto& Parts_SaveJson = SaveJson["Parts Datas"];
	for (_uint i = 0; i < ENUM_TO_UINT(CITIZEN_PARTTYPE::END); ++i)
	{
		string strPartName = CitizenPartType_ToString(CITIZEN_PARTTYPE(i));
		auto& Part_SaveJson = Parts_SaveJson[strPartName];
		this->arrayPartDatas[i].to_Json(Part_SaveJson);
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

	if (LoadJson.contains("Select Column"))
		this->iSelectColumn = LoadJson["Select Column"];
}

void CITIZEN_ATLAS_DATA::to_Json(json& SaveJson)
{
	SaveJson["Use Atlas"] = this->isUseAtlas;
	SaveJson["Max Colum"] = this->iMaxColumn;
	SaveJson["Max Row"] = this->iMaxRow;
	SaveJson["Select Row"] = this->iSelectRow;
	SaveJson["Select Column"] = this->iSelectColumn;
}
#pragma endregion


#pragma region PartData

void CITIZEN_PART_DATA::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Color"))
	{
		Engine_Utils::read_vec4_xyzw(LoadJson["Color"], this->vColor);
	}

	if (LoadJson.contains("Name"))
	{
		this->strName = LoadJson["Name"];
	}
}

void CITIZEN_PART_DATA::to_Json(json& SaveJson)
{
	Engine_Utils::write_vec4_xyzw(SaveJson["Color"],this->vColor);
	SaveJson["Name"] = this->strName;
}

#pragma endregion


void CB_CitizentFaceData::SetFaceUV(CITIZEN_ATLAS_TYPE eType, const struct CITIZEN_ATLAS_DATA* pData)
{

	if (!pData || !pData->isUseAtlas) {
		tCitizenFaceUV[ENUM_TO_UINT(eType)] = { {0.f, 0.f}, {1.f, 1.f} };
		return;
	}

	float fScaleX = 1.0f / (float)pData->iMaxColumn;
	float fScaleY = 1.0f / (float)pData->iMaxRow;

	auto& target = tCitizenFaceUV[ENUM_TO_UINT(eType)];
	target.vUVScale = { fScaleX, fScaleY };
	target.vUVOffset = { fScaleX * pData->iSelectColumn , fScaleY * pData->iSelectRow};
}

NS_END

