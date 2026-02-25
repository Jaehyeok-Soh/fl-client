#include "Engine_pch.h"
#include "DataStruct_Map.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")



NS_BEGIN(Engine)


#pragma region Data MapObject
json CData_MapObject::ToJson() const
{
	return json(m_tData);
}

HRESULT CData_MapObject::FromJson(const json& j)
{
	m_tData = j.get<DTO::TMap_MapObjectData>();
	return S_OK;
}

void CData_MapObject::Free()
{
	for (auto& Desc : m_tData.vecClientMakePathDesc)

		Safe_Delete(Desc);

	Super::Free();
}
#pragma endregion

#pragma region 

json CData_LevelData::ToJson() const
{
	return json(m_tData);
}

HRESULT CData_LevelData::FromJson(const json& j)
{
	m_tData = j.get<DTO::TLevelData>();
	return S_OK;
}

void CData_LevelData::Free()
{
	Super::Free();

	return;
}
#pragma endregion

NS_END


NS_BEGIN(DTO)

#pragma region SRT DATA
void from_json(const json& LoadJson, SRT_DATA& tdata)
{
	if (LoadJson.contains("Scale"))
		Engine_Utils::read_vec3_xyz(LoadJson["Scale"], tdata.vScale);

	if (LoadJson.contains("Quaternion"))
		Engine_Utils::read_vec4_Quat(LoadJson["Quaternion"], tdata.vQuat);

	if (LoadJson.contains("Position"))
		Engine_Utils::read_vec3_xyz(LoadJson["Position"], tdata.vPosition);

	if (LoadJson.contains("Scale_Isolate"))
		Engine_Utils::read_vec3_xyz(LoadJson["Scale_Isolate"], tdata.vScale_Isolated);
}
void to_json(json& SaveJson, const SRT_DATA& tData)
{
    Engine_Utils::write_vec3_xyz(SaveJson["Scale"], tData.vScale);
    Engine_Utils::write_vec4_Quat(SaveJson["Quaternion"], tData.vQuat);
    Engine_Utils::write_vec3_xyz(SaveJson["Position"], tData.vPosition);

	Engine_Utils::write_vec3_xyz(SaveJson["Scale_Isolate"], tData.vScale_Isolated);
}
#pragma endregion

#pragma region OVERRIDE_MATERIALS
void from_json(const json& LoadJson, OVERRIDE_MATERIALS& tData)
{
	tData.wstrMtl_JsonFile_Name = Engine_Utils::ToWString(LoadJson.value("Name", ""));
	tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Path", ""));

	/* 이게 Emtpy면 json 파일을 못 읽어온것 */
	//if (tData.vecUsingTextureInfo.empty()) return;

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
void to_json(json& SaveJson, const OVERRIDE_MATERIALS& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Name);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

	if (tData.vecUsingTextureInfo.empty()) return;

	for (auto& pairTextureInfo : tData.vecUsingTextureInfo)
	{
		SaveJson["Textures"].push_back({ Engine_Utils::ToString(pairTextureInfo.first), Engine_Utils::ToString(pairTextureInfo.second) });
	}
}
#pragma endregion

#pragma region USING MODEL INFO

void from_json(const json& LoadJson, USING_MODEL_INFO& tData)
{
	tData.wstrName = Engine_Utils::ToWString(LoadJson.value("Name", ""));
	tData.wstrPath = Engine_Utils::ToWString(LoadJson.value("Path", ""));
	tData.wstrMtl_JsonFile_Path = Engine_Utils::ToWString(LoadJson.value("Material Json File Path",""));

	/* Override Material Override */
	tData.vecOverrideMaterial.clear();
	vector<OVERRIDE_MATERIALS>().swap(tData.vecOverrideMaterial);

	/* 키 값이 있다면 Using Material Info */
	if (LoadJson.contains("Override Materials"))
	{
		auto& MtlJsons = LoadJson["Override Materials"];
		tData.vecOverrideMaterial.resize(MtlJsons.size());
		_uint iIndex{};
		for (auto& MtlJson : MtlJsons)
		{
			/* Null Check */
			/* 배열 순번을 지키기위해 존재하지 않는 값이라도 사이즈에 맞게 할당해서 들고있어야 한다 */
			if (MtlJson.is_null()) // Null Check 후 처리
				tData.vecOverrideMaterial[iIndex].isNull = true;
			else // Null이 아니라면 값 대입
				tData.vecOverrideMaterial[iIndex++] = MtlJson;
		}
	}
}

