#include "Engine_pch.h"
#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Change_Level(_uint iNewLevelID, CLevel* pNewLevel)
{
	if (m_pCurrentLevel)
	{
		m_bAwaked = false;
		m_pGameInstance->Clear(m_iCurrentLevelID);
	}


	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iCurrentLevelID = iNewLevelID;
 	Awake();
	return S_OK;
}

HRESULT CLevel_Manager::Awake()
{
	if (FAILED(m_pCurrentLevel->Awake(m_iCurrentLevelID)))
	{
		MSG_BOX("CLevel_Manager::Awake, m_pCurrentLevel Failed");
		return E_FAIL;
	}
	m_bAwaked = true;
	return S_OK;
}

void CLevel_Manager::Update(_float fTimeDelta)
{
	if(m_pCurrentLevel)
		m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (m_pCurrentLevel)
		m_pCurrentLevel->Render();
	return S_OK;
}

const Vec4& CLevel_Manager::Get_ClearColor() const
{
	return m_pCurrentLevel->Get_ClearColor();
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
	Safe_Release(m_pGameInstance);
	Super::Free();
}
