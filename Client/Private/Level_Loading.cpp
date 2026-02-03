#include "pch.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
#include "TextureBase.h"
#include "Loader.h"

//=================
// Builder
//=================
#include "Builder_Example.h"
#include "BuilderSystem.h"

#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
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
		case Client::ELevelType::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
			break;
		default:
			break;
		}

		if (!pNewLevel)
			return;

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
