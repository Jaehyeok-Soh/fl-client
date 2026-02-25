#include "pch.h"
#include "Level_Tutorial_Village.h"
#include "GameInstance.h"
#include "DataDocument_Map.h"

CLevel_Tutorial_Village::CLevel_Tutorial_Village(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Tutorial_Village::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;


	if (FAILED(Ready_Map()))
		return E_FAIL;

	return S_OK;
}



HRESULT CLevel_Tutorial_Village::Ready_Map()
{
	ELevelType		eLevelType	= ELevelType::TUTORIAL_VILLAGE;
	DTO::ECategory	eCategory	= DTO::ECategory::MAP;
	_uint			iLevelID	= ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	/* Dev Map */
	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/Tutorial/Tutorial_Village.json";

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}



HRESULT CLevel_Tutorial_Village::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	return S_OK;
}




void CLevel_Tutorial_Village::Update(const _float fTimeDelta)
{

	Super::Update(fTimeDelta);

}

HRESULT CLevel_Tutorial_Village::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}



void CLevel_Tutorial_Village::Free()
{
	Super::Free();
}

