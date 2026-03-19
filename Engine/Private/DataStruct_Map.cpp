#include "Engine_pch.h"
#include "DataStruct_Map.h"
#include "Model.h"
#include "Transform.h"
#include "Collider.h"
#include "Bounding_Sphere.h"
#include "Bounding_Obb.h"
#include "TextureBase.h"
#include "Light.h"

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")



#include "GameInstance.h"

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

	/* 생성해야하는지 안하는지 */
	if (IsExist_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(tData.eClientMakePath)))
	{
		if (!tData.vecClientMakePathDesc.empty())
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
		else
		{
			MSG_BOX(" Desc이 존재해야하는데 없는 거같다 조심해라 뭐가 이상하다 ");
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



	if (IsExist_ClientMakePathDesc(tData.eClientMakePath))
	{
		tData.vecClientMakePathDesc.clear();
		size_t srtCount = tData.vecSRTs.size();

		for (size_t i = 0; i < srtCount; ++i)
		{
			CLIENT_MAKEPATH_DESC_BASE* pDescBase = Engine::Create_ClientMakePathDesc(static_cast<DTO::EClientMakePath>(tData.eClientMakePath), nullptr);
			if (LoadJson.contains("Client Make Path Desc") && i < LoadJson["Client Make Path Desc"].size())
			{
				const auto& DescJson = LoadJson["Client Make Path Desc"][i];
				if (!DescJson.is_null())
				{
					pDescBase->from_Json(DescJson);
				}
			}
			tData.vecClientMakePathDesc.push_back(pDescBase);
		}
	}

}
#pragma endregion


#pragma region Level Data


inline void to_json(json& SaveJson, const TLevelData& tData)
{
	SaveJson["strTag"] = tData.strTag;
	SaveJson["Texture Splating Info"] = tData.strTextureSplatingInfoName;
	SaveJson["Level Type"] = tData.strLevelTypeName;

	auto& WindJson = SaveJson["Env Data"]["Wind Data"];
	Engine_Utils::write_vec3_xyz(WindJson["Direction"], tData.vWindDirection);
	WindJson["Power"] = tData.fWindPower;

	auto& MapBox_Json = SaveJson["Map Box"];
	Engine_Utils::write_vec3_xyz(MapBox_Json["Center"],tData.vMapMinMaxBox_Center);
	Engine_Utils::write_vec3_xyz(MapBox_Json["Extents"],tData.vMapMinMaxBox_extents);

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
	
	if (LoadJson.contains("Env Data"))
	{
		auto& EnvLoadJson = LoadJson["Env Data"];
		if (EnvLoadJson.contains("Wind Data"))
		{
			auto& WindLoaddJson = EnvLoadJson["Wind Data"];
			
			if (WindLoaddJson.contains("Direction"))
			{
				Engine_Utils::read_vec3_xyz(WindLoaddJson["Direction"],tData.vWindDirection);
			}
			if (WindLoaddJson.contains("Power"))
			{
				tData.fWindPower = WindLoaddJson["Power"];
			}
		}
	}

	if (LoadJson.contains("Map Box"))
	{
		auto& MapBox_Json = LoadJson.at("Map Box");
		if (MapBox_Json.contains("Center"))
		{
			Engine_Utils::read_vec3_xyz(MapBox_Json["Center"],tData.vMapMinMaxBox_Center);
		}
		if (MapBox_Json.contains("Extents"))
		{
			Engine_Utils::read_vec3_xyz(MapBox_Json["Extents"],tData.vMapMinMaxBox_extents);
		}
	}


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
	case DTO::EClientMakePath::LandScape:							return pSource == nullptr ? new LANDSCAPE_DESC		: new LANDSCAPE_DESC(*static_cast<LANDSCAPE_DESC*>(pSource));

	case DTO::EClientMakePath::Tree:								return pSource == nullptr ? new TREE_DESC			: new TREE_DESC(*static_cast<TREE_DESC*>(pSource));
	case DTO::EClientMakePath::Grass:								return pSource == nullptr ? new GRASS_DESC			: new GRASS_DESC(*static_cast<GRASS_DESC*>(pSource));
	case DTO::EClientMakePath::Vine:								return pSource == nullptr ? new VINE_DESC			: new VINE_DESC(*static_cast<VINE_DESC*>(pSource));
	case DTO::EClientMakePath::Moss:								return pSource == nullptr ? new MOSS_DESC			: new MOSS_DESC(*static_cast<MOSS_DESC*>(pSource));
	case DTO::EClientMakePath::Bush:								return pSource == nullptr ? new BUSH_DESC			: new BUSH_DESC(*static_cast<BUSH_DESC*>(pSource));

	case DTO::EClientMakePath::Water:								return pSource == nullptr ? new WATER_DESC			: new WATER_DESC(*static_cast<WATER_DESC*>(pSource));
	case DTO::EClientMakePath::Env:									return pSource == nullptr ? new ENV_DESC			: new ENV_DESC(*static_cast<ENV_DESC*>(pSource));
		/* Batch Object 관련 */
	case DTO::EClientMakePath::Batch_Monster:						return pSource == nullptr ? new BATCH_MONSTER_DESC	: new BATCH_MONSTER_DESC(*static_cast<BATCH_MONSTER_DESC*>(pSource));
	case DTO::EClientMakePath::Batch_Object:						return pSource == nullptr ? new BATCH_OBJECT_DESC	: new BATCH_OBJECT_DESC(*static_cast<BATCH_OBJECT_DESC*>(pSource));
	case DTO::EClientMakePath::Batch_NPC:							return pSource == nullptr ? new BATCH_NPC_DESC		: new BATCH_NPC_DESC(*static_cast<BATCH_NPC_DESC*>(pSource));
		/* Trigger Box 관련 */
	case DTO::EClientMakePath::TriggerBox_ChangeLevel:				return pSource == nullptr ? new TRIGGERBOX_CHANGELEVEL_DESC				: new TRIGGERBOX_CHANGELEVEL_DESC(*static_cast<TRIGGERBOX_CHANGELEVEL_DESC*>(pSource));
	case DTO::EClientMakePath::TriggerBox_MonsterSpawner:			return pSource == nullptr ? new TRIGGERBOX_MONSTERSPAWNER_DESC			: new TRIGGERBOX_MONSTERSPAWNER_DESC(*static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(pSource));
	case DTO::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:	return pSource == nullptr ? new TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC : new TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC(*static_cast<TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC*>(pSource));
	case DTO::EClientMakePath::TriggerBox_TutorialUIEvent:			return pSource == nullptr ? new TRIGGERBOX_TUTORIALUIEVENT_DESC			: new TRIGGERBOX_TUTORIALUIEVENT_DESC(*static_cast<TRIGGERBOX_TUTORIALUIEVENT_DESC*>(pSource));
	case DTO::EClientMakePath::TriggerBox_CinematicPlayer:			return pSource == nullptr ? new TRIGGERBOX_CINEMATICPLAYER_DESC			: new TRIGGERBOX_CINEMATICPLAYER_DESC(*static_cast<TRIGGERBOX_CINEMATICPLAYER_DESC*>(pSource));




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


#pragma region Static Object


void STATICOBJECT_DESC::from_Json(const json& LoadJson)
{
	/* Desc 키값으로 들어온다 */
	if (LoadJson.contains("Test"))
		this->wstrTest = Engine_Utils::ToWString(LoadJson["Test"].get<string>());

	return;
}

void STATICOBJECT_DESC::to_Json(json& SaveJson)
{
	SaveJson["Test"] = Engine_Utils::ToString(this->wstrTest);

	return;
}

#pragma endregion

#pragma region LandScape

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

#pragma endregion

#pragma region	Plants

void PLANTS_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Plants"))
	{
		const auto& Plants_LoadJson = LoadJson["Plants"];
		if (Plants_LoadJson.contains("Color"))
		{
			Engine_Utils::read_vec4_xyzw(Plants_LoadJson["Color"],this->vMITint_Color);
		}
	}

	if (LoadJson.contains("Diffuse Color Power"))
	{
		this->fDiffuseColorPower = LoadJson["Diffuse Color Power"];
	}
}

void PLANTS_DESC::to_Json(json& SaveJson)
{
	auto& Plants_SaveJson = SaveJson["Plants"];

	Engine_Utils::write_vec4_xyzw(Plants_SaveJson["Color"],this->vMITint_Color);

	SaveJson["Diffuse Color Power"] = this->fDiffuseColorPower;
}

#pragma region Tree

void TREE_DESC::from_Json(const json& LoadJson)
{
	this->PLANTS_DESC::from_Json(LoadJson);

	/* 그다음 나무 Desc Load */
}

void TREE_DESC::to_Json(json& SaveJson)
{
	this->PLANTS_DESC::to_Json(SaveJson);

	/* 그다움 나무 Desc Save */

}

#pragma endregion


#pragma region Moss

void MOSS_DESC::from_Json(const json& LoadJson)
{
	this->PLANTS_DESC::from_Json(LoadJson);

	/* 그다음 나무 Desc Load */
}

void MOSS_DESC::to_Json(json& SaveJson)
{
	this->PLANTS_DESC::to_Json(SaveJson);

	/* 그다움 나무 Desc Save */

}

#pragma endregion


#pragma region Grass

void GRASS_DESC::from_Json(const json& LoadJson)
{
	this->PLANTS_DESC::from_Json(LoadJson);

	if (LoadJson.contains("Grass Sway Speed"))
	{
		this->fGrassSwaySpeed = LoadJson["Grass Sway Speed"];
	}
	if (LoadJson.contains("Grass Wave Size"))
	{
		this->fGrassWaveSize = LoadJson["Grass Wave Size"];
	}


	/* 그다음 나무 Desc Load */
}

void GRASS_DESC::to_Json(json& SaveJson)
{
	this->PLANTS_DESC::to_Json(SaveJson);

	/* 그다움 나무 Desc Save */
	SaveJson["Grass Sway Speed"]	= this->fGrassSwaySpeed;
	SaveJson["Grass Wave Size"]		= this->fGrassWaveSize;

}

#pragma endregion


#pragma region Vine

void VINE_DESC::from_Json(const json& LoadJson)
{
	this->PLANTS_DESC::from_Json(LoadJson);

	/* 그다음 나무 Desc Load */
}

void VINE_DESC::to_Json(json& SaveJson)
{
	this->PLANTS_DESC::to_Json(SaveJson);

	/* 그다움 나무 Desc Save */

}

#pragma endregion


#pragma region Bush

void BUSH_DESC::from_Json(const json& LoadJson)
{
	this->PLANTS_DESC::from_Json(LoadJson);

	/* 그다음 나무 Desc Load */
}

void BUSH_DESC::to_Json(json& SaveJson)
{
	this->PLANTS_DESC::to_Json(SaveJson);

	/* 그다움 나무 Desc Save */

}

#pragma endregion


#pragma endregion

#pragma region Env

#pragma region ENV Desc

void ENV_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Effect Infos"))
	{
		this->vecEnvEffectInfo.clear(); // 기존 데이터 초기화
		auto& EffectInfos = LoadJson["Effect Infos"];

		for (auto& InfoJson : EffectInfos)
		{
			if (InfoJson.is_null()) continue;

			ENV_EFFECT_INFO tInfo{};

			// 1. 태그 읽기
			if (InfoJson.contains("Tag"))
				tInfo.strTags = InfoJson["Tag"].get<string>();

			// 2. Desc (Transform 데이터) 읽기
			if (InfoJson.contains("Desc"))
			{
				auto& DescJson = InfoJson["Desc"];
				Engine_Utils::read_vec3_xyz(DescJson["Pos"]  , tInfo.tDesc.VFX_Target_Position);
				Engine_Utils::read_vec3_xyz(DescJson["Rot"]  , tInfo.tDesc.VFX_Rotation);
				Engine_Utils::read_vec3_xyz(DescJson["Scale"], tInfo.tDesc.VFX_Scale);
			}

			this->vecEnvEffectInfo.push_back(tInfo);
		}
	}
}