void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);
	SaveJson["Material Json File Path"] = Engine_Utils::ToString(tData.wstrMtl_JsonFile_Path);

	/* 키값이 없어서 할당받지 못했다면 Size가 0 일것 이때도 마찬가지로 키값에서 빼버린다 */
	if (!tData.vecOverrideMaterial.empty())
	{
		auto& Material_Json = SaveJson["Override Materials"];
		for (auto& Material_Info : tData.vecOverrideMaterial)
		{
			if (Material_Info.isNull)
				Material_Json.push_back(nullptr);
			else
				Material_Json.push_back(Material_Info);
		}
	}
}

#pragma endregion

#pragma region Map_StaticModleData
inline void to_json(json& SaveJson, const DTO::TMap_MapObjectData& tData)
{
	SaveJson = json
	{
		{ "Section Number" , tData.iSectionNum},
		{ "strTag", tData.strTag },
		{ "UE Loaded"  , tData.isUELoaded},
		{ "UE Raw Data Path" , tData.strUERawDataPath},
		{ "Draw Type" , tData.eMapObjectDrawType},
		{ "Client Make Path" , tData.eClientMakePath},
		{ "Client Level Type", tData.eClientLevelType},
		{ "Model Path" , tData.strModelPath},
		{ "SRT" , tData.vecSRTs},
	};

	if (!tData.vecClientMakePathDesc.empty())
	{
		if (IsExist_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(tData.eClientMakePath)))
		{
			auto& DescJson = SaveJson["Client Make Path Desc"];

			for (auto& Desc : tData.vecClientMakePathDesc)
			{
				json BufferJson = json::object();

				if (!Desc)
				{
					DescJson.push_back(nullptr);
					continue;
				}
				Desc->to_Json(BufferJson);
				DescJson.push_back(BufferJson);
			}
		}
	}

}

inline void from_json(const json& LoadJson, DTO::TMap_MapObjectData& tData)
{
	LoadJson.at("strTag").get_to(tData.strTag);

	if (LoadJson.contains("Section Number"))
		tData.iSectionNum = LoadJson["Section Number"].get<_uint>();

	if (LoadJson.contains("UE Loaded"))
		tData.isUELoaded =  LoadJson["UE Loaded"].get<_bool>();

	if (LoadJson.contains("UE Raw Data Path"))
		tData.strUERawDataPath = LoadJson["UE Raw Data Path"].get<string>();

	/* Type 3개  */
	if (LoadJson.contains("Draw Type"))
		LoadJson["Draw Type"].get_to(tData.eMapObjectDrawType);

	if (LoadJson.contains("Client Make Path"))
		LoadJson["Client Make Path"].get_to(tData.eClientMakePath);

	if (LoadJson.contains("Client Level Type"))
		LoadJson["Client Level Type"].get_to(tData.eClientLevelType);

	if (LoadJson.contains("Model Path"))
		tData.strModelPath = LoadJson["Model Path"].get<string>();


	if (LoadJson.contains("SRT"))
		tData.vecSRTs = LoadJson["SRT"].get<vector<SRT_DATA>>();

	if (LoadJson.contains("Client Make Path Desc"))
	{
		for (auto& DescJson : LoadJson["Client Make Path Desc"])
		{
			if (DescJson.is_null()) continue;
			CLIENT_MAKEPATH_DESC_BASE* pDescBase = Engine::Create_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(tData.eClientMakePath),nullptr);
			if (!pDescBase) continue;
			pDescBase->from_Json(DescJson);
			tData.vecClientMakePathDesc.push_back(pDescBase);
		} 
	}
}
#pragma endregion


#pragma region Level Data


inline void to_json(json& SaveJson, const TLevelData& tData)
{
	SaveJson = json
	{
		{ "strTag", tData.strTag },
		{ "Texture Splating Info"	, tData.strTextureSplatingInfoName },
		{ "Level Type"				, tData.strLevelTypeName},
	};

	return;
}
inline void from_json(const json& LoadJson, TLevelData& tData)
{
	if (LoadJson.contains("strTag"))
		tData.strTag = LoadJson["strTag"].get<string>();
	
	if (LoadJson.contains("Texture Splating Info"))
		tData.strTextureSplatingInfoName = LoadJson["Texture Splating Info"].get<string>();
	

	if (LoadJson.contains("Level Type"))
		tData.strLevelTypeName = LoadJson["Level Type"].get<string>();
	
	return;
}

