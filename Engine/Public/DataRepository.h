#pragma once
#include "Base.h"
#include "DataStore.h"

NS_BEGIN(Engine)

class CDataRepository final : public CBase
{
	using Super = CBase;
public:
	using DataRepository_LoadFunc = std::function<HRESULT(CDataStore*, const path&)>;
	using DataRepository_CreateFunc = std::function<CDataDocumentBase*()>;
	struct StoreEntry
	{
		CDataStore* pStore{ nullptr };
		DataRepository_LoadFunc load;
		DataRepository_CreateFunc create;
	};
private:
	CDataRepository();
	virtual ~CDataRepository() = default;

	HRESULT Initialize(_uint iLevelCount);
public:
	template<typename T>
	HRESULT Regist_Category(_uint iLevelID, DTO::ECategory eCategory);

	HRESULT Load_Folder_Json(_uint iLevelID, DTO::ECategory eCategory, const path& folderPath);
	HRESULT Load_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath);
	HRESULT Save_File_Json(_uint iLevelID, DTO::ECategory eCategory, const path& filePath) const;
	const CDataDocumentBase* Get_Document(_uint iLevelID, DTO::ECategory eCategory, const string& strFileKey);
	CDataDocumentBase* Ensure_Document(_uint iLevelID, DTO::ECategory eCategory, const path& filePath);
private:
	void Clear(_uint iLevelID);
	void Clear_All();
private:
	vector<map<DTO::ECategory, StoreEntry>> m_mapStores;
public:
	static CDataRepository* Create(_uint iLevelCount);
	virtual void Free() override;
};

template<typename T>
inline HRESULT CDataRepository::Regist_Category(_uint iLevelID, DTO::ECategory eCategory)
{
	if (iLevelID >= m_mapStores.size())
		return E_FAIL;

	CDataStore* pStore = CDataStore::Create();

	StoreEntry entry;
	entry.pStore = pStore;

	entry.load = [](CDataStore* pStore, const path& filePath) ->HRESULT
	{
			return pStore->LoadFile_Json<T>(filePath);
	};
	entry.create = []()->CDataDocumentBase*
	{
			return T::Create();
	};

	auto itr = m_mapStores[iLevelID].find(eCategory);
	if (itr != m_mapStores[iLevelID].end())
	{
		Safe_Release(itr->second.pStore);
		m_mapStores[iLevelID].erase(itr);
	}

	m_mapStores[iLevelID][eCategory] = std::move(entry);
	return S_OK;
}

NS_END