void ENV_DESC::to_Json(json& SaveJson)
{
	json EffectInfos_SaveJson = json::array(); // 배열 형태로 생성

	for (auto& tInfo : vecEnvEffectInfo)
	{
		json InfoObj;
		InfoObj["Tag"] = tInfo.strTags;

		// Desc 데이터를 JSON 객체로 변환
		json DescObj;
		Engine_Utils::write_vec3_xyz(DescObj["Pos"],tInfo.tDesc.VFX_Target_Position);
		Engine_Utils::write_vec3_xyz(DescObj["Rot"],tInfo.tDesc.VFX_Rotation);
		Engine_Utils::write_vec3_xyz(DescObj["Scale"],tInfo.tDesc.VFX_Scale);

		InfoObj["Desc"] = DescObj;
		EffectInfos_SaveJson.push_back(InfoObj);
	}

	SaveJson["Effect Infos"] = EffectInfos_SaveJson;
}
#pragma endregion



#pragma region Water


WATER_DESC::WATER_DESC(const WATER_DESC& rhs)
	: CLIENT_MAKEPATH_DESC_BASE(rhs), arrayTextureBase{ rhs.arrayTextureBase }, vMI_TintColor{rhs.vMI_TintColor}
	, vSpeed1{ rhs.vSpeed1 }, vSpeed2{rhs.vSpeed2}
	, vDistortionSpeed{rhs.vDistortionSpeed }
	, fDistortionPower{rhs.fDistortionPower }
	, vWaterUVPower{rhs.vWaterUVPower }
	, vDistortionUVPower{rhs.vDistortionUVPower }
	, vSparkleUVPower{rhs.vSparkleUVPower}
	, fSparklePower{rhs.fSparklePower}
{
	for (auto& Tex : arrayTextureBase)
		Safe_AddRef(Tex);
}