#pragma endregion

NS_END



#pragma region Client Make Path Desc


NS_BEGIN(Engine)


inline CLIENT_MAKEPATH_DESC_BASE* Create_ClientMakePathDesc(DTO::EClientMakePath ePath , CLIENT_MAKEPATH_DESC_BASE* pSource)
{
	switch (ePath)
	{
	case DTO::EClientMakePath::StaticObject: return pSource == nullptr ? new STATICOBJECT_DESC	: new STATICOBJECT_DESC(*static_cast<STATICOBJECT_DESC*>(pSource));
	case DTO::EClientMakePath::LandScape:	 return pSource == nullptr ? new LANDSCAPE_DESC		: new LANDSCAPE_DESC(*static_cast<LANDSCAPE_DESC*>(pSource));


	case DTO::EClientMakePath::TriggerBox_ChangeLevel:	 return pSource == nullptr ? new TRIGGERBOX_CHANGELEVEL_DESC : new TRIGGERBOX_CHANGELEVEL_DESC(*static_cast<TRIGGERBOX_CHANGELEVEL_DESC*>(pSource));




	default:								 return nullptr;
	}

	return nullptr;
}

inline _bool IsExist_ClientMakePathDesc(DTO::EClientMakePath ePath)
{
	CLIENT_MAKEPATH_DESC_BASE* pCheck = Create_ClientMakePathDesc( ePath , nullptr);
	/* Description이 있는지 없는지 체크하는기능 */
	if (pCheck)
	{
		Safe_Delete(pCheck);
		return true;
	}
	return false;
}



void STATICOBJECT_DESC::from_Json(const json& LoadJson)
{
	/* Desc 키값으로 들어온다 */
	this->wstrTest = Engine_Utils::ToWString(LoadJson["Test"].get<string>());
	return;
}

void STATICOBJECT_DESC::to_Json(json& SaveJson)
{
	SaveJson["Test"] = Engine_Utils::ToString(this->wstrTest);

	return;
}

void LANDSCAPE_DESC::from_Json(const json& LoadJson)
{

	if (LoadJson.contains("Index"))
		this->iIndex = LoadJson["Index"].get<_int>();

	if (LoadJson.contains("Texture UV"))
	{
		const auto& TextureUV_Json = LoadJson["Texture UV"];
		if (TextureUV_Json.contains("LT"))
			Engine_Utils::read_vec2_xy( TextureUV_Json["LT"] ,this->vTextureUV_LT);
		if (TextureUV_Json.contains("RB"))
			Engine_Utils::read_vec2_xy(TextureUV_Json["RB"], this->vTextureUV_RB);

	}

	return;
}

void LANDSCAPE_DESC::to_Json(json& SaveJson)
{
	SaveJson["Index"] = this->iIndex;

	auto& TetxtureUV_Json = SaveJson["Texture UV"];

	Engine_Utils::write_vec2_xy(TetxtureUV_Json["LT"],this->vTextureUV_LT);
	Engine_Utils::write_vec2_xy(TetxtureUV_Json["RB"],this->vTextureUV_RB);

	return;
}


#pragma region Trigger Box

void TRIGGERBOX_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Extents"))
	{
		Engine_Utils::read_vec3_xyz(LoadJson["Extents"], this->vExtents);
	}
}

void TRIGGERBOX_DESC::to_Json(json& SaveJson)
{
	Engine_Utils::write_vec3_xyz(SaveJson["Extents"], this->vExtents);
}



#pragma region Change Level

void TRIGGERBOX_CHANGELEVEL_DESC::from_Json(const json& LoadJson)
{
	Super::from_Json(LoadJson);

	if (LoadJson.contains("Change Level Type Name"))
		this->strChangeLevelTypeName = LoadJson["Change Level Type Name"].get<string>();
}

void TRIGGERBOX_CHANGELEVEL_DESC::to_Json(json& SaveJson)
{
	Super::to_Json(SaveJson);


	SaveJson["Change Level Type Name"] = this->strChangeLevelTypeName;
}

#pragma endregion

#pragma endregion


#pragma endregion

NS_END
