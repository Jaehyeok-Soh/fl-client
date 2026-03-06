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
	for (auto& Element : m_pLayers)
	{
		for (auto& [tag, Layer] : Element)
		{
			_bool bScaled = Layer->Is_ScaledDomain();
			Layer->Update_Priority(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Update(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (auto& Element : m_pLayers)
	{
		for (auto& [tag, Layer] : Element)
		{
			_bool bScaled = Layer->Is_ScaledDomain();
			Layer->Update(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Update_Late(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (auto& Element : m_pLayers)
	{
		for (auto& [tag, Layer] : Element)
		{
			_bool bScaled = Layer->Is_ScaledDomain();
			Layer->Update_Late(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Ready_Before_Render(const _float fUnscaledTimeDelta, const _float fScaledTimeDelta)
{
	for (auto& Element : m_pLayers)
	{
		for (auto& [tag, Layer] : Element)
		{
			_bool bScaled = Layer->Is_ScaledDomain();
			Layer->Ready_Before_Render(bScaled == true ? fScaledTimeDelta : fUnscaledTimeDelta);
		}
	}
}

void CObject_Manager::Delete_GameObject(_uint iCloneLevelIndex, CGameObject* pGo)
{
	if (pGo == nullptr)
		return;

	if (pGo->m_pOwnerPool != nullptr)
	{
		MSG_BOX("CObject_Manager::Delete_GameObject, choosed wrong function, must use Despawn_GameObject..");
		return;
	}

	if (CLayer* pFindLayer = Find_Layer(iCloneLevelIndex, pGo->m_wstrLayerTag))
	{
		pFindLayer->Delete_GameObject(pGo);
	}
	else
		MSG_BOX("CObject_Manager::Delete_GameObject, layer invalid");

	Safe_Release(pGo);
}

void CObject_Manager::Despawn_GameObject(_uint iCloneLevelIndex, CGameObject* pGo)
{
	if (pGo == nullptr)
	{
		MSG_BOX("CObject_Manager::Despawn_GameObject, GameObject nullptr");
		return;
	}
	if (pGo->m_pOwnerPool == nullptr)
	{
		MSG_BOX("CObject_Manager::Despawn_GameObject, OwnerPool nullptr");
		return;
	}

	if (CLayer* pFindLayer = Find_Layer(iCloneLevelIndex, pGo->m_wstrLayerTag))
		pFindLayer->Delete_GameObject(pGo);
	else
		MSG_BOX("CObject_Manager::Despawn_GameObject, layer invalid");

	pGo->m_pOwnerPool->Despawn(pGo);
}

CGameObject* CObject_Manager::Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (pGo == nullptr)
	{
		MSG_BOX("CObject_Manager::Add_GameObject, GameObject nullptr");
		return nullptr;
	}
	if (wstrLayerTag.empty())
	{
		MSG_BOX("CObject_Manager::Add_GameObject, wstrLayerTag empty");
		return nullptr;
	}

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
	{
		Safe_Release(pGo);
		return nullptr;
	}

	pGo->Set_Layer(wstrLayerTag);

	if (m_pGameInstance->Is_Awaked(iCloneLevelIndex) == true && (pGo->Is_Awaked() == false))
		pGo->Awake(iCloneLevelIndex);

	return pGo;
}

CGameObject* CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg)
{
	CGameObject* pGo{ nullptr };

	pGo = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg));

	if (pGo == nullptr)
		return nullptr;

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
	{
		Safe_Release(pGo);
		return nullptr;
	}

	pGo->Set_Layer(wstrLayerTag);

	if (m_pGameInstance->Is_Awaked(iCloneLevelIndex) == true)
		pGo->Awake(iCloneLevelIndex);
	return pGo;
}

CGameObject* CObject_Manager::Spawn_GameObject(_uint iCloneLevelIndex, _uint iPoolLevelIndex, const wstring& wstrPoolTag, void* pArg)
{
	CGameObject* pGo{ nullptr };
	wstring wstrLayerTagFromPool = { L"" };
	pGo = m_pPoolManager->Spawn(iPoolLevelIndex, wstrPoolTag, wstrLayerTagFromPool, pArg);

	if (pGo == nullptr)
		return nullptr;

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTagFromPool);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTagFromPool, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
	{
		pGo->m_pOwnerPool->Despawn(pGo);
		return nullptr;
	}

	pGo->Set_Layer(wstrLayerTagFromPool);
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
			m_pGameInstance->Request_DeleteGameObject(iLevelIndex, pGo);
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

	// 스태틱 레벨 오브젝트들 정보 정리
	for (auto& Pair : m_pLayers[/* static */ 0])
	{
		for (auto& go : *Pair.second->Get_GameObject_List())
		{
			if(FAILED(go->Clear_WhenChangeLevel()))
				MSG_BOX("CObject_Manager::Clear, Clear_WhenChangeLevel failed");
		}
	}
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
