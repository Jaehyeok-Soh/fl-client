#include "pch.h"
#include "UEMapdataParser.h"
#include <fstream>
#include "Engine_Utils.h"
#include "Model.h"
#include "StaticModel.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUEMapdataParser)


CUEMapdataParser::CUEMapdataParser()
{
	m_vecTypeFilter = { "StaticMeshComponent" , "InstancedStaticMeshComponent" };

	m_umapConvertedMapData.clear();
	m_umapUnreal_Map_Data.clear();
}

HRESULT CUEMapdataParser::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_vecTypeFilter = { "StaticMeshComponent" , "InstancedStaticMeshComponent" };
	m_vecMtlTextureFilter = { "PM_Diffuse","PM_Normals","PM_SpecularMasks","PM_Emissive"};

	m_vecOuterFilter = {"LOD"};
	m_umapConvertedMapData.clear();
	m_umapUnreal_Map_Data.clear();

	m_pDevice = pDevice;
	m_pContext = m_pContext;
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
	return S_OK;
}


bool CUEMapdataParser::Filter(const string& strName, const string& strType)
{
	if (m_vecTypeFilter.empty()) return true;
	if (strName.empty()) return true;
	if (strType.empty()) return true;

	
	for (auto& strFilterOuter : m_vecOuterFilter)
	{
		if (strType.find(strFilterOuter) != string::npos)
			return true;
	}

	bool isFiltering_Type{ true };
	bool isFiltering_Name{ true };


	for (auto& strFilter : m_vecTypeFilter)
	{
		isFiltering_Type = true;
		isFiltering_Name = true;

		if (strType == strFilter)
			isFiltering_Type = false;
		if (strName.find(strFilter) != string::npos)
			isFiltering_Name = false;

		if (!isFiltering_Type && !isFiltering_Name)
			return false;
	}


	return true;
}

vector<CONVERTED_MAPDATA> CUEMapdataParser::Convert_UE_MapData(const vector<UE_MAP_DATA>& tData)
{
	vector<CONVERTED_MAPDATA> vecConvertedData{};
	vector<UE_MAP_DATA>		 vecUEMapData = tData;
	if (tData.empty()) return vecConvertedData;

	vecConvertedData.reserve(tData.size());


	for (_uint i = 0; i < ENUM_TO_UINT(tData.size()); ++i)
	{
		CONVERTED_MAPDATA tConvertedData{};
		UE_MAP_DATA		 tUEMapData{};

		tUEMapData = vecUEMapData[i];
		tConvertedData.eType = tUEMapData.Type_ToString();


		tConvertedData.tUsingModelInfo.wstrName = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectName);
		tConvertedData.tUsingModelInfo.wstrPath = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectPath);

		for (auto& Material : tUEMapData.tProperties.tOverrideMaterials.vecObjectInfo)
		{
			if (Material.strObjectName.empty())
				continue;
			tConvertedData.tUsingModelInfo.vecMaterialInfo.push_back({ false ,Engine_Utils::ToWString(Material.strObjectName) , Engine_Utils::ToWString(Material.strObjectPath) });
		}
		Change_UsingModelInfo(tConvertedData.tUsingModelInfo);


		if(tConvertedData.eType == EStaticModel_Type::DEFUALT)
		{
			tConvertedData.vScale		 = tUEMapData.tProperties.vRelativeScale;
			tConvertedData.vPosition     = tUEMapData.tProperties.vRelativeLocation;
			tConvertedData.vPitchYawRoll = tUEMapData.tProperties.vRelativeRotation;

			vecConvertedData.push_back(tConvertedData);
		}

#pragma region INSTANCE

		else if (tConvertedData.eType == EStaticModel_Type::INSTANCE)
		{
			for (auto& PerInstanceSMData : tUEMapData.vecPerInstanceSMData)
			{
				if (PerInstanceSMData.isNull == true)
					continue;
				
				tConvertedData.vPosition = PerInstanceSMData.tTransformData.vTranslation;
				tConvertedData.vScale = PerInstanceSMData.tTransformData.vScale3D;
				tConvertedData.vQuaternion = PerInstanceSMData.tTransformData.vRotation;

				vecConvertedData.push_back(tConvertedData);
			}
		}

#pragma endregion

	}

	return vecConvertedData;
}

