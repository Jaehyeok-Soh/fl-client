#include "pch.h"
#include "UEMapdataParser.h"
#include <fstream>
#include "Engine_Utils.h"
#include "Model.h"
#include "StaticModel.h"
#include "GameInstance.h"
#include "MapToolManager.h"

IMPLEMENT_SINGLETON(CUEMapdataParser)


CUEMapdataParser::CUEMapdataParser()
{
	m_vecTypeFilter = { "StaticMeshComponent" , "InstancedStaticMeshComponent" };

	m_umapConvertedMapData.clear();
	m_umapUnreal_Map_Data.clear();
}

HRESULT CUEMapdataParser::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	//"InstancedStaticMeshComponent"
	m_vecTypeFilter = { "StaticMeshComponent"  };
	m_vecMtlTextureFilter = { "PM_Diffuse","PM_Normals","PM_SpecularMasks","PM_Emissive"};

	m_vecOuterFilter = {"LOD"};
	m_umapConvertedMapData.clear();
	m_umapUnreal_Map_Data.clear();

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = CGameInstance::GetInstance();
	m_pMapToolManager = CMapToolManager::GetInstance();
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


vector<MAPDATA_BASE*> CUEMapdataParser::Convert_UE_MapData(const vector<UE_MAP_DATA>& tData)
{
	vector<MAPDATA_BASE*> vecConvertedData{};
	vector<UE_MAP_DATA>		 vecUEMapData = tData;
	if (tData.empty()) return vecConvertedData;

	vecConvertedData.reserve(tData.size());


	for (_uint i = 0; i < ENUM_TO_UINT(tData.size()); ++i)
	{
		MAPDATA_BASE*	 pMapDataBase{ nullptr };
		UE_MAP_DATA		 tUEMapData{};

		tUEMapData = vecUEMapData[i];

		EMapObject_Type eType = tUEMapData.Type_ToEnum();

		if (eType == EMapObject_Type::STATICMODEL)
		{
			STATICMODEL_DATA* pStaticModel_Data = new STATICMODEL_DATA;
			pStaticModel_Data->eMapObjectType = eType;
			pStaticModel_Data->tUsingModelInfo.wstrName = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectName);
			pStaticModel_Data->tUsingModelInfo.wstrPath = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectPath);

			for (auto& Material : tUEMapData.tProperties.tOverrideMaterials.vecObjectInfo)
			{
				if (Material.strObjectName.empty())
					continue;
				pStaticModel_Data->tUsingModelInfo.vecOverrideMaterial.push_back({ false ,Engine_Utils::ToWString(Material.strObjectName) , Engine_Utils::ToWString(Material.strObjectPath) });
			}
			Change_UsingModelInfo(pStaticModel_Data->tUsingModelInfo);

			pStaticModel_Data->tOriginSRT.vScale = tUEMapData.tProperties.vRelativeScale;
			pStaticModel_Data->tOriginSRT.vPosition = tUEMapData.tProperties.vRelativeLocation;

#pragma region Change Quat
			float pitch = XMConvertToRadians(tUEMapData.tProperties.vRelativeRotation.x) * 0.5f;
			float yaw = XMConvertToRadians(tUEMapData.tProperties.vRelativeRotation.y) * 0.5f;
			float roll = XMConvertToRadians(tUEMapData.tProperties.vRelativeRotation.z) * 0.5f;

			const float SP = sinf(pitch), CP = cosf(pitch);
			const float SY = sinf(yaw), CY = cosf(yaw);
			const float SR = sinf(roll), CR = cosf(roll);

			Quat q;
			q.x = CR * SP * SY - SR * CP * CY;
			q.y = -CR * SP * CY - SR * CP * SY;
			q.z = CR * CP * SY - SR * SP * CY;
			q.w = CR * CP * CY + SR * SP * SY;

			if (q.w < 0.f) q = Quat(-q.x, -q.y, -q.z, -q.w);
#pragma endregion
			pStaticModel_Data->tOriginSRT.vQuat = q;
			Change_SRT(pStaticModel_Data->tOriginSRT);
			pMapDataBase = pStaticModel_Data;
			vecConvertedData.push_back(pMapDataBase);
		}
		//else if (eType == EMapObject_Type::INSTANCEMODEL)
		//{
		//	INSTANCEMODEL_DATA* pInstanceModel_Data = new INSTANCEMODEL_DATA;
		//	pInstanceModel_Data->eMapObjectType = eType;
		//	pInstanceModel_Data->tUsingModelInfo.wstrName = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectName);
		//	pInstanceModel_Data->tUsingModelInfo.wstrPath = Engine_Utils::ToWString(tUEMapData.tProperties.tStaticMesh.strObjectPath);

		//	for (auto& Material : tUEMapData.tProperties.tOverrideMaterials.vecObjectInfo)
		//	{
		//		if (Material.strObjectName.empty())
		//			continue;
		//		pInstanceModel_Data->tUsingModelInfo.vecMaterialInfo.push_back({ false ,Engine_Utils::ToWString(Material.strObjectName) , Engine_Utils::ToWString(Material.strObjectPath) });
		//	}
		//	Change_UsingModelInfo(pInstanceModel_Data->tUsingModelInfo);


		//	for (auto& PerInsData : tUEMapData.vecPerInstanceSMData)
		//	{
		//		SRT_DATA tSRTData{};

		//		if (PerInsData.isNull == true) continue;

		//		tSRTData.vScale = PerInsData.tTransformData.vScale3D;
		//		tSRTData.vPosition = PerInsData.tTransformData.vTranslation;
		//		Quat qV = PerInsData.tTransformData.vRotation;
		//		if (qV.w < 0.f) qV = Quat(-qV.x, -qV.y, -qV.z, -qV.w);
		//		tSRTData.vQuat = qV;
		//		Change_SRT(tSRTData);
		//		pInstanceModel_Data->vecOriginSRT.push_back(tSRTData);
		//	}
		//	pMapDataBase = pInstanceModel_Data;
		//}

	}

	return vecConvertedData;
}