WATER_DESC::~WATER_DESC()
{
	for (auto& TexBase : this->arrayTextureBase)
		Safe_Release(TexBase);
}


void WATER_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Color"))
	{
		Engine_Utils::read_vec4_xyzw(LoadJson["Color"],this->vMI_TintColor);
	}
	if (LoadJson.contains("Speed 1"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["Speed 1"], this->vSpeed1);
	}
	if (LoadJson.contains("Speed 2"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["Speed 2"], this->vSpeed2);
	}

	if (LoadJson.contains("UV Power"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["UV Power"], this->vWaterUVPower);
	}

	if (LoadJson.contains("Distortion UV Power"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["Distortion UV Power"], this->vDistortionUVPower);
	}

	if (LoadJson.contains("Distortion Speed"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["Distortion Speed"], this->vDistortionSpeed);
	}

	if (LoadJson.contains("Distortion Power"))
	{
		this->fDistortionPower = LoadJson["Distortion Power"];
	}

	if (LoadJson.contains("Sparkle UV Power"))
	{
		Engine_Utils::read_vec2_xy(LoadJson["Sparkle UV Power"], this->vSparkleUVPower);
	}

	if (LoadJson.contains("Sparkle Power"))
	{
		this->fSparklePower = LoadJson["Sparkle Power"];
	}


	if (LoadJson.contains("Texture Names"))
	{
		const json& TextureNames_LoadJson = LoadJson["Texture Names"];

		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END); ++i)
		{
			string strTextureKeyName = WaterTextureType_ToString(static_cast<EWaterTextureType>(i));
			if (TextureNames_LoadJson.contains(strTextureKeyName))
			{
				wstring wstrTextureName = Engine_Utils::ToWString(TextureNames_LoadJson[strTextureKeyName].get<string>());
				if (wstrTextureName == L"None")
					continue;

				CTextureBase* pTexBase = pGameInstance->GetOrAddTexture(L"Texture_" + wstrTextureName , nullptr);
				if (pTexBase == nullptr)
				{
					MSG_BOX("Water Texture 연동중 문제 발생");
					continue;
				}
				this->arrayTextureBase[i] = pTexBase;
			}
		}
	}
}
void WATER_DESC::to_Json(json& SaveJson)
{
	
	Engine_Utils::write_vec4_xyzw(SaveJson["Color"] , this->vMI_TintColor );
	Engine_Utils::write_vec2_xy(SaveJson["Speed 1"],this->vSpeed1);
	Engine_Utils::write_vec2_xy(SaveJson["Speed 2"],this->vSpeed2);

	Engine_Utils::write_vec2_xy(SaveJson["UV Power"],this->vWaterUVPower);


	Engine_Utils::write_vec2_xy(SaveJson["Distortion UV Power"],this->vDistortionUVPower);

	Engine_Utils::write_vec2_xy(SaveJson["Distortion Speed"],this->vDistortionSpeed);


	SaveJson["Distortion Power"] = this->fDistortionPower;

	
	Engine_Utils::write_vec2_xy(SaveJson["Sparkle UV Power"],this->vSparkleUVPower);
	SaveJson["Sparkle Power"] = this->fSparklePower;

	auto& Texture_SaveJson = SaveJson["Texture Names"];;

	for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END); ++i)
	{
		string strTextureName{};
		CTextureBase* pTexBase{ nullptr };
		pTexBase = this->arrayTextureBase[i];
		if (!pTexBase)
		{
			strTextureName = "None";
		}
		else
		{
			strTextureName = Engine_Utils::ToString(pTexBase->Get_Name());
		}
		Texture_SaveJson[WaterTextureType_ToString(static_cast<EWaterTextureType>(i))] = strTextureName;
	}
}

