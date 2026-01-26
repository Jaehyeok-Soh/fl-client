#include "Engine_pch.h"
#include "DataStore.h"

CDataStore::CDataStore()
{
}

HRESULT CDataStore::SaveFile_Json(const path& filePath)
{
	if (filePath.extension() != ".json")
		return E_FAIL;

	CFileUtils* pFileUtil = CFileUtils::Create();
	if (FAILED(pFileUtil->Open(filePath, FileMode::WRITE)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, open failed");
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	CDataDocumentBase* pBase = const_cast<CDataDocumentBase*>(Get_Document(Engine_Utils::Make_DataFileKey(filePath)));
	if (pBase == nullptr)
		return E_FAIL;

	json j = pBase->ToJson();
	std::string text = j.dump(4);

	if (FAILED(pFileUtil->WriteAllText(text)))
	{
		MSG_BOX("CMapFile_Manager::SaveData, write failed");
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	Safe_Release(pFileUtil);
	return S_OK;
}

/// <summary>
/// Document의 소유권은 Store에 있으며 무조건 읽기전용으로!
/// </summary>
/// <param name="strFileKey"></param>
/// <returns></returns>
const CDataDocumentBase* CDataStore::Get_Document(const string& strFileKey)
{
	auto itr = m_umapDocuments.find(strFileKey);
	if (itr == m_umapDocuments.end())
		return nullptr;

	return itr->second;
}

_bool CDataStore::Has(const string& strFileKey) const
{
	auto itr = m_umapDocuments.find(strFileKey);
	return itr != m_umapDocuments.end();
}

HRESULT CDataStore::Regist_Document(const string& strFileKey, CDataDocumentBase* pDoc)
{
	if (strFileKey.empty() || pDoc == nullptr)
		return E_FAIL;

	auto itr = m_umapDocuments.find(strFileKey);
	if (itr != m_umapDocuments.end())
	{
		Safe_Release(itr->second);
		itr->second = nullptr;
		m_umapDocuments.erase(itr);
	}

	m_umapDocuments.emplace(strFileKey, pDoc);
	return S_OK;
}

void CDataStore::Erase_Document(const string& strFileKey)
{
	auto itr = m_umapDocuments.find(strFileKey);
	if (itr == m_umapDocuments.end())
		return;

	Safe_Release(itr->second);
	m_umapDocuments.erase(itr);
}

void CDataStore::Clear()
{
	for (auto& doc : m_umapDocuments)
		Safe_Release(doc.second);

	m_umapDocuments.clear();
}

CDataStore* CDataStore::Create()
{
	return new CDataStore();
}

void CDataStore::Free()
{
	Clear();
	Super::Free();
}
