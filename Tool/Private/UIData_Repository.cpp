#include "pch.h"
#include "UIData_Repository.h"
#include "Tool_Defines.h"
#include "ImGui_UIManager.h"
#include "FileUtils.h"
#include "GameInstance.h"

NS_BEGIN(Tool)

IMPLEMENT_SINGLETON(CUIData_Repository)


CUIData_Repository::CUIData_Repository()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIData_Repository::Load_UIData(const _wstring& wstrSaveFilePath, OUT vector<CANVAS_DATA>& OutVec)
{
	CFileUtils* pFileUtil = CFileUtils::Create();

	if (FAILED(pFileUtil->Open(wstrSaveFilePath, FileMode::READ)))
	{
		MSG_BOX("CUIData_Repository::Load_UIData, open failed");
		return E_FAIL;
	}

	std::string text = {};
	pFileUtil->ReadAllText(text);

	order_json j = json::parse(text);
	m_vecCanvasData = j.get<vector<CANVAS_DATA>>();
	OutVec = j.get<vector<CANVAS_DATA>>();
	Safe_Release(pFileUtil);

	return S_OK;
}

HRESULT CUIData_Repository::Save_UIData(const _wstring& wstrSaveFilePath)
{
	CFileUtils* pFileUtil = CFileUtils::Create();

	m_vecCanvasData = CImGui_UIManager::GetInstance()->Get_CurCanvas_Ref();

	if (FAILED(pFileUtil->Open(wstrSaveFilePath, FileMode::WRITE)))
	{
		MSG_BOX("CUIData_Repository::Save_UIData, open failed");
		return E_FAIL;
	}

	order_json j = m_vecCanvasData;
	std::string text = j.dump(4);
	if (FAILED(pFileUtil->WriteAllText(text)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, write failed");
		return E_FAIL;
	}

	Safe_Release(pFileUtil);
	return S_OK;
}

HRESULT CUIData_Repository::Make_UIObjects(const vector<CANVAS_DATA>& vecData)
{
	if (vecData.empty())
		return S_OK;

	vector<vector<LAYER_DATA>> vecLayer;
	vector<vector<vector<GENERIC_UI_DATA>>> vecUIdata;

	vecLayer.clear();
	vecUIdata.clear();

	for (const CANVAS_DATA& CanvasData : vecData)
	{
		// 1) Canvas 한 칸 생성
		vecLayer.emplace_back();  
		vecUIdata.emplace_back(); 

		// 2) 레이어들 채우기
		for (const LAYER_DATA& LayerData : CanvasData.vecLayers)
		{
			// 레이어 추가
			vecLayer.back().push_back(LayerData);

			// 3) Layer 한 칸 생성 (이 레이어의 UI 리스트 자리)
			vecUIdata.back().emplace_back(); // vecUIdata.back().back() : 이번 레이어의 UI 목록

			// 4) UI들 채우기
			for (const GENERIC_UI_DATA& UIData : LayerData.vecUIData)
			{
				vecUIdata.back().back().push_back(UIData);
			}
		}
	}
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
	_j["Layers"] = _tData.vecLayers;
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
	if (_j.contains("Layers"))
		_j.at("Layers").get_to(_tData.vecLayers);
}


void to_json(order_json& _j, const LAYER_DATA& _tData)
{
	_j["Tag"] = _tData.strTag;
	_j["UIs"] = _tData.vecUIData;
}

void from_json(const order_json& _j, LAYER_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
	if (_j.contains("UIs"))
		_j.at("UIs").get_to(_tData.vecUIData);

	_tData.vecUIObjects.clear();
}

void to_json(order_json& _j, const GENERIC_UI_DATA& _tData)
{
	_j["Name"] = _tData.strName;
	_j["UIType"] = _tData.iUIType;
	_j["RectTransformType"] = _tData.iRectTransformType;
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
	_j.at("Width").get_to(_tData.fWidth);
	_j.at("Height").get_to(_tData.fHeight);
	_j.at("PosX").get_to(_tData.fPosX);
	_j.at("PosY").get_to(_tData.fPosY);
	_j.at("PosZ").get_to(_tData.fPosZ);
}

NS_END