#include "pch.h"
#include "AttackPreset_DataManager.h"

IMPLEMENT_SINGLETON(CAttackPreset_DataManager)

CAttackPreset_DataManager::CAttackPreset_DataManager()
{
}

HRESULT CAttackPreset_DataManager::Upsert_Data(const DTO::TAttackPreset_Data& inData)
{
	DTO::TAttackPreset_Data data = inData;
	data.Make_Key();

	if (data.strTag.empty() == true)
		return E_FAIL;

	// Tag 面倒 眉农
	auto itrTag = m_umapTagToKey.find(data.strTag);
	if (itrTag != m_umapTagToKey.end())
	{
		if (itrTag->second != data.iPresetKey)
			return E_FAIL;
	}

	// Key 面倒 眉农
	auto itrKey = m_mapDatas.find(data.iPresetKey);
	if (itrKey != m_mapDatas.end())
	{
		if (itrKey->second.strTag != data.strTag)
		{
			m_umapTagToKey.erase(itrKey->second.strTag);
			m_umapTagToKey[data.strTag] = data.iPresetKey;
		}
		itrKey->second = data;
	}
	else
	{
		m_mapDatas.emplace(data.iPresetKey, data);
		m_umapTagToKey[data.strTag] = data.iPresetKey;
	}

	return S_OK;
}

void CAttackPreset_DataManager::Clear()
{
	m_mapDatas.clear();
	m_umapTagToKey.clear();
	m_mapNextID.clear();
}

_bool CAttackPreset_DataManager::Is_TagUnique(const std::string& strTag, _uint iIgnoreKey) const
{
	auto itr = m_umapTagToKey.find(strTag);
	if (itr == m_umapTagToKey.end())
		return true;

	return (iIgnoreKey != 0 && itr->second == iIgnoreKey);
}

HRESULT CAttackPreset_DataManager::Create_Data(DTO::EAttackPresetCategory eCategory, const string& strTag, OUT _uint& iOutKey)
{
	if (strTag.empty())
		return E_FAIL;
	if (Is_TagUnique(strTag) == false)
		return E_FAIL;

	DTO::TAttackPreset_Data data{};
	data.strTag = strTag;
	data.eCategory = eCategory;

	_ushort iNext = Get_NextID(eCategory);
	data.iID = iNext;
	data.Make_Key();

	if (FAILED(Upsert_Data(data)))
		return E_FAIL;

	m_mapNextID[eCategory] = static_cast<_ushort>(iNext + 1);
	iOutKey = data.iPresetKey;
	return S_OK;
}

DTO::TAttackPreset_Data* CAttackPreset_DataManager::Find(_uint iKey)
{
	auto itr = m_mapDatas.find(iKey);
	if (itr == m_mapDatas.end())
		return nullptr;

	return &itr->second;
}

HRESULT CAttackPreset_DataManager::Remove(_uint iKey)
{
	auto itr = m_mapDatas.find(iKey);
	if (itr == m_mapDatas.end())
		return E_FAIL;

	m_umapTagToKey.erase(itr->second.strTag);
	m_mapDatas.erase(itr);
	return S_OK;
}

_ushort CAttackPreset_DataManager::Get_NextID(DTO::EAttackPresetCategory eCategory)
{
	auto itr = m_mapNextID.find(eCategory);
	if (itr != m_mapNextID.end())
		return itr->second;

	_ushort iMaxID{ 0 };
	for (const auto& [key, value] : m_mapDatas)
	{
		if (value.eCategory != eCategory)
			continue;
		if (value.iID > iMaxID)
			iMaxID = value.iID;
	}
	_ushort iNext = static_cast<_ushort>(iMaxID + 1);
	if (iNext == 0)
		iNext = 1;
	m_mapNextID[eCategory] = iNext;
	return iNext;
}

void CAttackPreset_DataManager::Update_NextID(const DTO::TAttackPreset_Data& data)
{
	_ushort iCur = data.iID;
	auto& iNext = m_mapNextID[data.eCategory];
	if (iNext  == 0)
		iNext  = 1;
	if (iCur >= iNext)
		iNext = static_cast<_ushort>(iCur + 1);
}

void CAttackPreset_DataManager::Free()
{
	Clear();
	Super::Free();
}
