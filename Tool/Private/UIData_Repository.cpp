#include "pch.h"
#include "UIData_Repository.h"
#include "Tool_Defines.h"
#include "ImGui_UIManager.h"
#include "FileUtils.h"
#include "GameInstance.h"

#define UISAVEPATH L"../../Resources/Data/UIData/Data.json"
NS_BEGIN(Tool)

IMPLEMENT_SINGLETON(CUIData_Repository)


CUIData_Repository::CUIData_Repository()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIData_Repository::Load_UIData(OUT vector<CANVAS_DATA>& OutRef)
{
	CFileUtils* pFileUtil = CFileUtils::Create();

	if (FAILED(pFileUtil->Open(UISAVEPATH, FileMode::READ)))
	{
		MSG_BOX("CUIData_Repository::Load_UIData, open failed");
		return E_FAIL;
	}

	std::string text = {};
	pFileUtil->ReadAllText(text);

	order_json j = json::parse(text);
	m_vecUIDTO = j.get<vector<CANVAS_DATA>>();
	OutRef = j.get<vector<CANVAS_DATA>>();
	Safe_Release(pFileUtil);
	return S_OK;
}

HRESULT CUIData_Repository::Save_UIData()
{
	CFileUtils* pFileUtil = CFileUtils::Create();

	if (FAILED(pFileUtil->Open(UISAVEPATH, FileMode::WRITE)))
	{
		MSG_BOX("CUIData_Repository::Save_UIData, open failed");
		return E_FAIL;
	}

	order_json j = m_vecUIDTO;
	std::string text = j.dump(4);
	if (FAILED(pFileUtil->WriteAllText(text)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, write failed");
		return E_FAIL;
	}

	Safe_Release(pFileUtil);
	return S_OK;
}

void CUIData_Repository::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

void to_json(order_json& _j, const CANVAS_DATA& _tData)
{
	_j["Tag"] = _tData.strTag;
	_j["UsingViewport"] = _tData.isUsingViewport;
	_j["Width"] = _tData.fWidth;
	_j["Height"] = _tData.fHeight;
	_j["PosX"] = _tData.fPosX;
	_j["PosY"] = _tData.fPosY;
	_j["PosZ"] = _tData.fPosZ;
}

void from_json(const order_json& _j, CANVAS_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
	_j.at("UsingViewport").get_to(_tData.isUsingViewport);
	_j.at("Width").get_to(_tData.fWidth);
	_j.at("Height").get_to(_tData.fHeight);
	_j.at("PosX").get_to(_tData.fPosX);
	_j.at("PosY").get_to(_tData.fPosY);
	_j.at("PosZ").get_to(_tData.fPosZ);
}


void to_json(order_json& _j, const LAYER_DATA& _tData)
{
	_j["Tag"] = _tData.strTag;
}

void from_json(const order_json& _j, LAYER_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
}

void to_json(order_json& _j, const GENERIC_UI_DATA& _tData)
{
	_j["Name"] = _tData.strName;
	_j["UIType"] = _tData.iUIType;
	_j["RectTransformType"] = _tData.iRectTransformType;
	_j["TextureTag"] = _tData.strTextureTag;
	_j["TextureIndex"] = _tData.iTextureIndex;
	_j["Width"] = _tData.fWidth;
	_j["Height"] = _tData.fHeight;
	_j["PosX"] = _tData.fPosX;
	_j["PosY"] = _tData.fPosY;
	_j["PosZ"] = _tData.fPosZ;
}

void from_json(const order_json& _j, GENERIC_UI_DATA& _tData)
{
	_j.at("Name").get_to(_tData.strName);
	_j.at("UIType").get_to(_tData.iUIType);
	_j.at("RectTransformType").get_to(_tData.iRectTransformType);
	_j.at("TextureTag").get_to(_tData.strTextureTag);
	_j.at("TextureIndex").get_to(_tData.iTextureIndex);
	_j.at("Width").get_to(_tData.fWidth);
	_j.at("Height").get_to(_tData.fHeight);
	_j.at("PosX").get_to(_tData.fPosX);
	_j.at("PosY").get_to(_tData.fPosY);
	_j.at("PosZ").get_to(_tData.fPosZ);
}

void to_json(order_json& _j, const UI_DTO& _tData)
{
	_j = _tData.UIVO;
}

void from_json(const order_json& _j, UI_DTO& _tData)
{
	_tData.UIVO = _j.get<GENERIC_UI_DATA>();
}

void to_json(order_json& _j, const LAYER_DTO& _tData)
{
	_j["Layer"] = _tData.LayerVO;
	_j["UIs"] = _tData.vecUIDTO;
}

void from_json(const order_json& _j, LAYER_DTO& _tData)
{
	_j.at("Layer").get_to(_tData.LayerVO);
	_j.at("UIs").get_to(_tData.vecUIDTO);
}

void to_json(order_json& _j, const CANVAS_DTO& _tData)
{
	_j["Canvas"] = _tData.CanvasVO;
	_j["Layers"] = _tData.vecLayerDTO;
}

void from_json(const order_json& _j, CANVAS_DTO& _tData)
{
	_j.at("Canvas").get_to(_tData.CanvasVO);
	_j.at("Layers").get_to(_tData.vecLayerDTO);
}

NS_END