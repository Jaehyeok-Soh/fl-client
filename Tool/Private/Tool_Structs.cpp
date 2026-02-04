#include "pch.h"
#include "Tool_Structs.h"
#include "Engine_Utils.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
#include <wincrypt.h>
using json = nlohmann::json;
#pragma pop_macro("new")


namespace Tool
{

#pragma region SRT Data
	void from_json(const json& LoadJson, SRT_DATA& tData)
	{
		if (LoadJson.contains("Scale"))
			Engine_Utils::read_vec3_xyz(LoadJson["Scale"], tData.vScale);

		if (LoadJson.contains("Quaternion"))
			Engine_Utils::read_vec4_Quat(LoadJson["Quaternion"], tData.vQuat);

		if (LoadJson.contains("Position"))
			Engine_Utils::read_vec3_xyz(LoadJson["Position"], tData.vPosition);
	}
	void to_json(json& SaveJson, const SRT_DATA& tData)
	{
		Engine_Utils::write_vec3_xyz(SaveJson["Scale"], tData.vScale);
		Engine_Utils::write_vec4_Quat(SaveJson["Quaternion"], tData.vQuat);
		Engine_Utils::write_vec3_xyz(SaveJson["Position"], tData.vPosition);
	}
#pragma endregion
#pragma region Using Model 
	void from_json(const json& LoadJson, USING_MODEL_INFO& tData)
	{
		tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Name", ""));
		tData.wstrPath = Engine_Utils::ToWString(LoadJson.value("Path", ""));
		tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Meterial Json File Path", ""));


		if (LoadJson.contains("Override Materials"))
		{
			auto& MtlJsons = LoadJson["Override Materials"];
			tData.vecOverrideMaterial.resize(MtlJsons.size());

			_uint iIndex{};
			for (auto& MtlJson : MtlJsons)
			{
				if (MtlJson.empty())
					tData.vecOverrideMaterial[iIndex].isNull = true;
				else
					tData.vecOverrideMaterial[iIndex] = MtlJson;
				iIndex++;
			}
		}
	}
	void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
	{
		SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
		SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);
		SaveJson["Meterial Json File Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

		if (!tData.vecOverrideMaterial.empty())
		{
			auto& Material_Json = SaveJson["Override Materials"];

			for (auto& Material_Info : tData.vecOverrideMaterial)
			{
				json NullJson{};
				if (!Material_Info.isNull)
					NullJson = Material_Info;
				Material_Json.push_back(NullJson);
			}
		}
	}

#pragma endregion
#pragma region Using Material
	void from_json(const json& SaveJson, OVERRIDE_MATERIALS& tData)
	{
		if (tData.isNull == true) return;

		tData.wstrMtl_JsonFile_Name = Engine_Utils::ToWString(SaveJson.value("Name", ""));
		tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(SaveJson.value("Path", ""));


		if (SaveJson.contains("Textures"))
		{
			auto& TexJsons = SaveJson["Textures"];
			for (auto& TexJson : TexJsons)
			{
				if (TexJson.empty()) continue;
				std::pair<string, string> strPair = TexJson;
				tData.vecUsingTextureInfo.push_back({ Engine_Utils::ToWString(strPair.first) , Engine_Utils::ToWString(strPair.second) });
			}
		}
	}

	void Tool::to_json(json& SaveJson, const OVERRIDE_MATERIALS& tData)
	{

		SaveJson["Name"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Name);
		SaveJson["Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

		for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
		{
			SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
		}
	}
#pragma endregion

#pragma region Static Model
	void from_json(const json& LoadJson, STATICMODEL_DATA& tData)
	{
		if (LoadJson.contains("SRT"))
			tData.tOriginSRT = LoadJson["SRT"];
		if (LoadJson.contains("Using Model Info"))
			tData.tUsingModelInfo = LoadJson["Using Model Info"];
	}

	void Tool::to_json(json& SaveJson, const STATICMODEL_DATA& tData)
	{
		
		Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Scale"], tData.tOriginSRT.vScale);
		Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Position"], tData.tOriginSRT.vPosition);
		Engine_Utils::write_vec4_Quat(SaveJson["SRT"]["Rotation"], tData.tOriginSRT.vQuat);

		SaveJson["Using Model Info"] = tData.tUsingModelInfo;
	}
#pragma endregion

#pragma region InstanceModel 
	void from_json(const json& LoadJson, INSTANCEMODEL_DATA& tData)
	{
		if (LoadJson.contains("SRTs"))
			tData.vecOriginSRT = LoadJson["SRTs"];
		if (LoadJson.contains("Using Model Info"))
			tData.tUsingModelInfo = LoadJson["Using Model Info"];
		if (LoadJson.contains("Usage"))
			tData.eInstance_Usage = Engine_Utils::D3D11_USAGE_ToEnum(LoadJson["Usage"].get<string>());
	}

	void to_json(json& SaveJson, const INSTANCEMODEL_DATA& tData)
	{
		SaveJson["Using Model Info"] = tData.tUsingModelInfo;
		if (!tData.vecOriginSRT.empty())
			SaveJson["SRTs"] = tData.vecOriginSRT;

		SaveJson["Usage"] = Engine_Utils::D3D11_USAGE_ToString(tData.eInstance_Usage);
	}
#pragma endregion

#pragma endregion

}