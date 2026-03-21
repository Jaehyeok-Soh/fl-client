#include "pch.h"
#include "UITutorial_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUITutorial_Manager)

CUITutorial_Manager::CUITutorial_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUITutorial_Manager::Tutorial_Update(const _float fTimeDelta)
{

}

void CUITutorial_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

NS_END