#pragma endregion

#pragma region Fog Desc

FOG_DESC::FOG_DESC(const FOG_DESC& rhs)
	: CLIENT_MAKEPATH_DESC_BASE(rhs), vMI_TintColor{ rhs.vMI_TintColor }
	, arrayTextureBase{rhs.arrayTextureBase}
	, fDistortionPower{1.f}
{
	for (auto& Tex : arrayTextureBase)
		Safe_AddRef(Tex);
	memcpy(this->vUV, this->vUV, sizeof(Vec4) * ENUM_TO_UINT(EFogTextureType::END));
}

FOG_DESC::~FOG_DESC()
{
	for (auto& Texture : arrayTextureBase)
		Safe_Release(Texture);
}

void FOG_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Color"))
	{
		Engine_Utils::read_vec4_xyzw(LoadJson["Color"], this->vMI_TintColor);
	}

	if (LoadJson.contains("Distortion Power"))
	{
		this->fDistortionPower = LoadJson["Distortion Power"];
	}


	auto* pGameInstance = CGameInstance::GetInstance();
	for (_uint i = 0; i < static_cast<_uint>(EFogTextureType::END); ++i)
	{
		string strFindKey = g_szFogTextureType[i];
		if (LoadJson.contains(strFindKey))
		{
			const json& KeyJson = LoadJson[strFindKey];
			if (KeyJson.contains("UV"))
			{
				Engine_Utils::read_vec4_xyzw(KeyJson["UV"], this->vUV[i]);
			}
			if (KeyJson.contains("Texture"))
			{
				string strTextureName = KeyJson["Texture"];
				if (strTextureName != "None")
				{
					CTextureBase* pTexBase = pGameInstance->GetOrAddTexture(L"Texture_" + Engine_Utils::ToWString(strTextureName), nullptr);
					if (pTexBase == nullptr)
					{
						MSG_BOX("Water Texture 연동중 문제 발생");
						continue;
					}
					this->arrayTextureBase[i] = pTexBase;
				}
			}
		}
	}
}

void FOG_DESC::to_Json(json& SaveJson)
{
	Engine_Utils::write_vec4_xyzw(SaveJson["Color"], this->vMI_TintColor);

	SaveJson["Distortion Power"] =  this->fDistortionPower ;

	for (_uint i = 0; i < ENUM_TO_UINT(EFogTextureType::END); ++i)
	{
		json& KeyJson = SaveJson[g_szFogTextureType[i]];
		Engine_Utils::write_vec4_xyzw(KeyJson["UV"], this->vUV[i]);
		string strTextureName{"None"};
		if(this->arrayTextureBase[i])
			strTextureName = Engine_Utils::ToString(this->arrayTextureBase[i]->Get_Name());
		KeyJson["Texture"] = strTextureName;
	}
}

