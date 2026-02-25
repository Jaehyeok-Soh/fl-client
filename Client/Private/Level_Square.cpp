#include "pch.h"
#include "Level_Square.h"

// Document & Builder
#include "DataDocument_Map.h"
#include "DataDocument_Effect.h"
#include "Builder_Effect.h"

#include "GameInstance.h"

CLevel_Square::CLevel_Square(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Square::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
		return E_FAIL;

	if (FAILED(Build_Files()))
		return E_FAIL;

	if (FAILED(Ready_Map()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::SQUARE)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Build_Files()
{
	ELevelType eLevelType = ELevelType::SQUARE;
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
	return S_OK;
}

HRESULT CLevel_Square::Ready_Map()
{
	ELevelType		eLevelType = ELevelType::SQUARE;
	DTO::ECategory	eCategory = DTO::ECategory::MAP;
	_uint			iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	/* Dev Map */
	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/Tutorial/Viilage/Viilage.json";

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

void CLevel_Square::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Square::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CLevel_Square::Free()
{
	Super::Free();
}