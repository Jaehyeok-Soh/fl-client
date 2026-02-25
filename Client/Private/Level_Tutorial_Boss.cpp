#include "pch.h"
#include "Level_Tutorial_Boss.h"

// Document & Builder
#include "DataDocument_Map.h"
#include "DataDocument_Effect.h"
#include "Builder_Effect.h"

#include "GameInstance.h"

//=================
// Builder
//=================
#include "Builder_UI.h"
#include "DataStruct_UI.h"

CLevel_Tutorial_Boss::CLevel_Tutorial_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Tutorial_Boss::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Prototype Create Failed");
		return E_FAIL;
	}

	if (FAILED(Build_Files()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Files Create Failed");
		return E_FAIL;
	}
	if (FAILED(Ready_Map()))
		return E_FAIL;
	return S_OK;
}


HRESULT CLevel_Tutorial_Boss::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::TUTORIAL_VILLAGE)))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Build_Files()
{
	ELevelType eLevelType = ELevelType::TUTORIAL_BOSS;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

#pragma region EFFECT
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(iLevelID, eCategory)))
		return E_FAIL;
	std::filesystem::path strUIFolderPath = L"../../Resources/Data/EffectData/";
	if (std::filesystem::exists(strUIFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strUIFolderPath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}
#pragma endregion

	eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	strUIFolderPath = L"../../Resources/Data/UIData/Static/";
	if (std::filesystem::exists(strUIFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strUIFolderPath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CLevel_Tutorial_Boss::Ready_Map()
{
	ELevelType eLevelType = ELevelType::TEST;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	/* Dev Map */
	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/Tutorial/Tutorial_Boss.json";


	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;



	return S_OK;
}

void CLevel_Tutorial_Boss::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

HRESULT CLevel_Tutorial_Boss::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Tutorial_Boss* CLevel_Tutorial_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Tutorial_Boss* pInstance = new CLevel_Tutorial_Boss(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tutorial_Boss::Free()
{
	Super::Free();
}