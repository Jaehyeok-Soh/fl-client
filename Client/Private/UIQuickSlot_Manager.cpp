#include "pch.h"
#include "UIQuickSlot_Manager.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUIQuickSlot_Manager)
CUIQuickSlot_Manager::CUIQuickSlot_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUIQuickSlot_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END