void CUEMapdataParser::Change_SRT(Vec3* vScale, Vec3* vPitchYawRoll, Vec3* vPosition , EStaticModel_Type eType)
{	

}

void CUEMapdataParser::Change_ObjectPath(OUT _wstring& wstrModelName, OUT _wstring& wstrModelPath,EObject_Path_Type eType)
{
	wstring wstrName = wstrModelName;
	wstring wstrPath = wstrModelPath;
	 

	size_t Pos_Point = wstrPath.rfind(L".");
	if (Pos_Point != std::string::npos)
		wstrPath.replace(Pos_Point, wstrPath.length(), g_wszModelExtension);


	wstring wstrChange = L"Level";
	wstring wstrTarget = L"Scene";
	size_t Pos_Scene = wstrPath.find(wstrTarget);
	if (Pos_Scene != std::string::npos)
		wstrPath.replace(0, Pos_Scene + wstrTarget.length(), wstrChange);
	else
	{
		wstrTarget = L"Content";
		size_t Pos_Target = wstrPath.find(wstrTarget);
		if (Pos_Target != std::string::npos)
			wstrPath.replace(0, Pos_Target + wstrTarget.length(), wstrChange);
	}

	vector<wstring> vecTargetWStr = { L"EN000_" , L"EN001_" , L"EN002_" , L"EN003_" };
	size_t Pos_EN{ std::string::npos };

	for (auto& Target : vecTargetWStr)
	{
		Pos_EN = wstrPath.find(Target);
		if (Pos_EN != std::string::npos)
		{
			wstrPath.erase(Pos_EN, Target.length());
			break;
		}
	}

	wstrModelName = path(wstrPath).filename().stem().wstring();
	if (eType == EObject_Path_Type::Mtl)
	{
		wstrModelPath = wstring(g_wszAssetRelativePath) + L"Map/" + path(wstrPath).parent_path().wstring()+ L"/" + wstrModelName + L".json";
		//if (!std::filesystem::exists(wstrModelPath))
		//	MSG_BOX(" 변환된 Mtl Path 값이 존재하지 않습니다 [ 확인 바람 ] ");
	}
	else if (eType == EObject_Path_Type::Model)
	{
		wstrModelPath = L"Map/" + path(wstrPath).parent_path().wstring() + L"/Model/" + wstrModelName + L".fbx";
	}
	else if (eType == EObject_Path_Type::Tex)
	{
		wstrModelPath = wstring(g_wszAssetRelativePath) + L"Map/" + path(wstrPath).parent_path().wstring() + L"/" + wstrModelName + L".png";
	}

	/* Lies of P */
	//wstrModelName = path(wstrPath).filename().stem().wstring();
}

void CUEMapdataParser::Change_UsingModelInfo(OUT USING_MODEL_INFO& tUsingModelInfo)
{
	/* Converted 된 이후 Mesh 파일 경로 변환 */
	Change_ObjectPath(tUsingModelInfo.wstrName,tUsingModelInfo.wstrPath,EObject_Path_Type::Model);
	/* Converted 된 이후 Material Json이 저장된 경로 변환 */
	Change_Material_JsonFile_Path(tUsingModelInfo.wstrMtl_JsonFile_Path,tUsingModelInfo.wstrPath);


	for (auto& UsingMtlInfo : tUsingModelInfo.vecMaterialInfo)
	{
		Change_UsingMaterialInfo(UsingMtlInfo);
	}
}

void CUEMapdataParser::Change_Material_JsonFile_Path(OUT wstring& wstrMaterialJsonFilePath, const wstring& wstrModelPath)
{
	/* 모델 패스에서 한번뒤로나가서 Meterial 폴더로 변환 */
	path pathChange{ wstrModelPath };
	wstring wstrModelName = path(pathChange).filename().stem();
	pathChange._Remove_filename_and_separator();
	pathChange = pathChange.parent_path();

	pathChange = g_wszModelRelativePath + pathChange.wstring() + L"/Material";

	if (!std::filesystem::exists(pathChange))
	{
		//MSG_BOX(" Model Material 폴더 경로를 찾을 수 없습니다 [  확인 바랍니다 ] ");
		return;
	}

	pathChange += L"/" + wstrModelName + L".json";

	if (!std::filesystem::exists(pathChange))
	{
		MSG_BOX(" 저장된 모델의 메테리얼 Json 파일 경로를 찾을 수 없습니다 [  확인 바랍니다 ] ");
		return;
	}

	wstrMaterialJsonFilePath = pathChange;
}

