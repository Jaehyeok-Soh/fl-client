#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UITutorial_Manager.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUI_Manager)
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize_UIManager()
{
	m_pTutorialManager = CUITutorial_Manager::Create();
	if (nullptr == m_pTutorialManager)
	{
		MSG_BOX("CUI_Manager::Initialize_UIManager, CUITutorial_Manager::Create Failed");
		return E_FAIL;
	}
	m_pGameInstance->Add_GameObject(0, g_wszStaticObjectLayer, m_pTutorialManager);
	return S_OK;
}

HRESULT CUI_Manager::Regist_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, const _wstring& wstrPrototype, const _wstring& wstrPooltag, const _uint iPrototypeLevel, void* pArg, _uint iNumPrefab)
{
	if (FAILED(m_pGameInstance->Regist_Pool(iPoolRegistLevel, wstrPooltag, g_wszUILayer, iPrototypeLevel, wstrPrototype, pArg, iNumPrefab)))
		return E_FAIL;
	m_vecPrefabs[ENUM_TO_UINT(ePrefab)] = wstrPooltag;
	return S_OK;
}

void CUI_Manager::Request_Add_Prefab(_uint iPoolRegistLevel, EUIPrefabType ePrefab, _uint iSpawnLevel, void* pArg)
{
	m_pGameInstance->Request_AddObject(iPoolRegistLevel, m_vecPrefabs[ENUM_TO_UINT(ePrefab)], iSpawnLevel, pArg,
		[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
		{
			auto* p = static_cast<CCanvas*>(pObj);
			p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
		});
}

void CUI_Manager::Free()
{
	Safe_Release(m_pTutorialManager);
	Safe_Release(m_pGameInstance);
}

NS_END