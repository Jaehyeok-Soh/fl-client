#include "pch.h"
#include "UITutorial_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

CUITutorial_Manager::CUITutorial_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CUITutorial_Manager* CUITutorial_Manager::Create()
{
	return new CUITutorial_Manager();
}

void CUITutorial_Manager::Free()
{
	Super::Free();
}

NS_END