#pragma endregion



#pragma endregion


#pragma region Batch Monster

void BATCH_MONSTER_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Batch Monster Type"))
	{
		this->eBatchMonsterType = DTO::MakeMonsterType_ToEnum(LoadJson["Batch Monster Type"].get<string>());
	}
}

void BATCH_MONSTER_DESC::to_Json(json& SaveJson)
{
	SaveJson["Batch Monster Type"] = DTO::MakeMonsterType_ToString(this->eBatchMonsterType);
}

#pragma endregion 

#pragma region Batch Object

void BATCH_OBJECT_DESC::Change_BatchObjecType(DTO::EMakeObjectType eChangeType)
{
	if (eChangeType == DTO::EMakeObjectType::END) return;
	if (eChangeType == this->eBatchObjectType) return;

	/* 기존에 있던 Desc 정리 */

	Safe_Delete(pBatchObjectDesc);
	pBatchObjectDesc = nullptr;

	eBatchObjectType = eChangeType;

	/* Description 다시할당 */
	pBatchObjectDesc = Make_BatchObject_Desc(eBatchObjectType);

	return;
}

void BATCH_OBJECT_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Batch Object Info"))
	{
		auto& BatchObjectDesc_LoadJson = LoadJson["Batch Object Info"];
		
		if(BatchObjectDesc_LoadJson.contains("Type"))
		{
			this->Change_BatchObjecType(DTO::MakeObjectType_ToEnum(BatchObjectDesc_LoadJson["Type"].get<string>()));
		}

		if (pBatchObjectDesc)
		{
			pBatchObjectDesc->from_Json(BatchObjectDesc_LoadJson["Desc"]);
		}
	}
}

void BATCH_OBJECT_DESC::to_Json(json& SaveJson)
{
	auto& BatchObjectDesc_SaveJson = SaveJson["Batch Object Info"];

	BatchObjectDesc_SaveJson["Type"] = DTO::MakeObjectType_ToString(this->eBatchObjectType);

	if(this->pBatchObjectDesc)
		pBatchObjectDesc->to_Json(BatchObjectDesc_SaveJson["Desc"]);
}


#pragma region Battle Field

BATTLE_FIELD_DESC::BATTLE_FIELD_DESC()
	: fRadius{ 1.f }, vExtents{ 1.f,1.f,1.f }, pBattleFieldColliderBox{ nullptr }, pBattleFieldColliderSphere{ nullptr }, eFieldType{ Field_Type::Box }
{
}
BATTLE_FIELD_DESC::BATTLE_FIELD_DESC(const BATTLE_FIELD_DESC& rhs)
	: fRadius{ rhs.fRadius }, vExtents{ rhs.vExtents }, pBattleFieldColliderSphere{ rhs.pBattleFieldColliderSphere }, pBattleFieldColliderBox{ rhs.pBattleFieldColliderBox }, eFieldType{ rhs.eFieldType }
{
}
BATTLE_FIELD_DESC::~BATTLE_FIELD_DESC()
{
	Safe_Release(pBattleFieldColliderSphere);
	Safe_Release(pBattleFieldColliderBox);
}
void BATTLE_FIELD_DESC::Update_Collider(const Matrix* pWorldMatrix)
{
	if (!pBattleFieldColliderSphere)
		return;
	if (!pBattleFieldColliderBox)
		return;

	/* 위치 + Rotation 만 빼와서 Update 해주자 */


	if (eFieldType == BATTLE_FIELD_DESC::Field_Type::Box)
	{
		static_cast<BoundingOrientedBox*>(static_cast<CBounding_OBB*>(pBattleFieldColliderBox->Get_Bounding())->Get_OriginalDesc())->Extents = this->vExtents;
		if (pWorldMatrix)
		{
			Vec3 vPosition{}, vScale{};
			Quat vQuat{};

			Matrix World = *pWorldMatrix;
			World.Decompose(vScale, vQuat, vPosition);

			World = Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
			pBattleFieldColliderBox->Update(World);
		}
	}
	else if (eFieldType == BATTLE_FIELD_DESC::Field_Type::Sphere)
	{
		static_cast<BoundingSphere*>(static_cast<CBounding_Sphere*>(pBattleFieldColliderSphere->Get_Bounding())->Get_OriginalDesc())->Radius = this->fRadius;

		if (pWorldMatrix)
		{
			Vec3 vPosition{}, vScale{};
			Quat vQuat{};

			Matrix World = *pWorldMatrix;
			World.Decompose(vScale, vQuat, vPosition);

			World = Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
			pBattleFieldColliderSphere->Update(World);
		}
		/* 나중에 Rotation 값을 실제 SRT Rotation 값으로 들어가면 될거 같기도하고... */
	}
}
void BATTLE_FIELD_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Field Type"))
	{
		this->eFieldType = BATTLE_FIELD_DESC::FieldType_ToEnum(LoadJson["Field Type"].get<string>());
	}

	if (LoadJson.contains("Radius"))
	{
		this->fRadius = LoadJson["Radius"];
	}
	if (LoadJson.contains("Extents"))
	{
		Engine_Utils::read_vec3_xyz(LoadJson["Extents"], this->vExtents);
	}

}
void BATTLE_FIELD_DESC::to_Json(json& SaveJson)
{
	SaveJson["Field Type"] = BATTLE_FIELD_DESC::FieldType_ToString(this->eFieldType);

	if (this->eFieldType == BATTLE_FIELD_DESC::Field_Type::Box)
	{
		Engine_Utils::write_vec3_xyz(SaveJson["Extents"], this->vExtents);
	}
	else
	{
		SaveJson["Radius"] = this->fRadius;
	}
}
#pragma endregion

