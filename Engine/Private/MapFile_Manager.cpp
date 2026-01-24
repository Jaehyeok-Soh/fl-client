#include "Engine_pch.h"
#include "MapFile_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"

NLOHMANN_JSON_SERIALIZE_ENUM(EMaterialInstanceType, {
	{EMaterialInstanceType::Default, "Default"},
	{EMaterialInstanceType::Concrete, "Concrete"},
	{EMaterialInstanceType::Mirror, "Mirror"},
	{EMaterialInstanceType::Water, "Water"},
	{EMaterialInstanceType::Dirt, "Dirt"},
	{EMaterialInstanceType::Red, "Red"},
	{EMaterialInstanceType::Blue, "Blue"},
	{EMaterialInstanceType::Green, "Green"},
	{EMaterialInstanceType::Grass, "Grass"},
	{EMaterialInstanceType::Orange, "Orange"},
	{EMaterialInstanceType::Brown, "Brown" },
	{EMaterialInstanceType::Pupple, "Pupple" },
	{EMaterialInstanceType::WinterGreen, "WinterGreen" },
	{EMaterialInstanceType::BurnishedBrown, "BurnishedBrown" },
	{EMaterialInstanceType::ConcreteLight, "ConcreteLight" },
	{EMaterialInstanceType::ConcreteMid, "ConcreteMid" },
	{EMaterialInstanceType::ConcreteWarm, "ConcreteWarm" },
	{EMaterialInstanceType::MetalCool, "MetalCool" },
	{EMaterialInstanceType::MetalDark, "MetalDark" },
	{EMaterialInstanceType::CourtBlue, "CourtBlue" }}
)
NLOHMANN_JSON_SERIALIZE_ENUM(LIGHT_TYPE, {
		{LIGHT_TYPE::DIRECTIONAL, "DIRECTIONAL"},
		{LIGHT_TYPE::STATICPOINT, "STATIC_POINT"},
		{LIGHT_TYPE::DYNAMICPOINT, "DYNAMIC_POINT"}}
)


CMapFile_Manager::CMapFile_Manager()
{
}

CMapFile_Manager::~CMapFile_Manager()
{
}

