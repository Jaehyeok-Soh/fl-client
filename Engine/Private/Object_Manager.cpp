#include "Engine_pch.h"
#include "Object_Manager.h"
#include "Event_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "GameInstance.h"

CObject_Manager::CObject_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iLevelCount)
{
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

	return S_OK;
}

void CObject_Manager::Update_Priority(const _float fTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			Pair.second->Update_Priority(fTimeDelta);
		}
	}
}

void CObject_Manager::Update(const _float fTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			Pair.second->Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Update_Late(const _float fTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			Pair.second->Update_Late(fTimeDelta);
		}
	}
}

void CObject_Manager::Ready_Before_Render(const _float fTimeDelta)
{
	for (map<const wstring, CLayer*>& Element : m_pLayers)
	{
		for (auto& Pair : Element)
		{
			Pair.second->Ready_Before_Render(fTimeDelta);
		}
	}
}

void CObject_Manager::Delete_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (!pGo || wstrLayerTag.empty())
		return;

	if (CLayer* pFindLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag))
		pFindLayer->Delete_GameObject(pGo);
}

CGameObject* CObject_Manager::Add_GameObject(_uint iCloneLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (!pGo || wstrLayerTag.empty())
		return nullptr;

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pGo)))
		return nullptr;

	if (m_pGameInstance->Is_Awaked() == true)
		pGo->Awake(iCloneLevelIndex);

	return pGo;
}

CGameObject* CObject_Manager::Add_GameObject(_uint iPrototypeLevelIndex, const wstring& wstrPrototypeTag, _uint iCloneLevelIndex, const wstring& wstrLayerTag, void* pArg)
{
	CGameObject* pClone = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT, iPrototypeLevelIndex, wstrPrototypeTag, pArg));
	if (pClone == nullptr)
		return nullptr;

	CLayer* pLayer = Find_Layer(iCloneLevelIndex, wstrLayerTag);
	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();
		m_pLayers[iCloneLevelIndex].insert(map<const wstring, CLayer*>::value_type(wstrLayerTag, pLayer));
	}

	if (FAILED(pLayer->Add_GameObject(pClone)))
		return nullptr;

	if (m_pGameInstance->Is_Awaked() == true)
		pClone->Awake(iCloneLevelIndex);

	return pClone;
}

CGameObject* CObject_Manager::Get_GameObject(_uint iLevelIndex, const wstring& wstrLayerTag, CGameObject* pGo)
{
	if (CLayer* pFindLayer = Find_Layer(iLevelIndex, wstrLayerTag))
	{
		return pFindLayer->Get_GameObject(pGo);
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

CObject_Manager* CObject_Manager::Create(_uint iLevelCount)
{
	CObject_Manager* pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iLevelCount)))
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
