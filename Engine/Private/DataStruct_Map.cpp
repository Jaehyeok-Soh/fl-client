#include "Engine_pch.h"
#include "DataStruct_Map.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")



NS_BEGIN(Engine)

json CData_StaticModel::ToJson() const
{
    return json(m_tData);
}

HRESULT CData_StaticModel::FromJson(const json& j)
{
    m_tData = j.get<DTO::STATICMODEL_DATA>();
    return S_OK;
}
NS_END


NS_BEGIN(DTO)

void from_json(const json& LoadJson, SRT_DATA& tdata)
{
	if (LoadJson.contains("Scale"))
		Engine_Utils::read_vec3_xyz(LoadJson["Scale"], tdata.vScale);

	if (LoadJson.contains("Quaternion"))
		Engine_Utils::read_vec4_Quat(LoadJson["Quaternion"], tdata.vQuat);

	if (LoadJson.contains("Position"))
		Engine_Utils::read_vec3_xyz(LoadJson["Position"], tdata.vPosition);
}
void to_json(json& SaveJson, const SRT_DATA& tData)
{
    Engine_Utils::write_vec3_xyz(SaveJson["Scale"], tData.vScale);
    Engine_Utils::write_vec4_Quat(SaveJson["Quaternion"], tData.vQuat);
    Engine_Utils::write_vec3_xyz(SaveJson["Position"], tData.vPosition);
}
void from_json(const json& LoadJson, USING_MATERIAL_INFO& tData)
{
	if (tData.isNull == true) return;

	tData.wstrOriginMtl_JsonFile_Name = Engine_Utils::ToWString(LoadJson.value("Name", ""));
	tData.wstrOriginMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Path", ""));

	if (tData.vecUsingTextureInfo.empty()) return;

	if (LoadJson.contains("Textures"))
	{
		auto& TexJsons = LoadJson["Textures"];
		for (auto& TexJson : TexJsons)
		{
			if (TexJson.empty()) continue;
			std::pair<string, string> strPair = TexJson;
			tData.vecUsingTextureInfo.push_back({ Engine_Utils::ToWString(strPair.first) , Engine_Utils::ToWString(strPair.second) });
		}
	}
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

void from_json(const json& LoadJson, USING_MODEL_INFO& tData)
{
	tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Name", ""));
	tData.wstrPath = Engine_Utils::ToWString(LoadJson.value("Path", ""));
	tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Meterial Json File Path",""));


	if (LoadJson.contains("Using Mateiral Info"))
	{

		auto& MtlJsons = LoadJson["Using Mateiral Info"];
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
	SaveJson["Meterial Json File Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

	auto& Material_Json = SaveJson["Using Mateiral Info"];

	for (auto& Material_Info : tData.vecMaterialInfo)
	{
		json Nulljson{};
		if (!Material_Info.isNull)
			Nulljson = Material_Info;
		Material_Json.push_back(Nulljson);
	}
}

void to_json(json& SaveJson, const TMap_StaticModelData& tData)
{
	SaveJson = json
	{
		{ "Type", TMap_StaticModelData::eType },
		{ "strTag", tData.strTag },
		{ "SRT" , tData.tSRTData},
		{ "Using Model Info", tData.tUsingModelInfo}
	};
}
void from_json(const json& LoadJson, TMap_StaticModelData& data)
{
	LoadJson.at("strTag").get_to(data.strTag);
	if (LoadJson.contains("SRT"))
		data.tSRTData = LoadJson["SRT"];
	if (LoadJson.contains("Using Model Info"))
		data.tUsingModelInfo = LoadJson["Using Model Info"];
}

void to_json(json& j, const TMap_InstanceModelData& data)
{
}
void from_json(const json& j, TMap_InstanceModelData& data)
{
}

NS_END
