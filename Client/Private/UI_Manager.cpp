#include "pch.h"
#include "UI_Manager.h"

#include "GameInstance.h"

CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
