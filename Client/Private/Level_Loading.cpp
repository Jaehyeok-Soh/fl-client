#include "pch.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "Level_Square.h"
#include "Level_Test.h"
#include "Level_Tutorial_Boss.h"
#include "Level_Tutorial_Village.h"
#include "TextureBase.h"
#include "Loader.h"

//=================
// Builder
//=================
#include "Builder_Example.h"
#include "BuilderSystem.h"
#include "Builder_UI.h"

#include "CameraMan_Targeter.h"

#include "UI_Manager.h"

#include "GameInstance.h"
CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Loading::Initialize(ELevelType eNextLevelID)
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (!(m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, eNextLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(g_wszUILayer)))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
		return E_FAIL;

	if (FAILED(Build_Files()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;
	return S_OK;
}

HRESULT CLevel_Loading::Awake(const _uint iLevelID)
{


	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;
	return S_OK;
}

void CLevel_Loading::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	
	if (m_pLoader->IsFinished())
	{
		CLevel* pNewLevel = { nullptr };
		switch (m_eNextLevelID)
		{
		case Client::ELevelType::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
			break;
		case Client::ELevelType::TUTORIAL_VILLAGE:
			pNewLevel = CLevel_Tutorial_Village::Create(m_pDevice, m_pDeviceContext);
			break;
		case Client::ELevelType::TUTORIAL_BOSS:
			pNewLevel = CLevel_Tutorial_Boss::Create(m_pDevice, m_pDeviceContext);
			break;
		case Client::ELevelType::SQUARE:
			pNewLevel = CLevel_Square::Create(m_pDevice, m_pDeviceContext);
			break;
		case Client::ELevelType::TEST:
			break;
		case Client::ELevelType::END:
		default:
			break;
		}

		if (!pNewLevel)
			return;
		CUI_Manager::GetInstance()->Clear_Cache(m_pGameInstance->Get_CurrentLevelIndex());
		m_pGameInstance->Immediately_ChangeLevel(ENUM_TO_UINT(m_eNextLevelID), pNewLevel);
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pLoader->Output();
	return S_OK;
}

HRESULT CLevel_Loading::Ready_UI_Layer(const wstring& wstrLayerTag)
{

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Camera_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Loading::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::LOADING)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Build_Files()
{
	ELevelType eLevelType = ELevelType::LOADING;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);
	DTO::ECategory eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Set_LoadingRatio(m_pLoader->Get_LoadingRatio());

	std::filesystem::path  strUIFolderPath = L"../../Resources/Data/UIData/Loading/";
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

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("CLevel_Loading::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	Safe_Release(m_pLoader);
	Super::Free();
}
