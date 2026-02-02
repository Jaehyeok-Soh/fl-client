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

	void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
	{
		SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
		SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);

		auto& Material_Json = SaveJson["Mateiral Info"];

		for (auto& Material_Info : tData.vecMaterialInfo)
		{
			if (Material_Info.isNull == true)
			{
				json NullJson{};
				Material_Json.push_back(NullJson);
			}
			Material_Json.push_back(Material_Info);
		}
	}

	void Tool::to_json(json& SaveJson, const USING_MATERIAL_INFO& tData)
	{

		SaveJson["Name"] = Engine_Utils::ToString(tData.wstrOriginMtl_JsonFile_Name);
		SaveJson["Path"] = Engine_Utils::ToString(tData.wstrOriginMtl_JsonFile_Path);

		if (tData.vecUsingTextureInfo.empty()) return;

		for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
		{
			SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
		}
	}

	void Tool::to_json(json& SaveJson, const STATICMODEL_DATA& tData)
	{

		Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Scale"], tData.tOriginSRT.vScale);
		Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Position"], tData.tOriginSRT.vPosition);
		Engine_Utils::write_vec4_Quat(SaveJson["SRT"]["Rotation"], tData.tOriginSRT.vQuat);

		SaveJson["Model Info"] = tData.tUsingModelInfo;
	}
}