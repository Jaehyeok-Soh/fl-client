#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUI_Manager)
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Load_UIData(const _wstring& wstrSaveFilePath)
{
	CFileUtils* pFileUtil = CFileUtils::Create();

	if (FAILED(pFileUtil->Open(wstrSaveFilePath, FileMode::READ)))
	{
		MSG_BOX("CUIData_Repository::Load_UIData, open failed");
		return E_FAIL;
	}

	std::string text = {};
	pFileUtil->ReadAllText(text);

	json j = json::parse(text);
	vector<CANVAS_DATA>vec = j.get<vector<CANVAS_DATA>>();
	
	/* Create */
	if (vec.empty())
		return S_OK;

	vector<vector<LAYER_DATA>> vecLayer;
	vector<vector<vector<GENERIC_UI_DATA>>> vecUIdata;

	vecLayer.clear();
	vecUIdata.clear();

	for (const CANVAS_DATA& CanvasData : vec)
	{
		vecLayer.emplace_back();
		vecUIdata.emplace_back();

		for (const LAYER_DATA& LayerData : CanvasData.vecLayers)
		{
			vecLayer.back().push_back(LayerData);

			vecUIdata.back().emplace_back();

			for (const GENERIC_UI_DATA& UIData : LayerData.vecUIData)
			{
				CGenericUI::GENERIC_UI_DESC Desc = {};
				Desc.fX = UIData.fPosX;
				Desc.fY = UIData.fPosY;
				Desc.wstrTextureTag = L"Prototype_Component_GenericUI_Texture";
				Desc.isAlpha = TRUE;
				Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::LOGO);

				CGameObject* pResult = { nullptr };
				pResult = m_pGameInstance->Add_GameObject(static_cast<uint32_t>(ELevelType::LOGO), L"Prototype_UI_GenericUI",
					static_cast<uint32_t>(ELevelType::LOGO), Engine_Utils::ToWString(LayerData.strTag), &Desc);
				if (nullptr == pResult)
					return E_FAIL;
			}
		}
	}
	
	Safe_Release(pFileUtil);

	return S_OK;
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}



void from_json(const json& _j, CANVAS_DATA& _tData)
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

void from_json(const json& _j, LAYER_DATA& _tData)
{
	_j.at("Tag").get_to(_tData.strTag);
	if (_j.contains("UIs"))
		_j.at("UIs").get_to(_tData.vecUIData);

}

void from_json(const json& _j, GENERIC_UI_DATA& _tData)
{
	_j.at("UIType").get_to(_tData.iUIType);
	_j.at("RectTransformType").get_to(_tData.iRectTransformType);
	_j.at("Width").get_to(_tData.fWidth);
	_j.at("Height").get_to(_tData.fHeight);
	_j.at("PosX").get_to(_tData.fPosX);
	_j.at("PosY").get_to(_tData.fPosY);
	_j.at("PosZ").get_to(_tData.fPosZ);
}

NS_END