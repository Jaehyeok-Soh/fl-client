#include "Engine_pch.h"
#include "Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Awake(const _uint iCurrentLevelID)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement)
		{
			if (FAILED(pElement->Awake(iCurrentLevelID)))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

void CLayer::Update_Priority(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement)
			pElement->Update_Priority(fTimeDelta);
	}
}

void CLayer::Update(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement)
			pElement->Update(fTimeDelta);
	}
}

void CLayer::Update_Late(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement)
			pElement->Update_Late(fTimeDelta);
	}
}

void CLayer::Ready_Before_Render(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement)
			pElement->Ready_Before_Render(fTimeDelta);
	}
}

CGameObject* CLayer::Get_GameObject(CGameObject* pGo)
{
	if (!pGo)
		return nullptr;

	return Find_GameObject(pGo);
}

void CLayer::Delete_GameObject(CGameObject* pGo)
{
	if (!pGo)
		return;

	auto itr = std::find_if(m_pGameObjects.begin(), m_pGameObjects.end(),
		[pGo](CGameObject* pElement)->_bool
		{
			return pGo == pElement;
		});

	if (itr == m_pGameObjects.end())
		return;

	Safe_Release(pGo);
	m_pGameObjects.erase(itr);
}

CGameObject* CLayer::Add_GameObject(CGameObject* pGO)
{
	if (!pGO)
		return nullptr;

	m_pGameObjects.push_back(pGO);
	return pGO;
}

CGameObject* CLayer::Get_GameObject_Front()
{
	if (m_pGameObjects.size() <= 0)
		return nullptr;

	return m_pGameObjects.front();
}

CGameObject* CLayer::Get_GameObject_Back()
{
	if (m_pGameObjects.size() <= 0)
		return nullptr;

	return m_pGameObjects.back();
}

CGameObject* CLayer::Find_GameObject(CGameObject* pGo)
{
	if (m_pGameObjects.size() <= 0)
		return nullptr;

	auto itr = std::find_if(m_pGameObjects.begin(), m_pGameObjects.end(),
		[pGo](CGameObject* pElement)->_bool
		{
			return pGo == pElement;
		});

	if (itr == m_pGameObjects.end())
		return nullptr;

	return *itr;
}

CLayer* CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		Safe_Release(pElement);
	}

	m_pGameObjects.clear();

	Super::Free();
}