HRESULT CMapFile_Manager::SaveData(const wstring& wstrSavePath, const MAPFILE_DATA& data)
{
	CFileUtils *pFileUtil = CFileUtils::Create();
	if (FAILED(pFileUtil->Open(wstrSavePath, FileMode::WRITE)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, open failed");
		return E_FAIL;
	}

	json j = data;
	std::string text = j.dump(4);

	if (FAILED(pFileUtil->WriteAllText(text)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, write failed");
		return E_FAIL;
	}

	Safe_Release(pFileUtil);
	return S_OK;
}

HRESULT CMapFile_Manager::LoadData(const wstring& wstrFilePath, OUT MAPFILE_DATA& outData)
{
	CFileUtils* pFileUtil = CFileUtils::Create();
	if (FAILED(pFileUtil->Open(wstrFilePath, FileMode::READ)))
	{
		MSG_BOX("CMapFile_Manager::LoadData, open failed");
		return E_FAIL;
	}

	std::string text;
	if (FAILED(pFileUtil->ReadAllText(text)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, read failed");
		return E_FAIL;
	}

	json j = json::parse(text);
	outData = j.get<MAPFILE_DATA>();

	Safe_Release(pFileUtil);
	return S_OK;
}

NS_BEGIN(Engine)

void to_json(json& j, const TRANSFORM_SAVEDATA& data)
{
	j = json
	{
		{"Position", {data.vPos.x, data.vPos.y, data.vPos.z}},
		{"Quaternion", {data.vQuaternion.x, data.vQuaternion.y, data.vQuaternion.z, data.vQuaternion.w }},
		{"Scale", {data.vScale.x, data.vScale.y, data.vScale.z}}
	};
}

void from_json(const json& j, TRANSFORM_SAVEDATA& data)
{
	auto position = j.value("Position", vector<_float>{0.f, 0.f, 0.f});
	auto quaternion = j.value("Quaternion", vector<_float>{0.f, 0.f, 0.f, 0.f});
	auto scale = j.value("Scale", vector<_float>{1.f, 1.f, 1.f});

	if (position.size() == 3) { data.vPos = { position[0], position[1], position[2] }; }
	if (quaternion.size() == 4) { data.vQuaternion = { quaternion[0], quaternion[1], quaternion[2], quaternion[3]}; }
	if (scale.size() == 3) { data.vScale = { scale[0], scale[1], scale[2] }; }
}

void to_json(json& j, const TRIGGERBOX_SAVEDATA& data)
{
	j = json
	{
		{"PolygonName", data.strPolygonName},
	};

	{
		json vecJson = json::array();
		for (const auto& v : data.vecMonsterNames)
			vecJson.push_back(v);
		j["MonsterTags"] = std::move(vecJson);
	}

	{
		json vecJson = json::array();
		for (const auto& v : data.vecPositions)
			vecJson.push_back({ v.x, v.y, v.z });
		j["Positions"] = std::move(vecJson);
	}
}

void from_json(const json& j, TRIGGERBOX_SAVEDATA& data)
{
	j.at("PolygonName").get_to(data.strPolygonName);

	{
		data.vecMonsterNames.clear();
		auto vecJson = j.value("MonsterTags", vector<string>());
		data.vecMonsterNames.resize(vecJson.size());
		for (size_t i = 0; i < vecJson.size(); ++i)
		{
			data.vecMonsterNames[i] = vecJson[i];
		}
	}

	data.vecPositions.clear();
	auto verts = j.value("Positions", vector<vector<_float>>());

	data.vecPositions.reserve(verts.size());
	for (const auto& v : verts)
	{
		if (v.size() == 3)
		{
			data.vecPositions.push_back(Vec3(v[0], v[1], v[2]));
		}
	}
}

void to_json(json& j, const MODEL_SAVEDATA& data)
{
	j = json
	{
		{"FileName", data.fileName},
	};

	{
		json vecJson = json::array();
		for (const auto& v : data.vecMaskTextureTags)
			vecJson.push_back(v);
		j["MaskTextures"] = std::move(vecJson);
	}

	{
		json vecJson = json::array();
		for (const auto& v : data.vecMI)
			vecJson.push_back(v);
		j["MaterialInstances"] = std::move(vecJson);
	}

	{
		json vecJson = json::array();
		for (const auto& v : data.vecShaderPassesByMesh)
			vecJson.push_back(v);
		j["PassesByMesh"] = std::move(vecJson);
	}
}

void from_json(const json& j, MODEL_SAVEDATA& data)
{
	j.at("FileName").get_to(data.fileName);

	{
		data.vecMaskTextureTags.clear();
		auto vecJson = j.value("MaskTextures", vector<string>());
		data.vecMaskTextureTags.resize(vecJson.size());
		for (size_t i = 0; i < vecJson.size(); ++i)
		{
			data.vecMaskTextureTags[i] = vecJson[i];
		}
	}

	{
		data.vecMI.clear();
		auto vecJson = j.value("MaterialInstances", vector<EMaterialInstanceType>());
		data.vecMI.resize(vecJson.size());
		for (size_t i = 0; i < vecJson.size(); ++i)
		{
			data.vecMI[i] = vecJson[i];
		}
	}
	
	{
		data.vecShaderPassesByMesh.clear();
		auto vecJson = j.value("PassesByMesh", vector<_int>());
		data.vecShaderPassesByMesh.resize(vecJson.size());
		for (size_t i = 0; i < vecJson.size(); ++i)
		{
			data.vecShaderPassesByMesh[i] = vecJson[i];
		}
	}
}

void to_json(json& j, const COLLIDER_SAVEDATA& data)
{
	j = json
	{
		{"Shape", data.shape},
		{"Radius", data.fRadius},
		{"Center", {data.vCenter.x, data.vCenter.y, data.vCenter.z}},
		{"Extents", {data.vExtents.x, data.vExtents.y, data.vExtents.z}},
		{"Rotation", {data.vEuler.x, data.vEuler.y, data.vEuler.z}}
	};
}

void from_json(const json& j, COLLIDER_SAVEDATA& data)
{
	j.at("Shape").get_to(data.shape);
	j.at("Radius").get_to(data.fRadius);

	auto center = j.value("Center", vector<_float>{0.f, 0.f, 0.f});
	auto extents = j.value("Extents", vector<_float>{0.f, 0.f, 0.f});
	auto rotation = j.value("Rotation", vector<_float>{0.f, 0.f, 0.f});

	if (center.size() == 3) { data.vCenter = { center[0], center[1], center[2] }; }
	if (extents.size() == 3) { data.vExtents = { extents[0], extents[1], extents[2] }; }
	if (rotation.size() == 3) { data.vEuler = { rotation[0], rotation[1], rotation[2] }; }
}

void to_json(json& j, const CELL_SAVEDATA& data)
{
	j = json
	{
		{"Index", data.iIndex},
		{"PointA", {data.arrPoints[ENUM_TO_UINT(EPOINT::A)].x, data.arrPoints[ENUM_TO_UINT(EPOINT::A)].y, data.arrPoints[ENUM_TO_UINT(EPOINT::A)].z}},
		{"PointB", {data.arrPoints[ENUM_TO_UINT(EPOINT::B)].x, data.arrPoints[ENUM_TO_UINT(EPOINT::B)].y, data.arrPoints[ENUM_TO_UINT(EPOINT::B)].z}},
		{"PointC", {data.arrPoints[ENUM_TO_UINT(EPOINT::C)].x, data.arrPoints[ENUM_TO_UINT(EPOINT::C)].y, data.arrPoints[ENUM_TO_UINT(EPOINT::C)].z}},
		{"NormalAB", {data.arrNormals[ENUM_TO_UINT(ELINE::AB)].x, data.arrNormals[ENUM_TO_UINT(ELINE::AB)].y, data.arrNormals[ENUM_TO_UINT(ELINE::AB)].z}},
		{"NormalBC", {data.arrNormals[ENUM_TO_UINT(ELINE::BC)].x, data.arrNormals[ENUM_TO_UINT(ELINE::BC)].y, data.arrNormals[ENUM_TO_UINT(ELINE::BC)].z}},
		{"NormalCA", {data.arrNormals[ENUM_TO_UINT(ELINE::CA)].x, data.arrNormals[ENUM_TO_UINT(ELINE::CA)].y, data.arrNormals[ENUM_TO_UINT(ELINE::CA)].z}},
		{"Neighbors", {data.arrNeighbors[ENUM_TO_UINT(ELINE::AB)], data.arrNeighbors[ENUM_TO_UINT(ELINE::BC)], data.arrNeighbors[ENUM_TO_UINT(ELINE::CA)]}}
	};
}

void from_json(const json& j, CELL_SAVEDATA& data)
{
	j.at("Index").get_to(data.iIndex);

	auto pointA = j.value("PointA", vector<_float>{0.f, 0.f, 0.f});
	auto pointB = j.value("PointB", vector<_float>{0.f, 0.f, 0.f});
	auto pointC = j.value("PointC", vector<_float>{0.f, 0.f, 0.f});
	auto normalAB = j.value("NormalAB", vector<_float>{0.f, 0.f, 0.f});
	auto normalBC = j.value("NormalBC", vector<_float>{0.f, 0.f, 0.f});
	auto normalCA = j.value("NormalCA", vector<_float>{0.f, 0.f, 0.f});
	auto neighbors = j.value("Neighbors", vector<_int>{-1, -1, -1});

	if (pointA.size() == 3) { data.arrPoints[ENUM_TO_UINT(EPOINT::A)] = { pointA[0], pointA[1], pointA[2]}; }
	if (pointB.size() == 3) { data.arrPoints[ENUM_TO_UINT(EPOINT::B)] = { pointB[0], pointB[1], pointB[2]}; }
	if (pointC.size() == 3) { data.arrPoints[ENUM_TO_UINT(EPOINT::C)] = { pointC[0], pointC[1], pointC[2]}; }
	if (normalAB.size() == 3) { data.arrNormals[ENUM_TO_UINT(ELINE::AB)] = { normalAB[0], normalAB[1], normalAB[2] }; }
	if (normalBC.size() == 3) { data.arrNormals[ENUM_TO_UINT(ELINE::BC)] = { normalBC[0], normalBC[1], normalBC[2] }; }
	if (normalCA.size() == 3) { data.arrNormals[ENUM_TO_UINT(ELINE::CA)] = { normalCA[0], normalCA[1], normalCA[2] }; }
	if (neighbors.size() == 3) { data.arrNeighbors = { neighbors[0], neighbors[1], neighbors[2] }; }
}

void to_json(json& j, const LIGHT_SAVEDATA& data)
{
	j = json
	{
		{"Type", data.eType},
		{"vDiffuse", { data.vDiffuse.x, data.vDiffuse.y, data.vDiffuse.z, data.vDiffuse.w }},
		{"vAmbient", { data.vAmbient.x, data.vAmbient.y, data.vAmbient.z, data.vAmbient.w }}
	};
}

void from_json(const json& j, LIGHT_SAVEDATA& data)
{
	data.eType = j.value("Type", data.eType);

	if (j.contains("vDiffuse"))
	{
		auto vColor = j.value("vDiffuse", vector<_float>{0.f, 0.f, 0.f, 0.f});
		if (vColor.size() == 4) { data.vDiffuse = { vColor[0], vColor[1], vColor[2], vColor[3] }; }
	}

	if (j.contains("vAmbient"))
	{
		auto vColor = j.value("vAmbient", vector<_float>{0.f, 0.f, 0.f, 0.f});
		if (vColor.size() == 4) { data.vAmbient = { vColor[0], vColor[1], vColor[2], vColor[3] }; }
	}
}

void to_json(json& j, const POLYGON_SAVEDATA& data)
{
	j["Cells"] = data.vecCells;

	json verticesJson = json::array();
	for (const auto& v : data.vecVertices)
		verticesJson.push_back({v.x, v.y, v.z});
	j["Vertices"] = std::move(verticesJson);

	j["Indices"] = data.vecIndices;
}

void from_json(const json& j, POLYGON_SAVEDATA& data)
{
	data.vecCells = j.value("Cells", vector<CELL_SAVEDATA>());
	data.vecIndices = j.value("Indices", vector<_ushort>());

	data.vecVertices.clear();
	auto verts = j.value("Vertices", vector<vector<_float>>());

	data.vecVertices.reserve(verts.size());
	for (const auto& v : verts)
	{
		if (v.size() == 3)
		{
			data.vecVertices.push_back(Vec3(v[0], v[1], v[2]));
		}
	}
}

void to_json(json& j, const MAPOBJECT_SAVEDATA& data)
{
	j["Type"] = data.type;
	j["Name"] = data.name;

	json props = json::object();

	if (data.transform)
		props["Transform"] = *data.transform;
	if (data.model)
		props["Model"] = *data.model;
	if (data.vecColliders.size() > 0)
		props["Colliders"] = data.vecColliders;
	if (data.polygon)
		props["Polygon"] = *data.polygon;
	if (data.meshEffect)
		props["MeshEffect"] = *data.meshEffect;
	if(data.light)
		props["Light"] = *data.light;
	if (data.trigger)
		props["Trigger"] = *data.trigger;

	j["Properties"] = props;
}

void from_json(const json& j, MAPOBJECT_SAVEDATA& data)
{
	j.at("Type").get_to(data.type);
	j.at("Name").get_to(data.name);

	if (!j.contains("Properties") || !j["Properties"].is_object())
		return;

	const json& prop = j["Properties"];
	if (prop.contains("Transform"))
		data.transform = prop.at("Transform").get<TRANSFORM_SAVEDATA>();
	if (prop.contains("Model"))
		data.model = prop.at("Model").get<MODEL_SAVEDATA>();

	data.vecColliders.clear();
	if (prop.contains("Colliders") && prop["Colliders"].is_array())
		data.vecColliders = prop.at("Colliders").get<vector<COLLIDER_SAVEDATA>>();
	if (prop.contains("Polygon"))
		data.polygon = prop.at("Polygon").get<POLYGON_SAVEDATA>();
	if (prop.contains("MeshEffect"))
		data.meshEffect = prop.at("MeshEffect").get<MESHEFFECT_PREVIEW_SAVEDATA>();
	if (prop.contains("Light"))
		data.light = prop.at("Light").get<LIGHT_SAVEDATA>();
	if (prop.contains("Trigger"))
		data.trigger = prop.at("Trigger").get<TRIGGERBOX_SAVEDATA>();
}

void to_json(json& j, const EFFECT_PRESET_SNAPSHOT& data)
{
	j["tex"] = json::array
	(
		{
			data.wstrTextureTag[0],
			data.wstrTextureTag[1],
			data.wstrTextureTag[2],
			data.wstrTextureTag[3],
			data.wstrTextureTag[4]
		}
	);

	j["bPattern"] = data.bPattern;
	j["bUVScroll"] = data.bUVScroll;
	j["bDissovle"] = data.bDissovle;
	j["bDistortion"] = data.bDistortion;
	j["bGradation"] = data.bGradation;
	j["bGradationMap"] = data.bGradationMap;

	j["fIntensity"] = data.fIntensity;
	j["fThreshold"] = data.fThreshold;
	j["fEdgieWidth"] = data.fEdgieWidth;
	j["fUVSpeedX"] = data.fUVSpeedX;
	j["fUVSpeedY"] = data.fUVSpeedY;
	j["iPatternCols"] = data.iPatternCols;
	j["iPatternRows"] = data.iPatternRows;
	j["iGradationMapIndex"] = data.iGradationMapIndex;
	j["iGradationMapCount"] = data.iGradationMapCount;
	j["fGradationHeight"] = data.fGradationHeight;

	j["vTintColor"] = { data.vTintColor.x, data.vTintColor.y, data.vTintColor.z, data.vTintColor.w };
	j["fEmmissivePower"] = data.fEmissivePower;

	j["materialTag"] = { Engine_Utils::ToString(data.wstrMaterialTag) };
	j["materialInstanceTag"] = { Engine_Utils::ToString(data.wstrMaterialInstanceTag) };
}

void from_json(const json& j, EFFECT_PRESET_SNAPSHOT& data)
{
	if (j.contains("tex") && j["tex"].is_array())
	{
		const auto& a = j["tex"];
		for (int i = 0; i < 5; ++i)
		{
			if (i < (int)a.size() && a[i].is_string())
				data.wstrTextureTag[i] = a[i].get<string>();
		}
	}

	data.bPattern = j.value("bPattern", data.bPattern);
	data.bUVScroll = j.value("bUVScroll", data.bUVScroll);
	data.bDissovle = j.value("bDissovle", data.bDissovle);
	data.bDistortion = j.value("bDistortion", data.bDistortion);
	data.bGradation = j.value("bGradation", data.bGradation);
	data.bGradationMap = j.value("bGradationMap", data.bGradationMap);

	data.fIntensity = j.value("fIntensity", data.fIntensity);
	data.fThreshold = j.value("fThreshold", data.fThreshold);
	data.fEdgieWidth = j.value("fEdgieWidth", data.fEdgieWidth);
	data.fUVSpeedX = j.value("fUVSpeedX", data.fUVSpeedX);
	data.fUVSpeedY = j.value("fUVSpeedY", data.fUVSpeedY);
	data.iPatternCols = j.value("iPatternCols", data.iPatternCols);
	data.iPatternRows = j.value("iPatternRows", data.iPatternRows);
	data.iGradationMapIndex = j.value("iGradationMapIndex", data.iGradationMapIndex);
	data.iGradationMapCount = j.value("iGradationMapCount", data.iGradationMapCount);
	data.fGradationHeight = j.value("fGradationHeight", data.fGradationHeight);

	if (j.contains("vTintColor"))
	{
		auto vColor = j.value("vTintColor", vector<_float>{0.f, 0.f, 0.f, 0.f});
		if (vColor.size() == 4) { data.vTintColor = { vColor[0], vColor[1], vColor[2], vColor[3]}; }
	}

	data.fEmissivePower = j.value("fEmissivePower", data.fEmissivePower);
	if (j.contains("materialTag") && j["materialTag"].is_string())
		data.wstrMaterialTag = Engine_Utils::ToWString(j["materialTag"].get<std::string>());
	if (j.contains("materialInstanceTag") && j["materialInstanceTag"].is_string())
		data.wstrMaterialInstanceTag = Engine_Utils::ToWString(j["materialInstanceTag"].get<std::string>());
}

void to_json(json& j, const MESHEFFECT_PREVIEW_SAVEDATA& data)
{
	j["RefID"] = data.iPresetID;
	j["Pass"] = data.iShaderPass;
	j["modelTag"] = data.strModelTag;
}

void from_json(const json& j, MESHEFFECT_PREVIEW_SAVEDATA& data)
{
	data.iPresetID = j.value("RefID", data.iPresetID);
	data.iShaderPass = j.value("Pass", data.iShaderPass);
	if (j.contains("modelTag") && j["modelTag"].is_string())
		data.strModelTag = j["modelTag"].get<string>();
}

void to_json(json& j, const MESHEEFFECT_PRESET_SAVEDATA& data)
{
	j["id"] = data.iPresetID;
	j["name"] = data.strName;
	j["snapshot"] = data.snapShot;
}

void from_json(const json& j, MESHEEFFECT_PRESET_SAVEDATA& data)
{
	data.iPresetID = j.value("id", data.iPresetID);
	if (j.contains("name") && j["name"].is_string())
		data.strName = j["name"].get<string>();
	if (j.contains("snapshot"))
		data.snapShot = j["snapshot"].get<EFFECT_PRESET_SNAPSHOT>();
}

void to_json(json& j, const MAPFILE_DATA& data)
{
	j = json
	{
		{"MapName", data.mapName},
		{"Version", data.iVersion},
		{"Objects", data.objects},
		{"MeshEffectPresets", data.meshEffectPresets}
	};
}

void from_json(const json& j, MAPFILE_DATA& data)
{
	j.at("MapName").get_to(data.mapName);
	j.at("Version").get_to(data.iVersion);
	if (j.contains("Objects"))
		data.objects = j.at("Objects").get<vector<MAPOBJECT_SAVEDATA>>();
	else
		data.objects.clear();
	if (j.contains("MeshEffectPresets"))
		data.meshEffectPresets = j.at("MeshEffectPresets").get<vector<MESHEEFFECT_PRESET_SAVEDATA>>();
	else
		data.meshEffectPresets.clear();
}

NS_END

CMapFile_Manager* CMapFile_Manager::Create()
{
	return new CMapFile_Manager();
}

void CMapFile_Manager::Free()
{
	Super::Free();
}
