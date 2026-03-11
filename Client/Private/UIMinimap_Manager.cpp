#include "pch.h"
#include "UIMinimap_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

CUIMinimap_Manager::CUIMinimap_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CUIMinimap_Manager* CUIMinimap_Manager::Create()
{
	return new CUIMinimap_Manager();
}

void CUIMinimap_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END