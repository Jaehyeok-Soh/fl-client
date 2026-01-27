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

	bool isFilering_Type{ true };
	bool isFilering_Name{ true };
	for (auto& strFilter : m_vecTypeFilter)
	{
		isFilering_Type = true;
		isFilering_Name = true;

		if (strType == strFilter)
			isFilering_Type = false;
		if (strName.find(strFilter) != string::npos)
			isFilering_Name = false;

		if (!isFilering_Type && !isFilering_Name)
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
		Change_ModelPath(tConvertedData.tUsingModelInfo.wstrName, tConvertedData.tUsingModelInfo.wstrPath);

		if (tConvertedData.tUsingModelInfo.wstrPath == L"Map/EM/Content/Maps/Datas/Data_Art/Ailixian/Ailixian_Train01_Art.fbx")
			int a = 0;

		for (auto& Material : tUEMapData.tProperties.tOverrideMaterials.vecObjectInfo)
		{
			if (Material.strObjectName.empty())
				continue;
			tConvertedData.tUsingModelInfo.vecMaterialInfo.push_back({ false ,Engine_Utils::ToWString(Material.strObjectName) , Engine_Utils::ToWString(Material.strObjectPath) });
		}

		if(tConvertedData.eType == EStaticModel_Type::DEFUALT)
		{
			tConvertedData.vScale		 = tUEMapData.tProperties.vRelativeScale;
			tConvertedData.vPitchYawRoll = tUEMapData.tProperties.vRelativeRotation;
			tConvertedData.vPosition     = tUEMapData.tProperties.vRelativeLocation;
			Change_SRT(&tConvertedData.vScale, &tConvertedData.vPitchYawRoll, &tConvertedData.vPosition, tConvertedData.eType);
			//std::swap(tConvertedData.vPitchYawRoll.y,tConvertedData.vPitchYawRoll.z);
			vecConvertedData.push_back(tConvertedData);
		}
		else if (tConvertedData.eType == EStaticModel_Type::INSTANCE)
		{
			for (auto& PerInstanceSMData : tUEMapData.vecPerInstanceSMData)
			{
				if (PerInstanceSMData.isNull == true)
					continue;
				tConvertedData.vPosition = PerInstanceSMData.tTransformData.vTranslation;
				tConvertedData.vScale = PerInstanceSMData.tTransformData.vScale3D;
				std::swap(PerInstanceSMData.tTransformData.vRotation.y, PerInstanceSMData.tTransformData.vRotation.z);
				tConvertedData.vPitchYawRoll = PerInstanceSMData.tTransformData.vRotation.ToEuler() * To_DEGREE;
				Change_SRT(&tConvertedData.vScale,&tConvertedData.vPitchYawRoll,&tConvertedData.vPosition, tConvertedData.eType);

				/* SRT 변환 이후 Push Back */
				//vecConvertedData.push_back(tConvertedData);
			}
		}
	}

	return vecConvertedData;
}

void CUEMapdataParser::Change_SRT(Vec3* vScale, Vec3* vPitchYawRoll, Vec3* vPosition , EStaticModel_Type eType)
{	
	if(vScale)
		std::swap(vScale->y, vScale->z);
	if (vPitchYawRoll)
		vPitchYawRoll = vPitchYawRoll;
	if (vPosition)
	{
		Vec3 vSwap = Vec3( vPosition->x * m_fMulScale , vPosition->z * m_fMulScale , vPosition->y * (-m_fMulScale) );
		*vPosition = vSwap;
	}
}

void CUEMapdataParser::Change_ModelPath(OUT _wstring& wstrModelName, OUT _wstring& wstrModelPath)
{
	wstring wstrName = wstrModelName;
	wstring wstrPath = wstrModelPath;


	size_t Pos_Point = wstrPath.rfind(L".");
	if (Pos_Point != std::string::npos)
		wstrPath.replace(Pos_Point,wstrPath.length(),g_wszModelExtension);

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

		wstrTarget = L"BasicShapes";
		Engine_Utils::Add_Text(wstrPath  , wstrTarget , L"/Model" , wstrTarget.length());
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

	wstrTarget = L"Mesh";
	size_t Pos_Target = wstrPath.find(wstrTarget);
	if (Pos_Target != std::string::npos)
	{
		Pos_Target += wstrTarget.length();
		wstrPath.insert(Pos_Target, L"/Model");
	}

	wstrModelPath = wstrPath;
	wstrModelName = path(wstrPath).filename().stem().wstring();
	wstrModelPath = L"Map/" + wstrModelPath;
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
	wstring MapDataPath = wszUERawDataJsonFile;
	
	vector<UE_MAP_DATA>* vecUEData = Get_Unreal_MapData(wszUERawDataJsonFile);


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
		if (Filter(UE_Map_Data_Json.value("Name", ""), UE_Map_Data_Json.value("Type", ""))) continue;

		UE_MAP_DATA tData{};
		tData = UE_Map_Data_Json;
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
		CStaticModel::STATICMODEL_DESC desc = {};
		desc.wstrLayerTag = g_wszStaticModelLayer;
		desc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
		desc.wstrModelPath = CONVERTED_MAPDATA.tUsingModelInfo.wstrPath;
		desc.wstrModelName = CONVERTED_MAPDATA.tUsingModelInfo.wstrName;

		CTransform::TRANSFORM_DESC tTramsoformDesc{};
		tTramsoformDesc.vScale				= CONVERTED_MAPDATA.vScale;
		tTramsoformDesc.vRotation_Degrees	= CONVERTED_MAPDATA.vPitchYawRoll;
		tTramsoformDesc.vPosition			= CONVERTED_MAPDATA.vPosition;

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

	wstring wstrSavePath{FilePath};

	Engine_Utils::Add_Text(wstrSavePath,L".",L"_Converted", false , -1 );

	json SaveJson = json::array();


	for (auto& Converted_MapData : *pFind)
		SaveJson.push_back(Converted_MapData);

	std::ofstream ofs{wstrSavePath};

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

void to_json(json& SaveJson, const UE_OBJECT_INFO& tData)
{

}

void from_json(const json& LoadJson, UE_OBJECT_INFO& tData)
{
	tData.strObjectName = LoadJson.value("ObjectName", "");
	tData.strObjectPath = LoadJson.value("ObjectPath", "");
}

void to_json(json& SaveJson, const UE_OVERRIDEMATERIALS& tData)
{

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

	return;
}

void from_json(const json& LoadJson, UE_PER_INSTANCE_SM_DATA& tData)
{
	if (LoadJson.contains("TransformData"))
		tData.tTransformData = LoadJson["TransformData"];
}

void to_json(json& SaveJson, const UE_PROPERTIES& tData)
{

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



#pragma region Converted Map Data 



void to_json(json& SaveJson, const USING_MATERIAL_INFO& tData)
{
	SaveJson["Name"] = Engine_Utils::ToString(tData.wstrName);
	SaveJson["Path"] = Engine_Utils::ToString(tData.wstrPath);
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

NS_END

