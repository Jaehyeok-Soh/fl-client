#pragma once
#include "Base.h"
#include "DataStore.h"

NS_BEGIN(Engine)

/*
* 의의
* 런타임 게임데이터 저장 제공이 아닌 Tool에서 저장한 데이터를 System에 저장 및 각 객체 생성을 위한 짧은 수명의 데이터 집합소
* 툴의 저장 데이터를 Loading 및 요청 시점에 Repository에 데이터 저장하고 Loading이 끝나면 Clear 하는 구조
*/
class CDataRepository final : public CBase
{
	using Super = CBase;
public:
	using DataRepository_LoadFunc = std::function<HRESULT(CDataStore*, const path&)>;
	using DataRepository_CreateFunc = std::function<CDataDocumentBase*()>;
	// DataStore를 생성하여 캐싱, load함수와 CDataDocument 생성하는 함수를 바인딩한 Entry 구조체
	// 타입별로 load, create가 이루어져야하기때문에 내부적으로 일관된 처리를 하기위하여 만듬
	struct TStoreEntry
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

	// strFileKey - 파일의 이름으로 등록, 중복을 허용 하지않음
	const CDataDocumentBase* Get_Document(_uint iLevelID, DTO::ECategory eCategory, const string& strFileKey);
	CDataDocumentBase* Ensure_Document(_uint iLevelID, DTO::ECategory eCategory, const path& filePath);
	
	void Clear(_uint iLevelID);
	void Clear_All();
private:
	vector<map<DTO::ECategory, TStoreEntry>> m_mapStores;
public:
	static CDataRepository* Create(_uint iLevelCount);
	virtual void Free() override;
};

/*
* Repository에 TStoreEntry를 생성하는 함수
* 타입별로 Document의 Load_Json, Create가 호출되어야하는 이유로 TStoreEntry에 각 타입별 함수를 람다로 바인딩
* 추후에는 공통된 함수 호출로 처리하기 위한 과정이다.
* 때문에 Tool의 시작이나 Level을 시작할때 Category별, DocumentBase별 Registry가 이루어져야 함
*/
template<typename T>
inline HRESULT CDataRepository::Regist_Category(_uint iLevelID, DTO::ECategory eCategory)
{
	static_assert(std::is_base_of_v<CDataDocumentBase, T> == true, "T is not derived from CDataDocumentBase");
	if (iLevelID >= m_mapStores.size())
		return E_FAIL;

	CDataStore* pStore = CDataStore::Create();

	TStoreEntry entry;
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