#pragma region Batch Point Light

POINTLIHGT_DESC::POINTLIHGT_DESC()
	:BATCH_OBJECT_DESC_BASE()
	, isFlicker{ false }
	, fFlickerSpeed{ 1.f }
	, fFlickerMin{ 1.f }
	, fBaseRange{1.f}
	, pDebugLight{ nullptr }
	, tLightDesc{}
{
	this->tLightDesc.eType = LIGHT_TYPE::POINT;
	this->tLightDesc.vDiffuse  = {1.f,1.f,1.f,1.f};
	this->tLightDesc.vAmbient  = {1.f,1.f,1.f,1.f};
	this->tLightDesc.vSpecular = {1.f,1.f,1.f,1.f};
	pDebugLight = CLight::Create(this->tLightDesc);
}

POINTLIHGT_DESC::POINTLIHGT_DESC(const POINTLIHGT_DESC& rhs)
	: BATCH_OBJECT_DESC_BASE(rhs)
	, tLightDesc{rhs.tLightDesc}
	, isFlicker{ rhs.isFlicker }
	, fFlickerSpeed{ rhs.fFlickerSpeed }
	, fFlickerMin{ rhs.fFlickerMin }
	, fBaseRange{rhs.fBaseRange}
	, pDebugLight{ rhs.pDebugLight }
{
	/* 이때 생성 */
	pDebugLight = CLight::Create(this->tLightDesc);
}
POINTLIHGT_DESC::~POINTLIHGT_DESC()
{
	Safe_Release(pDebugLight);
}

void POINTLIHGT_DESC::Update_Light(const Vec4& vPos)
{
	if (this->pDebugLight)
	{
		pDebugLight->Setup_Diffuse(this->tLightDesc.vDiffuse);
		pDebugLight->Setup_Ambient(this->tLightDesc.vAmbient);
		pDebugLight->Setup_Specular(this->tLightDesc.vSpecular);
		pDebugLight->Setup_Range(this->fBaseRange);
		pDebugLight->Setup_Position(vPos);
	}
}

void POINTLIHGT_DESC::from_Json(const json& LoadJson)
{
	if (!LoadJson.contains("Light Data")) return;
	const auto& LightData_LoadJson = LoadJson["Light Data"];

	this->tLightDesc.eType = Engine_Utils::LIGHTTYPE_ToEnum(LightData_LoadJson["Type"]);

	Engine_Utils::read_vec4_xyzw(LightData_LoadJson["Diffuse"], this->tLightDesc.vDiffuse);
	Engine_Utils::read_vec4_xyzw(LightData_LoadJson["Ambient"], this->tLightDesc.vAmbient);
	Engine_Utils::read_vec4_xyzw(LightData_LoadJson["Specular"], this->tLightDesc.vSpecular);

	if (LightData_LoadJson.contains("Range"))
	{
		this->fBaseRange = LightData_LoadJson["Range"];
		this->tLightDesc.fRange = this->fBaseRange; // 로드 시점엔 일단 원본 크기로 세팅
	}

	// 4. Flicker 데이터 복원
	if (LightData_LoadJson.contains("Flicker"))
	{
		this->isFlicker = true;
		const auto& FlickerData_LoadJson = LightData_LoadJson["Flicker"];

		this->fFlickerSpeed = FlickerData_LoadJson.value("Speed", 1.0f);
		this->fFlickerMin = FlickerData_LoadJson.value("Min", 0.5f);
	}
	else
	{
		this->isFlicker = false;
	}
}

void POINTLIHGT_DESC::to_Json(json& SaveJson)
{
	auto& LightData_SaveJson = SaveJson["Light Data"];
	LightData_SaveJson["Type"] = Engine_Utils::LIGHTTYPE_ToString(this->tLightDesc.eType);

	Engine_Utils::write_vec4_xyzw(LightData_SaveJson["Diffuse"],this->tLightDesc.vDiffuse);
	Engine_Utils::write_vec4_xyzw(LightData_SaveJson["Ambient"],this->tLightDesc.vAmbient);
	Engine_Utils::write_vec4_xyzw(LightData_SaveJson["Specular"],this->tLightDesc.vSpecular);
	LightData_SaveJson["Range"] = this->fBaseRange;			/* 아마 Desc안에있는 Range는 업데이트때마다 달라진다 */

	if (this->isFlicker == true)
	{
		auto& FlickerData_SaveJson = LightData_SaveJson["Flicker"];
		FlickerData_SaveJson["Speed"]	= this->fFlickerSpeed;
		FlickerData_SaveJson["Min"]		= this->fFlickerMin;
	}
}


