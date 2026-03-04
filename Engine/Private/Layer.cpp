#include "Engine_pch.h"
#include "Layer.h"
#include "ObjectPool.h"
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
		if (pElement->Is_Active())
			pElement->Update_Priority(fTimeDelta);
	}
}

void CLayer::Update(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement->Is_Active())
			pElement->Update(fTimeDelta);
	}
}

void CLayer::Update_Late(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement->Is_Active())
			pElement->Update_Late(fTimeDelta);
	}
}

void CLayer::Ready_Before_Render(const _float fTimeDelta)
{
	for (CGameObject*& pElement : m_pGameObjects)
	{
		if (pElement->Can_Render())
			pElement->Ready_Before_Render(fTimeDelta);
	}
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	return Find_GameObject(iIndex);
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

	m_pGameObjects.erase(itr);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGO)
{
	if (!pGO)
		return E_FAIL;

	m_pGameObjects.push_back(pGO);
	return S_OK;
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

CGameObject* CLayer::Find_GameObject(_uint iIndex)
{
	if (m_pGameObjects.size() <= 0 || m_pGameObjects.size() <= iIndex)
		return nullptr;

	auto itr = m_pGameObjects.begin();
	std::advance(itr, iIndex);
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
		if (pElement->Is_FromPool() == false)
			Safe_Release(pElement);
		else
			pElement->Get_OwnerPool()->Despawn(pElement);
	}

	m_pGameObjects.clear();

	Super::Free();
}