void CUEMapdataParser::Change_SRT(OUT SRT_DATA& tSRT_Data)
{
	Matrix ScaleMatrix		= Matrix::CreateScale(tSRT_Data.vScale);
	Matrix TransMatrix		= Matrix::CreateTranslation(tSRT_Data.vPosition * m_fMulScale);
	Matrix RotationMatrix	= Matrix::CreateFromQuaternion(tSRT_Data.vQuat);

	Matrix InvB = m_ChangeMatrix.Transpose();


	Matrix WorldMatrix = ScaleMatrix *  RotationMatrix * TransMatrix;
	WorldMatrix = InvB * WorldMatrix * m_ChangeMatrix;

	WorldMatrix.Decompose(tSRT_Data.vScale, tSRT_Data.vQuat, tSRT_Data.vPosition);
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


	for (auto& OverrideMateral : tUsingModelInfo.vecOverrideMaterial)
	{
		Change_OverrideMaterials(OverrideMateral);
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

void CUEMapdataParser::Change_OverrideMaterials(OUT OVERRIDE_MATERIALS& tOverrideMtl)
{
	if (tOverrideMtl.isNull == true) return;

	tOverrideMtl.vecUsingTextureInfo;

	/* 임마를 다시 변환 시켜줘야 함 */
	tOverrideMtl.wstrMtl_JsonFile_Path;

	Change_ObjectPath(tOverrideMtl.wstrMtl_JsonFile_Name, tOverrideMtl.wstrMtl_JsonFile_Path,EObject_Path_Type::Mtl);
	Change_UsingMaterialTexturePath(tOverrideMtl);
}

void CUEMapdataParser::Change_UsingMaterialTexturePath(OUT OVERRIDE_MATERIALS& tUsingMtlInfo)
{
	if (tUsingMtlInfo.isNull == true) return;

	std::ifstream is (tUsingMtlInfo.wstrMtl_JsonFile_Path);

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

vector<MAPDATA_BASE*>* CUEMapdataParser::Get_Converted_MapData(const wstring& FindKey)
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
	vector<MAPDATA_BASE*>* pFind = Get_Converted_MapData(wszFileName);
	if (pFind == nullptr) return E_FAIL;

	UINT iLevelID = ENUM_TO_UINT(ELevelType::MAP);

	CGameObject* pResult{nullptr};



	for (auto& CONVERTED_MAPDATA : *pFind)
	{
		EMapObject_Type& eMapObjectType = CONVERTED_MAPDATA->eMapObjectType;

		if (eMapObjectType == EMapObject_Type::STATICMODEL)
		{
			CStaticModel::STATICMODEL_DESC desc{};
			desc.wstrLayerTag = g_wszStaticModelLayer;
			desc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
			desc.isLoaded = true;
			desc.tData = *static_cast<STATICMODEL_DATA*>(CONVERTED_MAPDATA);
			
			CTransform::TRANSFORM_DESC tTsDesc{};
			tTsDesc.ScaleMatrix = Matrix::CreateScale(desc.tData.tOriginSRT.vScale);
			tTsDesc.RotationMatrix = Matrix::CreateFromQuaternion(desc.tData.tOriginSRT.vQuat);
			tTsDesc.TranslationMatrix = Matrix::CreateTranslation(desc.tData.tOriginSRT.vPosition);
			desc.pTransform_Desc = &tTsDesc;

			m_pMapToolManager->Make_MapObject(eMapObjectType, &desc);
		}
		else if(eMapObjectType == EMapObject_Type::INSTANCEMODEL)
		{

		}
	}

	return S_OK;
}

HRESULT CUEMapdataParser::Save_ConvertedRawMapData(const wchar_t* wszFilePath)
{
	vector<MAPDATA_BASE*>* pFind = Get_Converted_MapData(wszFilePath);

	if (pFind == nullptr) return E_FAIL;

	path FilePath{ wszFilePath };
	wstring wstrFileName = path(FilePath).filename().stem();
	wstrFileName += L"_Converted.json";
	wstring wstrSavePath = FilePath.remove_filename();
	wstrSavePath += wstrFileName;

	json SaveJson = json::array();

	for (auto& Converted_MapData : *pFind)
	{
		EMapObject_Type eType = Converted_MapData->eMapObjectType;
		if (eType == EMapObject_Type::STATICMODEL)
		{
			json js{};
			STATICMODEL_DATA pData = *static_cast<STATICMODEL_DATA*>(Converted_MapData);
			to_json(js,pData);
			SaveJson.push_back(js);
		}
		else if (eType == EMapObject_Type::INSTANCEMODEL)
		{
			//SaveJson.push_back(*static_cast<INSTANCEMODEL_DATA*>(Converted_MapData));
		}
	}

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

	for (auto& Pair : m_umapConvertedMapData)
	{
		for (auto& Data : Pair.second)
			Safe_Delete(Data);
	}


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	m_pMapToolManager = nullptr;
}

NS_BEGIN(Tool)

#pragma region   To From Json   :   [Unreal Map Raw Data] 

#pragma region UE OBJECT INFO
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
#pragma endregion

#pragma region UE OVERRIDEMATERIALS
void to_json(json& SaveJson, const UE_OVERRIDEMATERIALS& tData)
{
	/* 그래서 저장하기전에 isNull 변수에 true false 를 먼저 체크한뒤 없으면 null을 pushBack 해주고 있으면 값을 push back해준다 */
	
	for (size_t i = 0; i < tData.vecObjectInfo.size(); ++i)
	{
		if (tData.isNull[i])
			SaveJson.push_back(nullptr);
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
	/* 읽어올떄 Size 만큼 할당을 하게된다 . 들어오기 전에 미리 키값이 있는지 없는지 판다을하고 들어오게 되고
		들어오고 나서는 배열의 size 만큼 배열을 부여하고 배열크기를 그대록 가져가야 배열의 순번을 유지하며 저장할 수 있다. */
	tData.vecObjectInfo.resize(LoadJson.size());
	tData.isNull.resize(LoadJson.size());


	_uint i{0};
	for (auto& ObjectInfoJson : LoadJson)
	{
		if (ObjectInfoJson.is_null())
			tData.isNull[i++] = true;
		else
			tData.vecObjectInfo[i++] = ObjectInfoJson;
	}
}
#pragma endregion

#pragma region UE TRANSFROMDATA
void to_json(json& SaveJson, const UE_TRANSFORMDATA& tData)
{
	Engine_Utils::write_vec3_xyz(SaveJson["Scale3D"], tData.vScale3D);
	SaveJson["Rotation"]["X"] = tData.vRotation.x;
	SaveJson["Rotation"]["Y"] = tData.vRotation.y;
	SaveJson["Rotation"]["Z"] = tData.vRotation.z;
	SaveJson["Rotation"]["W"] = tData.vRotation.w;
	Engine_Utils::write_vec3_xyz(SaveJson["Translation"], tData.vTranslation);

	return;
}

void from_json(const json& LoadJson, UE_TRANSFORMDATA& tData)
{
	const json& Translation_Json = LoadJson;

	if(Translation_Json.contains("Scale3D"))
		Engine_Utils::read_vec3_xyz(Translation_Json["Scale3D"], tData.vScale3D);
	if (Translation_Json.contains("Rotation"))
		Engine_Utils::read_vec4_Quat(Translation_Json["Rotation"], tData.vRotation);
	if (Translation_Json.contains("Translation"))
		Engine_Utils::read_vec3_xyz(Translation_Json["Translation"], tData.vTranslation);

	return;
}
#pragma endregion

#pragma region PER INSTANCE SM DATA
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
#pragma endregion

#pragma region UE PROPERTIES
void to_json(json& SaveJson, const UE_PROPERTIES& tData)
{
	SaveJson["InstancingRandomSeed"] = tData.iInstancingRandomSeed;

	SaveJson["StaticMesh"] = tData.tStaticMesh;
	SaveJson["AttachParent"] = tData.tAttachParent;

	if(!tData.tOverrideMaterials.vecObjectInfo.empty())
		SaveJson["OverrideMaterials"] = tData.tOverrideMaterials;

	Engine_Utils::write_vec3_xyz(SaveJson["RelativeLocation"], tData.vRelativeLocation);
	Engine_Utils::write_vec3_PitchYawRoll(SaveJson["RelativeRotation"], tData.vRelativeRotation);
	Engine_Utils::write_vec3_xyz(SaveJson["RelativeScale3D"], tData.vRelativeScale);
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
		Engine_Utils::read_vec3_xyz(Properties_Json["RelativeLocation"], tData.vRelativeLocation);
	if (Properties_Json.contains("RelativeRotation"))
		Engine_Utils::read_vec3_PitchYawRoll(Properties_Json["RelativeRotation"], tData.vRelativeRotation);
	if (Properties_Json.contains("RelativeScale3D"))
		Engine_Utils::read_vec3_xyz(Properties_Json["RelativeScale3D"], tData.vRelativeScale);

	return;
}
#pragma endregion

#pragma region UE MAP DATA
void to_json(json& SaveJson, const UE_MAP_DATA& tData)
{
	SaveJson["Type"]  =  tData.strType;
	SaveJson["Name"]  =  tData.strType;
	SaveJson["Outer"] =  tData.strOuter;

	SaveJson["Properties"] = tData.tProperties;
	if(!tData.vecPerInstanceSMData.empty())
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

#pragma endregion

NS_END

