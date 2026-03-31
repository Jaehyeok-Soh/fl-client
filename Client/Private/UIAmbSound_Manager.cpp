#include "pch.h"
#include "UIAmbSound_Manager.h"
#include "Engine_Utils.h"
#include "UI_Manager.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUIAmbSound_Manager)

CUIAmbSound_Manager::CUIAmbSound_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUIAmbSound_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

NS_END