void CUEMapdataParser::Change_UsingMaterialInfo(OUT USING_MATERIAL_INFO& tUsingMtlInfo)
{
	if (tUsingMtlInfo.isNull == true) return;

	tUsingMtlInfo.vecUsingTextureInfo;

	/* 임마를 다시 변환 시켜줘야 함 */
	tUsingMtlInfo.wstrOriginMtl_JsonFile_Path;

	Change_ObjectPath(tUsingMtlInfo.wstrOriginMtl_JsonFile_Name, tUsingMtlInfo.wstrOriginMtl_JsonFile_Path,EObject_Path_Type::Mtl);
	Change_UsingMaterialTexturePath(tUsingMtlInfo);
}

void CUEMapdataParser::Change_UsingMaterialTexturePath(OUT USING_MATERIAL_INFO& tUsingMtlInfo)
{
	if (tUsingMtlInfo.isNull == true) return;

	std::ifstream is (tUsingMtlInfo.wstrOriginMtl_JsonFile_Path);

	if (!is.is_open()) return;

	json LoadJson{};
	is >> LoadJson;

	if (!LoadJson.contains("Textures")) return;

	auto& j = LoadJson["Textures"];
	
	for (auto& MtlTexFilter : m_vecMtlTextureFilter)
	{
		if (j.contains(MtlTexFilter))
		{
			std::pair<wstring, wstring> TexInfo{ Engine_Utils::ToWString(MtlTexFilter) , Engine_Utils::ToWString(j.value(MtlTexFilter,""))};
			Change_ObjectPath(TexInfo.second, TexInfo.second,EObject_Path_Type::Tex);
			tUsingMtlInfo.vecUsingTextureInfo.push_back(TexInfo);
		}
	}

}

vector<UE_MAP_DATA>* CUEMapdataParser::Get_Unreal_MapData(const wstring& FindKey)
{
	const auto iter = m_umapUnreal_Map_Data.find(FindKey);
	if (iter == m_umapUnreal_Map_Data.end()) return nullptr;
	return &iter->second;
}

vector<CONVERTED_MAPDATA>* CUEMapdataParser::Get_Converted_MapData(const wstring& FindKey)
{
	const auto iter = m_umapConvertedMapData.find(FindKey);
	if (iter == m_umapConvertedMapData.end()) return nullptr;
	return &iter->second;
}


vector<wstring> CUEMapdataParser::Get_ConvertedFilePathList()
{
	vector<wstring> wstrFileNameList{};

	for (auto& Pair : m_umapConvertedMapData)
		wstrFileNameList.push_back(Pair.first);

	return wstrFileNameList;
}

HRESULT CUEMapdataParser::Convert_UnrealRawMapData(const wchar_t* wszUERawDataJsonFile)
{
	if (wszUERawDataJsonFile == nullptr)  return E_FAIL;

	wstring MapDataPath = wszUERawDataJsonFile;
	
	vector<wstring> wstrFilter = { m_WstringConverted , m_WstringFiltering };
	for (auto& Filter : wstrFilter)
	{
		if (MapDataPath.find(Filter) != wstring::npos)
		{
			MSG_BOX("[Unreal Raw Data Json] 파일이 아닙니다");
			return S_OK;
		}
	}

	
	vector<UE_MAP_DATA>* vecUEData = Get_Unreal_MapData(wszUERawDataJsonFile);

	if (vecUEData != nullptr)
	{
		m_umapConvertedMapData.at(wszUERawDataJsonFile) = Convert_UE_MapData(*vecUEData);
		MSG_BOX(" Unreal Raw Data Load Complete ");
		return S_OK;
	}


	std::ifstream ifs(wszUERawDataJsonFile, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		MSG_BOX("CUEMapdataParser::Read_Mapdata, File open failed");
		return E_FAIL;
	}
	
	json UE_Map_Datas_Json{};
	ifs >> UE_Map_Datas_Json;

	vector<UE_MAP_DATA> vecData{};
	vector<CONVERTED_MAPDATA> vecConvertedData{};
	for (const json& UE_Map_Data_Json : UE_Map_Datas_Json)
	{
		if (Filter(UE_Map_Data_Json.value("Name", ""), UE_Map_Data_Json.value("Type", ""))) 
			continue;
		if (UE_Map_Data_Json.value("Outer", "").find("LOD") != wstring::npos)
			continue;


		UE_MAP_DATA tData{};
		tData = UE_Map_Data_Json;
		if(!tData.m_isFiltering)
			vecData.push_back(tData);
	}

	m_umapUnreal_Map_Data.emplace(MapDataPath, vecData);
	/* 바로 변환 시키기 */
	m_umapConvertedMapData.emplace(MapDataPath, Convert_UE_MapData(vecData));

	MSG_BOX(" Unreal Raw Data Load Complete ");

	return S_OK;
}

