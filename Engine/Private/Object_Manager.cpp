#include "Engine_pch.h"
#include "Object_Manager.h"
#include "Event_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "ObjectPool.h"
#include "ObjectPool_Manager.h"
#include "GameInstance.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iLevelCount, CObjectPool_Manager* pPoolManager)
{
	if (pPoolManager == nullptr)
		return E_FAIL;

	m_pPoolManager = pPoolManager;
	m_iLevelCount = iLevelCount;
	m_pLayers = vector<map<const wstring, class CLayer*>>{ iLevelCount };

	return S_OK;
}

HRESULT CObject_Manager::Awake(const _uint iCurrentLevelID)
{
	for (auto &Pair : m_pLayers[iCurrentLevelID])
	{
		CLayer*& pLayer = Pair.second;
		if (pLayer != nullptr)
		{
			if (FAILED(pLayer->Awake(iCurrentLevelID)))
				return E_FAIL;
		}
	}

	for (auto& Pair : m_pLayers[0])
	{
		CLayer*& pLayer = Pair.second;
		if (pLayer != nullptr)
		{
			if (FAILED(pLayer->Awake(iCurrentLevelID)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CObject_Manager::Update_Priority(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			_bool bScaled = Pair.second->Is_ScaledDomain();
			Pair.second->Update_Priority(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Update(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			_bool bScaled = Pair.second->Is_ScaledDomain();
			Pair.second->Update(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Update_Late(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			_bool bScaled = Pair.second->Is_ScaledDomain();
			Pair.second->Update_Late(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Ready_Before_Render(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			_bool bScaled = Pair.second->Is_ScaledDomain();
			Pair.second->Ready_Before_Render(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Delete_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (!pGo || wstrLayerTag.empty())
		return;

	if (pGo->Is_PoolObject() == true)
	{
		CObjectPool* pOwnerPool = pGo->Get_OwnerPool();
		if (pOwnerPool == nullptr)
			return;

		const wstring& wstrLayerTagFromPool = pOwnerPool->Get_LayerTag();

		if (CLayer* pFindLayer = Find_Layer(iCloneLevelIndex, wstrLayerTagFromPool))
			pFindLayer->Delete_GameObject(pGo);

		pOwnerPool->Despawn(pGo);
	}
	else
	{
		if (CLayer* pFindLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag))
			pFindLayer->Delete_GameObject(pGo);

	}
	Safe_Release(pGo);
}

CGameObject* CObject_Manager::Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (!pGo || wstrLayerTag.empty())
		return nullptr;

	Safe_AddRef(pGo);

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
		return nullptr;

	if (m_pGameInstance->Is_Awaked(iCloneLevelIndex) == true)
		pGo->Awake(iCloneLevelIndex);

	return pGo;
}

CGameObject* CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg)
{
	CGameObject* pGo = { nullptr };

	if (iCloneLevelIndex == 0)
		int a = 0;

	// Pool쪽 먼저 체크
	wstring wstrLayerTagFromPool = { L"" };
	pGo = m_pPoolManager->Spawn(iPrototypeLevelIndex, wstrPrototypeTag, wstrLayerTagFromPool, pArg);

	// Pool에 없으면 Clone
	if (pGo == nullptr)
		pGo =  static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg));

	if (pGo == nullptr)
		return nullptr;

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
		return nullptr;

	if ((m_pGameInstance->Is_Awaked(iCloneLevelIndex) == true) && (pGo->Is_Awaked() == false))
		pGo->Awake(iCloneLevelIndex);

	return pGo;
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, _uint iIndex)
{
	if (CLayer* pFindLayer = Find_Layer(iLevelIndex, wstrLayerTag))
	{
		return pFindLayer->Get_GameObject(iIndex);
	}

	return nullptr;
}

CGameObject* CObject_Manager::Get_GameObject_Front(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	if (CLayer* pFindLayer = Find_Layer(iLayerIndex, wstrLayerTag))
	{
		return pFindLayer->Get_GameObject_Front();
	}

	return nullptr;
}

CGameObject* CObject_Manager::Get_GameObject_Back(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	if (CLayer* pFindLayer = Find_Layer(iLayerIndex, wstrLayerTag))
	{
		return pFindLayer->Get_GameObject_Back();
	}

	return nullptr;
}

list<CGameObject*>* CObject_Manager::Get_GameObject_List(_uint iLayerIndex, const wstring& wstrLayerTag)
{
	if (CLayer* pFindLayer = Find_Layer(iLayerIndex, wstrLayerTag))
	{
		return pFindLayer->Get_GameObject_List();
	}

	return nullptr;
}

void CObject_Manager::Clear_Layer(_uint iLevelIndex, const wstring& wstrLayerTag)
{
	if (CLayer* pFindLayer = Find_Layer(iLevelIndex, wstrLayerTag))
	{
		list<CGameObject*> *pFindList = pFindLayer->Get_GameObject_List();
		
		for (CGameObject* pGo : *pFindList)
		{
			m_pGameInstance->Request_DeleteGameObject(iLevelIndex, wstrLayerTag, pGo);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& wstrLayerTag)
{
	if (iLevelIndex >= m_iLevelCount)
		return nullptr;

	auto itr = m_pLayers[iLevelIndex].find(wstrLayerTag);
	if (itr == m_pLayers[iLevelIndex].end())
		return nullptr;

	return itr->second;
}

CObject_Manager* CObject_Manager::Create(_uint iLevelCount, CObjectPool_Manager* pPoolManager)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iLevelCount, pPoolManager)))
	{
		MSG_BOX("CObject_Manager::Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			Safe_Release(Pair.second);
		}
		Element.clear();
	}
	m_pLayers.clear();
	Safe_Release(m_pGameInstance);
	Super::Free();
}
