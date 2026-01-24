#include "pch.h"
#include "UEMapdataParser.h"
#include <fstream>


CUEMapdataParser::CUEMapdataParser()
{
}

HRESULT CUEMapdataParser::Initialize(const MAPPARSER_DESC& desc)
{
	m_path = desc.wstrPath;
	return S_OK;
}

HRESULT CUEMapdataParser::Read_Mapdata()
{
	if (!std::filesystem::exists(m_path))
		return E_FAIL;

	std::ifstream ifs(m_path, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		MSG_BOX("CUEMapdataParser::Read_Mapdata, File open failed");
		return E_FAIL;
	}

	json jArray;
	ifs >> jArray;
	m_vecData.reserve(jArray.size());
	for (const auto& jObj : jArray)
	{
		PARSED_MAPDATA_OUTER outer = {};
		const string type = jObj.value("Type", string{});
		if (type.find("StaticMesh") == string::npos)
			continue;
		outer.strType = jObj["Type"].get<string>();
		outer.strName = jObj.at("Name").get<string>();
		outer.Properties = jObj.at("Properties").get<PARSED_MAPDATA_INNER>();
		m_vecData.push_back(outer);		
	}
	ifs.close();
	return S_OK;
}

HRESULT CUEMapdataParser::Write_Mapdata()
{
	std::filesystem::path savePath = m_path.parent_path();
	wstring wstrfileName = m_path.stem();
	wstrfileName += L"_Parsed";
	savePath /= wstrfileName;
	savePath.replace_extension(L".json");

	std::ofstream ofs(savePath, std::ios::out | std::ios::binary);
	if (!ofs.is_open())
	{
		MSG_BOX("CUEMapdataParser::Write_Mapdata, Failed");
		return E_FAIL;
	}


	json jArray = json::array();
	for (PARSED_MAPDATA_OUTER element : m_vecData)
	{
		jArray.push_back(element);
	}

	// setw(i) 출력될 값의 최소폭을 i 만큼 지정
	// Json 이므로 4칸 들여쓰기로 출력
	ofs << std::setw(4) << jArray << std::endl;
	ofs.close();
	return S_OK;
}

CUEMapdataParser* CUEMapdataParser::Create(const MAPPARSER_DESC& desc)
{
	CUEMapdataParser* pInstance = new CUEMapdataParser();
	if (FAILED(pInstance->Initialize(desc)))
	{
		MSG_BOX("CUEMapdataParser::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUEMapdataParser::Free()
{
	Super::Free();
}

NS_BEGIN(Tool)

void to_json(json& _j, const PARSED_MAPDATA_INNER_STATICMESH& _tData)
{
	_j = json
	{
		{"ObjectName", _tData.strObjectName},
		{"ObjectPath", _tData.strObjectPath},
	};
}

void from_json(const json& _j, PARSED_MAPDATA_INNER_STATICMESH& _tData)
{
	_j.at("ObjectName").get_to(_tData.strObjectName);
	_j.at("ObjectPath").get_to(_tData.strObjectPath);
}

void to_json(json& _j, const PARSED_MAPDATA_INNER_BRUSH& _tData)
{
	_j = json
	{
		{"ObjectName", _tData.strObjectName},
		{"ObjectPath", _tData.strObjectPath},
	};
}

void from_json(const json& _j, PARSED_MAPDATA_INNER_BRUSH& _tData)
{
	_j.at("ObjectName").get_to(_tData.strObjectName);
	_j.at("ObjectPath").get_to(_tData.strObjectPath);
}

void to_json(json& _j, const PARSED_MAPDATA_INNER_BRUSHBODYSETUP& _tData)
{
	_j = json
	{
		{"ObjectName", _tData.strObjectName},
		{"ObjectPath", _tData.strObjectPath},
	};
}

void from_json(const json& _j, PARSED_MAPDATA_INNER_BRUSHBODYSETUP& _tData)
{
	_j.at("ObjectName").get_to(_tData.strObjectName);
	_j.at("ObjectPath").get_to(_tData.strObjectPath);
}

void to_json(json& _j, const PARSED_MAPDATA_OUTER& _tData)
{
	_j = json
	{
		{"Type", _tData.strType},
		{"Name", _tData.strName},
		{"Properties", _tData.Properties},
	};
}

void from_json(const json& _j, PARSED_MAPDATA_OUTER& _tData)
{
	const string type = _j.value("Type", string{});
	_tData.strType = type;

	if (type.find("StaticMesh") == string::npos)
	{
		_tData.strName = _j.value("Name", std::string{});
		return;
	}

	_tData.strName = _j.value("Name", string{});
	if (_j.contains("Properties") && _j["Properties"].is_object())
	{
		_j.at("Properties").get_to(_tData.Properties);
	}
}

void to_json(json& _j, const PARSED_MAPDATA_INNER &_tData)
{
	_j = json
	{
		{"StaticMesh", _tData.StaticMesh},
		{"RelativeLocation", json{{"X", _tData.vPosition.x }, {"Y", _tData.vPosition.y }, {"Z", _tData.vPosition.z }}},
		{"RelativeRotation", json{{"Pitch", _tData.vPitchYawRoll.x }, {"Yaw", _tData.vPitchYawRoll.y }, {"Roll", _tData.vPitchYawRoll.z }}},
		{"RelativeScale3D", json{{"X", _tData.vScale.x }, {"Y", _tData.vScale.y }, {"Z", _tData.vScale.z }}},
	};
}

void from_json(const json& _j, PARSED_MAPDATA_INNER& _tData)
{
	const json* pJsonRef = &_j;
	
	if (_j.contains("Properties") && _j["Properties"].is_object())
		pJsonRef = &_j["Properties"];


	if (pJsonRef->contains("StaticMesh") && (*pJsonRef)["StaticMesh"].is_object())
		(*pJsonRef)["StaticMesh"].get_to(_tData.StaticMesh);
	else if(_j.contains("StaticMesh") && _j["StaticMesh"].is_object())
		_j["StaticMesh"].get_to(_tData.StaticMesh);
	else if (_j.contains("StaticMeshComponent") && _j["StaticMeshComponent"].is_object())
	{
		const auto& staticMeshComponent = _j["StaticMeshComponent"];
		if (staticMeshComponent.contains("Properties") && staticMeshComponent["Properties"].contains("StaticMesh"))
		{
			staticMeshComponent["Properties"]["StaticMesh"].get_to(_tData.StaticMesh);
		}
	}

	if (pJsonRef->contains("RelativeLocation")) read_vec3_xyz((*pJsonRef)["RelativeLocation"], _tData.vPosition);
	else if(_j.contains("RelativeLocation")) read_vec3_xyz(_j["RelativeLocation"], _tData.vPosition);

	if (pJsonRef->contains("RelativeRotation")) read_vec3_PitchYawRoll((*pJsonRef)["RelativeRotation"], _tData.vPitchYawRoll);
	else if (_j.contains("RelativeRotation")) read_vec3_PitchYawRoll(_j["RelativeRotation"], _tData.vPitchYawRoll);

	if (pJsonRef->contains("RelativeScale3D")) read_vec3_defaultscale((*pJsonRef)["RelativeScale3D"], _tData.vScale);
	else if (_j.contains("RelativeScale3D")) read_vec3_defaultscale(_j["RelativeScale3D"], _tData.vScale);
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

NS_END