#pragma endregion



#pragma region Trigger Box

void TRIGGERBOX_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Extents"))
	{
		Engine_Utils::read_vec3_xyz(LoadJson["Extents"], this->vExtents);
	}
	if (LoadJson.contains("Rotation"))
	{
		Vec3 vDegree{};
		Engine_Utils::read_vec3_xyz(LoadJson["Rotation"], vDegree);
		this->vRotation = Vec3(XMConvertToRadians(vDegree.x), XMConvertToRadians(vDegree.y), XMConvertToRadians(vDegree.z));
	}


	if (LoadJson.contains("bHasQuest"))
		LoadJson.at("bHasQuest").get_to(this->bHasQuest);
	else
		this->bHasQuest = false;

	if (LoadJson.contains("tQuestObjectDesc"))
	{
		const auto& questJson = LoadJson.at("tQuestObjectDesc");

		if (questJson.is_array())
		{
			questJson.get_to(this->tQuestObjectDesc);
		}
		else if(questJson.is_object())
		{
			DTO::QUEST_CHAPTERDESC oldFormatDesc;
			questJson.get_to(oldFormatDesc);
			this->tQuestObjectDesc.push_back(oldFormatDesc);
		}
	}
}

void TRIGGERBOX_DESC::to_Json(json& SaveJson)
{
	Engine_Utils::write_vec3_xyz(SaveJson["Extents"], this->vExtents);

	Vec3 vDegree = Vec3(XMConvertToDegrees(vRotation.x), XMConvertToDegrees(vRotation.y), XMConvertToDegrees(vRotation.z));
	Engine_Utils::write_vec3_xyz(SaveJson["Rotation"], vDegree );

	SaveJson["bHasQuest"] = this->bHasQuest;

	if (this->bHasQuest && !this->tQuestObjectDesc.empty())
		SaveJson["tQuestObjectDesc"] = this->tQuestObjectDesc;
}
#pragma endregion


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


#pragma region Monster Spawner 

MonsterSpawnData::MonsterSpawnData(const MonsterSpawnData& Copy)
	: vPosition(Copy.vPosition) , vScale(Copy.vScale) , vPitchYawRoll(Copy.vPitchYawRoll) , eMakeMonsterType(Copy.eMakeMonsterType)
	 , fSpawnDelayTime(Copy.fSpawnDelayTime) , isPreviewDebugModel(Copy.isPreviewDebugModel), pDebugModel(Copy.pDebugModel)
{
	Safe_AddRef(pDebugModel);
}

MonsterSpawnData::~MonsterSpawnData()
{
	Safe_Release(pDebugModel);
}

void	MonsterSpawnData::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Monster Type"))
		this->eMakeMonsterType = DTO::MakeMonsterType_ToEnum(LoadJson["Monster Type"].get<string>());

	if (LoadJson.contains("Delay Time"))
		this->fSpawnDelayTime = LoadJson["Delay Time"];

	if (LoadJson.contains("SRT"))
	{
		const auto& SRT_LoadJson = LoadJson["SRT"];
		if (SRT_LoadJson.contains("Scale"))
		{
			Engine_Utils::read_vec3_xyz(SRT_LoadJson["Scale"] , this->vScale);
		}
		if (SRT_LoadJson.contains("PitchYawRoll"))
		{
			Engine_Utils::read_vec3_PitchYawRoll(SRT_LoadJson["PitchYawRoll"], this->vPitchYawRoll);
		}
		if (SRT_LoadJson.contains("Position"))
		{
			Engine_Utils::read_vec3_xyz(SRT_LoadJson["Position"], this->vPosition);
		}
	}

	return;
}

void	MonsterSpawnData::to_Json(json& SaveJson)
{
	/* 몬스터 타입 */
	SaveJson["Monster Type"] = DTO::MakeMonsterType_ToString(this->eMakeMonsterType);
	
	/* 소환 딜레이 시간 */
	SaveJson["Delay Time"] = this->fSpawnDelayTime;

	/* 위치 데이터 */
	Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Scale"],					this->vScale);
	Engine_Utils::write_vec3_PitchYawRoll(SaveJson["SRT"]["PitchYawRoll"],	this->vPitchYawRoll);
	Engine_Utils::write_vec3_xyz(SaveJson["SRT"]["Position"],				this->vPosition);



	return;
}



void TRIGGERBOX_MONSTERSPAWNER_DESC::from_Json(const json& LoadJson)
{
	Super::from_Json(LoadJson);
	
	this->vecMonsterSpawnData.clear();
	if(LoadJson.contains("Monster Spawn Data"))
	{
		const auto& MonsterSpawnData_LoadJsonArray = LoadJson["Monster Spawn Data"];
		this->vecMonsterSpawnData.reserve(MonsterSpawnData_LoadJsonArray.size());
		for (auto& MonsterSpawnData_LoadJson : MonsterSpawnData_LoadJsonArray)
		{
			if (MonsterSpawnData_LoadJson.is_null())
				continue;
			MonsterSpawnData tData{};
			tData.from_Json(MonsterSpawnData_LoadJson);
			vecMonsterSpawnData.push_back(tData);
		}
	}
}

