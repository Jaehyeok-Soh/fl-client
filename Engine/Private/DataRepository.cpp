#include "Engine_pch.h"
#include "DataEnum.h"
#include "DataRepository.h"

CDataRepository::CDataRepository()
{

}

HRESULT CDataRepository::Initialize(_uint iLevelCount)
{
	m_mapStores.resize(iLevelCount);
	return S_OK;
}

CDataRepository* CDataRepository::Create(_uint iLevelCount)
{
	CDataRepository* pInstance = new CDataRepository();
	if(FAILED(pInstance->Initialize(iLevelCount)))
	{
		MSG_BOX("CDataRepository::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CDataRepository::Load_Folder_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath)
{
	if (iLevelID >= m_mapStores.size())
		return E_FAIL;

	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr == m_mapStores[iLevelID].end() || itr->second.pStore == nullptr || itr->second.load == nullptr)
		return E_FAIL;

	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_regular_file() == false)
			continue;

		const path filePath = entry.path();
		if (filePath.extension() != ".json")
			continue;

		if (FAILED(itr->second.load(itr->second.pStore, filePath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CDataRepository::Load_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath)
{
	if (iLevelID >= m_mapStores.size())
		return E_FAIL;

	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr == m_mapStores[iLevelID].end() || itr->second.pStore == nullptr || itr->second.load == nullptr)
		return E_FAIL;

	return itr->second.load(itr->second.pStore, filePath);
}

HRESULT CDataRepository::Save_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath) const
{
	if (iLevelID >= m_mapStores.size())
		return E_FAIL;

	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr == m_mapStores[iLevelID].end() || itr->second.pStore == nullptr)
		return E_FAIL;

	return itr->second.pStore->SaveFile_Json(filePath);
}

const CDataDocumentBase* CDataRepository::Get_Document(_uint iLevelID, DTO::ECategory eCategory, const string& strFileKey)
{
	if (iLevelID >= m_mapStores.size())
		return nullptr;

	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr == m_mapStores[iLevelID].end() || (itr->second.pStore == nullptr))
		return nullptr;

	return itr->second.pStore->Get_Document(strFileKey);
}

CDataDocumentBase* CDataRepository::Ensure_Document(_uint iLevelID, DTO::ECategory eCategory, const path& filePath)
{
	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr == m_mapStores[iLevelID].end())
		return nullptr;

	CDataStore* pStore = itr->second.pStore;
	if (pStore == nullptr)
		return nullptr;

	string strKey = Engine_Utils::Make_DataFileKey(filePath);

	if (pStore->Has(strKey) == false)
	{
		if (itr->second.create == nullptr)
			return nullptr;

		CDataDocumentBase* pDocument = itr->second.create();
		if (FAILED(pStore->Regist_Document(strKey, pDocument)))
		{
			Safe_Release(pDocument);
			return nullptr;
		}
	}
	return const_cast<CDataDocumentBase*>(pStore->Get_Document(strKey));
}

void CDataRepository::Clear(_uint iLevelID)
{
	for (auto& Pair : m_mapStores[iLevelID])
		Safe_Release(Pair.second.pStore);

	m_mapStores[iLevelID].clear();
}

void CDataRepository::Clear_All()
{
	for (auto& stores : m_mapStores)
	{
		for (auto& Pair : stores)
		{
			Safe_Release(Pair.second.pStore);
		}
		stores.clear();
	}
	m_mapStores.clear();
}

void CDataRepository::Free()
{
	Clear_All();
	Super::Free();
}
