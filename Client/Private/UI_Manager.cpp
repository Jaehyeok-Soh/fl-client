#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
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

HRESULT CUI_Manager::Initialize_UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
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

void CUI_Manager::UISound_PlayOneShot(const _char* szSoundTag, const _float fVolume, const _uint iLevelIndex, const _float fPitch, _bool isSteal)
{
	m_pGameInstance->Play_OneShot(iLevelIndex, Engine_Utils::ToHash(szSoundTag), fPitch, isSteal);

}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END