void TRIGGERBOX_MONSTERSPAWNER_DESC::to_Json(json& SaveJson)
{
	Super::to_Json(SaveJson);

	for (auto& MonsterSpawnData : this->vecMonsterSpawnData)
	{
		json SaveObject{};
		MonsterSpawnData.to_Json(SaveObject);
		SaveJson["Monster Spawn Data"].push_back(SaveObject);
	}
}

#pragma endregion


#pragma region GlobalEvent BroadCaster


void TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC::from_Json(const json& LoadJson)
{
	Super::from_Json(LoadJson);

	if (LoadJson.contains("Global Event Broadcast Names"))
	{
		const auto& JsonArray = LoadJson["Global Event Broadcast Names"];
		vecGlobalEventBroadCasetNames.reserve(JsonArray.size());
		for (auto& Json : JsonArray)
		{
			vecGlobalEventBroadCasetNames.push_back(Json.get<string>());
		}
	}
}

void TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC::to_Json(json& SaveJson)
{
	Super::to_Json(SaveJson);

	SaveJson["Global Event Broadcast Names"] = this->vecGlobalEventBroadCasetNames;
}

#pragma endregion


#pragma region TutorialUIEvent Desc


void TRIGGERBOX_TUTORIALUIEVENT_DESC::from_Json(const json& LoadJson)
{
	Super::from_Json(LoadJson);

	if (LoadJson.contains("Tutorial UI Event ID"))
	{
		this->strEventName = LoadJson["Tutorial UI Event ID"];
	}
}

void TRIGGERBOX_TUTORIALUIEVENT_DESC::to_Json(json& SaveJson)
{
	Super::to_Json(SaveJson);

	SaveJson["Tutorial UI Event ID"] = this->strEventName;
}

#pragma endregion


#pragma region Cinemaitc Player
void TRIGGERBOX_CINEMATICPLAYER_DESC::from_Json(const json& LoadJson)
{
	Super::from_Json(LoadJson);


	if (LoadJson.contains("Play Cinematic Name"))
	{
		this->strCinematicName = LoadJson["Play Cinematic Name"];
	}
}

void TRIGGERBOX_CINEMATICPLAYER_DESC::to_Json(json& SaveJson)
{
	Super::to_Json(SaveJson);

	SaveJson["Play Cinematic Name"] = this->strCinematicName;
}
#pragma endregion


#pragma endregion

#pragma endregion

#pragma region NPC Desc

void BATCH_NPC_DESC::from_Json(const json& LoadJson)
{
	if (LoadJson.contains("Batch NPC Type"))
	{
		this->eBatchNPCType = DTO::MakeNPCType_ToEnum(LoadJson["Batch NPC Type"].get<string>());
	}
	else
		this->eBatchNPCType = OBJECT_ENUM_TAG::NPC_DEFAULT;

	if (LoadJson.contains("bHasQuest"))
		LoadJson.at("bHasQuest").get_to(this->bHasQuest);
	else
		this->bHasQuest = false;

	if (LoadJson.contains("tQuestObjectDesc"))
	{
		const auto& questJson = LoadJson.at("tQuestObjectDesc");

		if (questJson.is_array())
		{
			questJson.get_to(this->tQuestObjectDesc);
		}
		else if (questJson.is_object())
		{
			DTO::QUEST_CHAPTERDESC oldFormatDesc;
			questJson.get_to(oldFormatDesc);
			this->tQuestObjectDesc.push_back(oldFormatDesc);
		}
	}
}

void BATCH_NPC_DESC::to_Json(json& SaveJson)
{
	SaveJson["Batch NPC Type"] = DTO::MakeNPCType_ToString(this->eBatchNPCType);

	SaveJson["bHasQuest"] = this->bHasQuest;

	if (this->bHasQuest && !this->tQuestObjectDesc.empty())
		SaveJson["tQuestObjectDesc"] = this->tQuestObjectDesc;
}
#pragma endregion

#pragma region Make_BatchObject_Desc cpp구현부
BATCH_OBJECT_DESC_BASE* Make_BatchObject_Desc(DTO::EMakeObjectType eBatchObjectType, BATCH_OBJECT_DESC_BASE* pBase)
{
	switch (eBatchObjectType)
	{
	case DTO::EMakeObjectType::Battle_Field:	return pBase == nullptr ? new BATTLE_FIELD_DESC		:	new BATTLE_FIELD_DESC(*static_cast<BATTLE_FIELD_DESC*>(pBase));
	case DTO::EMakeObjectType::PointLight:		return pBase == nullptr ? new POINTLIHGT_DESC		:	new POINTLIHGT_DESC(*static_cast<POINTLIHGT_DESC*>(pBase));
	default:									return nullptr;
	}

	return nullptr;
}


#pragma endregion


NS_END

