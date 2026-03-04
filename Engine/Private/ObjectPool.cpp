#include "Engine_pch.h"
#include "ObjectPool.h"
#include "GameObject.h"
#include "EngineConsole.h"
#include "GameInstance.h"

CObjectPool::CObjectPool()
{
}

HRESULT CObjectPool::Initialize(const wstring& wstrLayerTag, void* pArg, CGameObject* pSeed, _uint iPoolCapacityCount)
{
	if (pSeed == nullptr || wstrLayerTag.empty() == true || iPoolCapacityCount <= 0)
		return E_FAIL;

	m_iTotalCount = iPoolCapacityCount;
	m_vecObjects.reserve(m_iTotalCount);
	m_wstrLayerTag = wstrLayerTag;

	if (FAILED(Ready_Objects(pArg, pSeed)))
	{
		Safe_Release(pSeed);
		return E_FAIL;
	}

	Safe_Release(pSeed);
	return S_OK;
}

CGameObject* CObjectPool::Spawn(void* pArg)
{
	CLOG_INFO("Spawn");
	if (m_iActiveCount >= m_vecObjects.size())
	{
		MSG_BOX("CObjectPool::Spawn, Pool is Full");
		return nullptr;
	}

	CGameObject* pGo = m_vecObjects[m_iActiveCount];

	if (FAILED(pGo->Spawn_FromPool(pArg)))
		return nullptr;

	pGo->Set_ActiveIndex(m_iActiveCount);
	++m_iActiveCount;
	return pGo;
}

HRESULT CObjectPool::Despawn(CGameObject* pGo)
{
	_int iIndex = pGo->Get_ActiveIndex();
	if (iIndex < 0 || iIndex >= (_int)m_iActiveCount)
	{
		CLOG_INFO("Despawn:: SKIP");
		return S_OK;
	}

	if (!pGo)
	{
		MSG_BOX("CObjectPool::Despawn, Parameter is nullptr");
		return E_FAIL;
	}
	_int iNeed_Despawned_Index = pGo->Get_ActiveIndex();
	if (iNeed_Despawned_Index < 0 || iNeed_Despawned_Index >= (_int)m_iActiveCount)
	{
		MSG_BOX("CObjectPool::Despawn, Index was wrong");
		return E_FAIL;
	}
	if (this->m_wstrLayerTag == L"UI_Layer")
		int a = 0;
	_int iLastActiveIndex = m_iActiveCount - 1;
	if (iNeed_Despawned_Index != iLastActiveIndex)
	{
		std::swap(m_vecObjects[iNeed_Despawned_Index], m_vecObjects[iLastActiveIndex]);

		const _int& Swaped_Despawned_Index = iNeed_Despawned_Index;
		const _int& Swaped_iLastActiveIndex = iLastActiveIndex;

		m_vecObjects[Swaped_Despawned_Index]->Set_ActiveIndex(Swaped_Despawned_Index);
		m_vecObjects[Swaped_iLastActiveIndex]->Set_ActiveIndex(Swaped_iLastActiveIndex);
	}

	if (FAILED(pGo->Despawn_FromPool()))
	{
		MSG_BOX("CObjectPool::Despawn, Despawn failed");
		return E_FAIL;
	}

	CLOG_INFO("Despawn");
	--m_iActiveCount;
	return S_OK;
}

void CObjectPool::All_Despawn()
{
	if (m_iActiveCount <= 0)
		return;

	for (_int i = m_iActiveCount - 1; i >= 0; --i)
	{
		Despawn(m_vecObjects[i]);
	}
}

CGameObject* CObjectPool::Get_ActiveObjectAt(_uint iIndex)
{
	if (iIndex >= m_iActiveCount)
		return nullptr;

	return m_vecObjects[iIndex];
}

HRESULT CObjectPool::Ready_Objects(void *pArg, CGameObject* pSeed)
{
	for (size_t i = 0; i < m_iTotalCount; ++i)
	{
		CGameObject* pReturned = { nullptr };
		if (!(pReturned = pSeed->Clone(pArg)))
			return E_FAIL;

		pReturned->Mark_Pooled();
		pReturned->Set_OwnerPool(this);
		m_vecObjects.push_back(pReturned);
	}
	return S_OK;
}

CObjectPool* CObjectPool::Create(const wstring& wstrLayerTag, void* pArg, CGameObject* pSeed, _uint iPoolCapacityCount)
{
	CObjectPool* pInstance = new CObjectPool();
	if (FAILED(pInstance->Initialize(wstrLayerTag, pArg, pSeed, iPoolCapacityCount)))
	{
		MSG_BOX("CObjectPool::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObjectPool::Free()
{
	All_Despawn();

	for (CGameObject* pElement : m_vecObjects)
	{
		Safe_Release(pElement);
	}
	m_vecObjects.clear();
	m_iActiveCount = 0;
	Super::Free();
}
