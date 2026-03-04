#include "Engine_pch.h"
#include "ObjectPool_Manager.h"
#include "ObjectPool.h"
#include "GameObject.h"
#include "GameInstance.h"

CObjectPool_Manager::CObjectPool_Manager()
{
}

HRESULT CObjectPool_Manager::Initialize(_uint iLevelCount)
{
	if (iLevelCount <= 0)
		return E_FAIL;
	
	m_iLevelCount = iLevelCount;
	m_Pools.resize(m_iLevelCount);
	return S_OK;
}

HRESULT CObjectPool_Manager::Regist_Pool(_uint iTargetLevelIndex, const wstring& wstrPoolTag, const wstring& wstrLayerTag, _uint iSeedLevelID, const wstring& wstrSeedPrototypeTag, void* pArg, _uint iPoolCapacityCount)
{
	if (Is_OutOfRange(iTargetLevelIndex))
		return E_FAIL;

	CObjectPool* pPool = Find_Pool(iTargetLevelIndex, wstrPoolTag);
	if (pPool != nullptr)
	{
		MSG_BOX("CObjectPool_Manager::Regist_Pool, already registed");
		return E_FAIL;
	}

 	CBase* pSeed = CGameInstance::GetInstance()->Clone_Prototype(EPrototypeType::GAMEOBJECT, iSeedLevelID, wstrSeedPrototypeTag, pArg);
	if (pSeed == nullptr)
	{
		MSG_BOX("CObjectPool_Manager::Regist_Pool, seed clone failed");
		return E_FAIL;
	}

	pPool = CObjectPool::Create(wstrLayerTag, pArg, static_cast<CGameObject*>(pSeed), iPoolCapacityCount);
	if (pPool == nullptr)
		return E_FAIL;

	auto& umapPools = m_Pools[iTargetLevelIndex];
	umapPools.insert(unordered_map<wstring, CObjectPool*>::value_type(wstrPoolTag, pPool));
	return S_OK;
}

CObjectPool* CObjectPool_Manager::Get_Pool(_uint iLevelIndex, const wstring& wstrPoolTag)
{
	if (Is_OutOfRange(iLevelIndex))
		return nullptr;

	return Find_Pool(iLevelIndex, wstrPoolTag);
}

CGameObject* CObjectPool_Manager::Spawn(_uint iLevelIndex, const wstring& wstrPoolTag, OUT wstring& wstrLayerTag_OUT, void* pArg)
{
	if (Is_OutOfRange(iLevelIndex))
		return nullptr;

 	CObjectPool* pPool = Find_Pool(iLevelIndex, wstrPoolTag);
	if (pPool == nullptr)
		return nullptr;

	wstrLayerTag_OUT = pPool->Get_LayerTag();
	return pPool->Spawn(pArg);
}

HRESULT CObjectPool_Manager::Despawn(_uint iLevelIndex, const wstring& wstrPoolTag, CGameObject* pGo)
{
	if (Is_OutOfRange(iLevelIndex))
		return E_FAIL;

	CObjectPool* pPool = Find_Pool(iLevelIndex, wstrPoolTag);
	if (pPool == nullptr)
		return E_FAIL;

	return pPool->Despawn(pGo);
}

_int CObjectPool_Manager::Get_ActiveCount(_uint iLevelIndex, const wstring& wstrPoolTag)
{
	if (Is_OutOfRange(iLevelIndex))
		return -1;

	CObjectPool* pPool = Find_Pool(iLevelIndex, wstrPoolTag);
	if (pPool == nullptr)
		return -1;

	return (_int)pPool->Get_ActiveCount();
}

CGameObject* CObjectPool_Manager::Get_ActiveObjectAt(_uint iLevelIndex, const wstring& wstrPoolTag, _uint iIndex)
{
	if (Is_OutOfRange(iLevelIndex))
		return nullptr;

	CObjectPool* pPool = Find_Pool(iLevelIndex, wstrPoolTag);
	if (pPool == nullptr)
		return nullptr;

	return pPool->Get_ActiveObjectAt(iIndex);
}

void CObjectPool_Manager::All_Despawn_StaticLevel()
{
	auto& umapPools = m_Pools[0];
	for (auto& Pair : umapPools)
		Pair.second->All_Despawn();
}

void CObjectPool_Manager::Clear(_uint iLevelIndex)
{
	if (Is_OutOfRange(iLevelIndex))
		return;

	auto& umapPools = m_Pools[iLevelIndex];
	for (auto& Pair : umapPools)
		Safe_Release(Pair.second);

	umapPools.clear();
}

void CObjectPool_Manager::All_Clear()
{
	for (_uint i = 0; i < m_iLevelCount; ++i)
		Clear(i);

	m_Pools.clear();
}

CObjectPool* CObjectPool_Manager::Find_Pool(_uint iLevelIndex, const wstring& wstrPoolTag)
{
	auto& umapPools = m_Pools[iLevelIndex];
	if (umapPools.size() <= 0)
		return nullptr;

	auto itr = umapPools.find(wstrPoolTag);
	if (itr == umapPools.end())
		return nullptr;

	return itr->second;
}

CObjectPool_Manager* CObjectPool_Manager::Create(_uint iLevelCount)
{
	CObjectPool_Manager* pInstance = new CObjectPool_Manager();
	if (FAILED(pInstance->Initialize(iLevelCount)))
	{
		MSG_BOX("CObjectPool_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObjectPool_Manager::Free()
{
	All_Clear();
	Super::Free();
}

#ifdef _DEBUG
void CObjectPool_Manager::Collect_PoolTags(_uint iLevelIndex, vector<wstring>& outTags) const
{
	outTags.clear();
	if (Is_OutOfRange(iLevelIndex))
		return;

	const auto& umapPools = m_Pools[iLevelIndex];
	outTags.reserve(umapPools.size());
	for (const auto& Pair : umapPools)
		outTags.push_back(Pair.first);
}

_int CObjectPool_Manager::Get_Capacity(_uint iLevelIndex, const wstring& wstrPoolTag)
{
	if (Is_OutOfRange(iLevelIndex))
		return -1;

	auto& umapPools = m_Pools[iLevelIndex];
	auto itr = umapPools.find(wstrPoolTag);
	if (itr == umapPools.end())
		return -1;

	return (_int)itr->second->Get_Capacity();
}
#endif