HRESULT CUEMapdataParser::Batch_UnrealRawMapData(const wchar_t* wszFileName)
{
	vector<CONVERTED_MAPDATA>* pFind = Get_Converted_MapData(wszFileName);
	if (pFind == nullptr) return E_FAIL;

	UINT iLevelID = ENUM_TO_UINT(ELevelType::MAP);

	CGameObject* pResult{nullptr};

	for (auto& CONVERTED_MAPDATA : *pFind)
	{
		CStaticModel::STATICMODEL_DESC desc{};
		desc.wstrLayerTag = g_wszStaticModelLayer;
		desc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
		desc.wstrModelPath = CONVERTED_MAPDATA.tUsingModelInfo.wstrPath;
		desc.wstrModelName = CONVERTED_MAPDATA.tUsingModelInfo.wstrName;
		desc.eType = CONVERTED_MAPDATA.eType;
		desc.isLoaded = true;
		CTransform::TRANSFORM_DESC tTramsoformDesc{};

		if (CONVERTED_MAPDATA.tUsingModelInfo.wstrName.find(L"Wall") != wstring::npos)
			int a = 0;

		if (desc.eType == EStaticModel_Type::INSTANCE)
		{
			tTramsoformDesc.bInstance = true;
			tTramsoformDesc.vQuaternion = CONVERTED_MAPDATA.vQuaternion;
		}

		tTramsoformDesc.vScale				= CONVERTED_MAPDATA.vScale;
		tTramsoformDesc.vRotation_Degrees	= CONVERTED_MAPDATA.vPitchYawRoll;
		tTramsoformDesc.vPosition			= CONVERTED_MAPDATA.vPosition;

		{
			tTramsoformDesc.vPosition *= m_fMulScale;
		}

		desc.pTransform_Desc = (void*)&tTramsoformDesc;

		if (!(pResult = m_pGameInstance->Add_GameObject(desc.iLevelIndex, L"Prototype_GameObject_StaticModel", desc.iLevelIndex, g_wszStaticModelLayer, &desc)))
		{
			Safe_Release(pResult);
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CUEMapdataParser::Save_ConvertedRawMapData(const wchar_t* wszFilePath)
{
	vector<CONVERTED_MAPDATA>* pFind = Get_Converted_MapData(wszFilePath);

	if (pFind == nullptr) return E_FAIL;

	path FilePath{ wszFilePath };
	wstring wstrFileName = path(FilePath).filename().stem();
	wstrFileName += L"_Converted.json";
	wstring wstrSavePath = FilePath.remove_filename();
	wstrSavePath += wstrFileName;

	json SaveJson = json::array();

	for (auto& Converted_MapData : *pFind)
		SaveJson.push_back(Converted_MapData);

	std::ofstream ofs{wstrSavePath};

	ofs << SaveJson.dump(4);

	ofs.close();

	return S_OK;
}

HRESULT CUEMapdataParser::Save_FilteringRawMapData(const wchar_t* wszFilePath)
{
	vector<UE_MAP_DATA>* pFind = Get_Unreal_MapData(wszFilePath);


	if (pFind == nullptr) return E_FAIL;

	path FilePath{ wszFilePath };

	wstring wstrFileName = path(FilePath).filename().stem();
	wstrFileName += L"_Filtering.json";
	wstring wstrSavePath = FilePath.remove_filename();
	wstrSavePath += wstrFileName;

	json SaveJson = json::array();

	for (auto& Converted_MapData : *pFind)
		SaveJson.push_back(Converted_MapData);

	std::ofstream ofs{ wstrSavePath };

	ofs << SaveJson.dump(4);

	ofs.close();

	return S_OK;
}


void CUEMapdataParser::Free()
{
	Super::Free();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}

NS_BEGIN(Tool)

#pragma region   To From Json   :   [Unreal Map Raw Data] 

void to_json(json& SaveJson, const UE_OBJECT_INFO& tData)
{
	SaveJson["ObjectName"] = tData.strObjectName;
	SaveJson["ObjectPath"] = tData.strObjectPath;

	return;
}

void from_json(const json& LoadJson, UE_OBJECT_INFO& tData)
{
	tData.strObjectName = LoadJson.value("ObjectName", "");
	tData.strObjectPath = LoadJson.value("ObjectPath", "");
}

void to_json(json& SaveJson, const UE_OVERRIDEMATERIALS& tData)
{
	for (size_t i = 0; i < tData.vecObjectInfo.size(); ++i)
	{
		if (tData.isNull[i])
			SaveJson.push_back(json::object());
		else
		{
			json Object{};
			Object = tData.vecObjectInfo[i];
			SaveJson.push_back(Object);
		}
	}
}

void from_json(const json& LoadJson, UE_OVERRIDEMATERIALS& tData)
{
	tData.vecObjectInfo.resize(LoadJson.size());
	tData.isNull.resize(LoadJson.size());
	_uint i{0};
	for (auto& ObjectInfoJson : LoadJson)
	{
		if (ObjectInfoJson.empty())
			tData.isNull[i++] = true;
		else
			tData.vecObjectInfo[i++] = ObjectInfoJson;
	}
}

void to_json(json& SaveJson, const UE_TRANSFORMDATA& tData)
{
	write_vec3_xyz(SaveJson["Scale3D"], tData.vScale3D);
	SaveJson["Rotation"]["X"] = tData.vRotation.x;
	SaveJson["Rotation"]["Y"] = tData.vRotation.y;
	SaveJson["Rotation"]["Z"] = tData.vRotation.z;
	SaveJson["Rotation"]["W"] = tData.vRotation.w;
	write_vec3_xyz(SaveJson["Translation"], tData.vTranslation);

	return;
}

void from_json(const json& LoadJson, UE_TRANSFORMDATA& tData)
{
	const json& Translation_Json = LoadJson;

	if(Translation_Json.contains("Scale3D"))
		read_vec3_defaultscale(Translation_Json["Scale3D"], tData.vScale3D);
	if (Translation_Json.contains("Rotation"))
		read_vec4_Quat(Translation_Json["Rotation"], tData.vRotation);
	if (Translation_Json.contains("Translation"))
		read_vec3_xyz(Translation_Json["Translation"], tData.vTranslation);

	return;
}

void to_json(json& SaveJson, const UE_PER_INSTANCE_SM_DATA& tData)
{
	if (tData.isNull)
	{
		json Object;
		SaveJson = Object;
	}
	else
	{
		SaveJson["TransformData"] = tData.tTransformData;
	}
	return;
}

void from_json(const json& LoadJson, UE_PER_INSTANCE_SM_DATA& tData)
{
	if (LoadJson.contains("TransformData"))
		tData.tTransformData = LoadJson["TransformData"];
}

void to_json(json& SaveJson, const UE_PROPERTIES& tData)
{
	SaveJson["InstancingRandomSeed"] = tData.iInstancingRandomSeed;

	SaveJson["StaticMesh"] = tData.tStaticMesh;
	SaveJson["AttachParent"] = tData.tAttachParent;
	SaveJson["OverrideMaterials"] = tData.tOverrideMaterials;

	write_vec3_xyz(SaveJson["RelativeLocation"], tData.vRelativeLocation);
	write_vec3_PitchYawRoll(SaveJson["RelativeRotation"], tData.vRelativeRotation);
	write_vec3_xyz(SaveJson["RelativeScale3D"], tData.vRelativeScale);
}

void from_json(const json& LoadJson, UE_PROPERTIES& tData)
{
	const json& Properties_Json = LoadJson;

	tData.iInstancingRandomSeed = Properties_Json.value("InstancingRandomSeed",0);

	if (Properties_Json.contains("StaticMesh"))
		tData.tStaticMesh = Properties_Json["StaticMesh"];
	if (Properties_Json.contains("AttachParent"))
		tData.tAttachParent = Properties_Json["AttachParent"];
	if (Properties_Json.contains("OverrideMaterials"))
		tData.tOverrideMaterials = Properties_Json["OverrideMaterials"];


	if (Properties_Json.contains("RelativeLocation"))
	read_vec3_xyz(Properties_Json["RelativeLocation"], tData.vRelativeLocation);
	if (Properties_Json.contains("RelativeRotation"))
	read_vec3_PitchYawRoll(Properties_Json["RelativeRotation"], tData.vRelativeRotation);
	if (Properties_Json.contains("RelativeScale3D"))
		read_vec3_defaultscale(Properties_Json["RelativeScale3D"], tData.vRelativeScale);

	return;
}

void to_json(json& SaveJson, const UE_MAP_DATA& tData)
{
	SaveJson["Type"]  =  tData.strType;
	SaveJson["Name"]  =  tData.strType;
	SaveJson["Outer"] =  tData.strType;

	SaveJson["Properties"] = tData.tProperties;
	SaveJson["PerInstanceSMData"] = tData.vecPerInstanceSMData;
	return;
}

void from_json(const json& LoadJson , UE_MAP_DATA& tData)
{
	tData.strType  = LoadJson.value("Type","");
	tData.strName  = LoadJson.value("Name","");
	tData.strOuter = LoadJson.value("Outer","");

	if (LoadJson.contains("Properties"))
		tData.tProperties = LoadJson["Properties"];

	if (LoadJson.contains("PerInstanceSMData"))
	{
		tData.vecPerInstanceSMData = LoadJson["PerInstanceSMData"].get<vector<UE_PER_INSTANCE_SM_DATA>>();
	}
}

#pragma endregion
#pragma region To From Json : [Converted Map Data]

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
void to_json(json& SaveJson, const USING_MODEL_INFO& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);

	auto& Material_Json = SaveJson["Mateiral Info"];

	for (auto& Material_Info : tData.vecMaterialInfo)
	{
		if (Material_Info.isNull == true)
			continue;
		Material_Json.push_back(Material_Info);
	}
}
void to_json(json& SaveJson, const CONVERTED_MAPDATA& tData)
{
	SaveJson["Type"] = StaticModelType_ToString(tData.eType);

	write_vec3_xyz(SaveJson["SRT"]["Scale"],tData.vScale);
	write_vec3_xyz(SaveJson["SRT"]["Position"],tData.vPosition);
	write_vec3_PitchYawRoll(SaveJson["SRT"]["Rotation"],tData.vPitchYawRoll);

	SaveJson["Model Info"] = tData.tUsingModelInfo;
}

#pragma endregion


void read_vec3_defaultscale(const json& _j, Vec3& vOut)
{
	vOut.x = _j.value("X", 1.f);
	vOut.y = _j.value("Y", 1.f);
	vOut.z = _j.value("Z", 1.f);
}
void read_vec3_xyz(const json& _j, Vec3& vOut)
{
	vOut.x = _j.value("X", 0.f);
	vOut.y = _j.value("Y", 0.f);
	vOut.z = _j.value("Z", 0.f);
}
void read_vec3_PitchYawRoll(const json& _j, Vec3& vOut)
{
	vOut.x = _j.value("Pitch", 0.f);
	vOut.y = _j.value("Yaw", 0.f);
	vOut.z = _j.value("Roll", 0.f);
}
void read_vec4_Quat(const json& _j, Quat& vOut)
{
	vOut.x = _j.value("X", 0.f);
	vOut.y = _j.value("Y", 0.f);
	vOut.z = _j.value("Z", 0.f);
	vOut.w = _j.value("W", 0.f);
}
void write_vec3_xyz(json& _j, const Vec3& vOut)
{
	_j["X"] = vOut.x;
	_j["Y"] = vOut.y;
	_j["Z"] = vOut.z;
}
void write_vec3_PitchYawRoll(json& _j, const Vec3& vOut)
{
	_j["Pitch"] = vOut.x;
	_j["Yaw"] = vOut.y;
	_j["Roll"] = vOut.z;
}
void write_vec4_Quat(json& _j, const Quat& vOut)
{
	_j["X"] = vOut.x;
	_j["Y"] = vOut.y;
	_j["Z"] = vOut.z;
	_j["W"] = vOut.w;
}


NS_END

