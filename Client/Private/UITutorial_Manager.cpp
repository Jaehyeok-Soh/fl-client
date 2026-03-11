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
	Super::Free()
	Safe_Release(m_pGameInstance);
}

HRESULT CUITutorial_Manager::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUITutorial_Manager::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

HRESULT CUITutorial_Manager::Awake(const _uint iCurrentLevelID)
{
	return E_NOTIMPL;
}

CGameObject* CUITutorial_Manager::Clone(void* pArg)
{
	return nullptr;
}

NS_END