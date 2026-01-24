#include "pch.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "Level_Assimp.h"
#include "Level_Map.h"
#include "Level_Animation.h"
#include "Level_Camera.h"
#include "Level_UI.h"
#include "Level_Effect.h"
#include "GameInstance.h"
#include "Loader.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Loading::Initialize(ELevelType eNextLevelID)
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	if (!(m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, eNextLevelID)))
		return E_FAIL;

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
		case Tool::ELevelType::MAP:
			pNewLevel = CLevel_Map::Create(m_pDevice, m_pDeviceContext);
			break;
		case Tool::ELevelType::ANIMATION:
			pNewLevel = CLevel_Animation::Create(m_pDevice, m_pDeviceContext);
			break;
		case Tool::ELevelType::EFFECT:
			pNewLevel = CLevel_Effect::Create(m_pDevice, m_pDeviceContext);
			break;
		case Tool::ELevelType::CAMERA:
			pNewLevel = CLevel_Camera::Create(m_pDevice, m_pDeviceContext);
			break;
		case Tool::ELevelType::UI:
			pNewLevel = CLevel_UI::Create(m_pDevice, m_pDeviceContext);
			break;
		case Tool::ELevelType::ASSET_CONVERT:
			pNewLevel = CLevel_Assimp::Create(m_pDevice, m_pDeviceContext);
			break;
		default:
			break;
		}

		if (!pNewLevel)
			return;

		if (SUCCEEDED(m_pGameInstance->Immediately_ChangeLevel(ENUM_TO_UINT(m_eNextLevelID), pNewLevel)))
		{
			return;
		}
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pLoader->Output();

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	//////////////////////////
	m_pImGuiManager->Render_End();

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
	Safe_Release(m_pImGuiManager);
	Safe_Release(m_pLoader);
	Super::Free();
}
