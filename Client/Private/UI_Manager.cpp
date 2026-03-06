#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUI_Manager)
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Add_VecCanvasCache(uint32_t iLevelIndex, CCanvas* pCache)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return E_FAIL; 
	m_vecCanvasCache[iLevelIndex].push_back(pCache); 
	return S_OK;
}

HRESULT CUI_Manager::Add_VecGenericUICache(uint32_t iLevelIndex, CGenericUI* pCache)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return E_FAIL; 
	m_vecGenericUICache[iLevelIndex].push_back(pCache); 
	return S_OK;
}

HRESULT CUI_Manager::Merge_MapCanvasCache(uint32_t iLevelIndex, unordered_map<_string, CCanvas*>&& Cache)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return E_FAIL;

	auto& dst = m_mapCanvasCache[iLevelIndex];

	for (auto& kv : Cache)
	{
		if (dst.find(kv.first) == dst.end())
			dst.emplace(std::move(kv.first), kv.second);
	}
	Cache.clear();
	return S_OK;
}

HRESULT CUI_Manager::Merge_MapGenericUICache(uint32_t iLevelIndex, unordered_map<_string, CGenericUI*>&& Cache)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return E_FAIL;

	auto& dst = m_mapUICache[iLevelIndex];

	for (auto& kv : Cache)
	{
		if (dst.find(kv.first) == dst.end())
			dst.emplace(std::move(kv.first), kv.second);
	}

	Cache.clear();
	return S_OK;
}

CCanvas* CUI_Manager::Find_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag)
{
	auto iter = m_mapCanvasCache[iLevelIndex].find(strCanvasTag);
	if (iter == m_mapCanvasCache[iLevelIndex].end())
		return nullptr;
	return iter->second;
}

CGenericUI* CUI_Manager::Find_GenericUI(uint32_t iLevelIndex, const _string& strUITag)
{
	auto iter = m_mapUICache[iLevelIndex].find(strUITag);
	if (iter == m_mapUICache[iLevelIndex].end())
		return nullptr;
	return iter->second;
}

vector<CGenericUI*>* CUI_Manager::Find_GenericUI_Vector(uint32_t iLevelIndex, const _string& strUILayerTag)
{
	auto* pCanvas = Find_Canvas(iLevelIndex, strUILayerTag);
	if (nullptr == pCanvas)
		return nullptr;
	return pCanvas->Get_UIVector();
}

vector<CCanvas*>* CUI_Manager::Get_Level_All_Canvas(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecCanvasCache[iLevelIndex];
}

vector<CGenericUI*>* CUI_Manager::Get_Level_All_GenericUI(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecGenericUICache[iLevelIndex];
}

void CUI_Manager::Clear_Cache(uint32_t iLevelIndex)
{
	m_vecCanvasCache[iLevelIndex].clear();
	m_vecGenericUICache[iLevelIndex].clear();
	m_mapCanvasCache[iLevelIndex].clear();
	m_mapUICache[iLevelIndex].clear();
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
	switch (ePrefab)
	{
	case Client::EUIPrefabType::MONSTER_NAMEPLATE:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::MONSTER_NAMEPLATE)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::DAMAGE_FONTS_COMMON:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::DAMAGE_FONTS_COMMON)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::DAMAGE_FONTS_CRITICAL:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::DAMAGE_FONTS_CRITICAL)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::DAMAGE_FONTS_HIT:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::DAMAGE_FONTS_HIT)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::BOSS_NAMEPLATE:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::BOSS_NAMEPLATE)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::MINIMAP_MONSTER_ICON:
	{
		_wstring wstr = m_vecPrefabs[ENUM_TO_UINT(EUIPrefabType::MINIMAP_MONSTER_ICON)];
		m_pGameInstance->Request_AddObject(iPoolRegistLevel, wstr, iSpawnLevel, pArg,
			[this, iPoolRegistLevel, iSpawnLevel](CGameObject* pObj)
			{
				auto* p = static_cast<CCanvas*>(pObj);
				p->Ready_Prefab(iPoolRegistLevel, iSpawnLevel);
			});
	}
	break;
	case Client::EUIPrefabType::END:
		break;
	default:
		break;
	}
}

void CUI_Manager::Request_Clear()
{
	m_isClear = true;
}

void CUI_Manager::Request_Clear_DeadUI()
{
	m_isDeadUIClear = true;
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END