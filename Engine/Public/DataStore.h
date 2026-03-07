#pragma once
#include "Base.h"
#include "FileUtils.h"
#include "DataDocumentBase.h"

NS_BEGIN(Engine)

/*
* DataRepository가 Level별, 카테고리별로 저장하는곳이였다면
* DataStore는 파일이름별 DataDocument를 저장
*/

class ENGINE_DLL CDataStore : public CBase
{
	using Super = CBase;
protected:
	CDataStore();
	virtual ~CDataStore() = default;
public:
	template<typename T>
	HRESULT LoadFile_Json(const path& filePath);
	HRESULT SaveFile_Json(const path& filePath);
	const CDataDocumentBase* Get_Document(const string& strFileKey);
	_bool Has(const string& strFileKey) const;
	HRESULT Regist_Document(const string& strFileKey, CDataDocumentBase* pDoc);
protected:
	void Erase_Document(const string& strFileKey);
private:
	void Clear();
protected:
	unordered_map<string, CDataDocumentBase*> m_umapDocuments;
public:
	static CDataStore* Create();
	virtual void Free() override;
};

template<typename T>
HRESULT CDataStore::LoadFile_Json(const path& filePath)
{
	CFileUtils* pFileUtil = CFileUtils::Create();
	if (FAILED(pFileUtil->Open(filePath, FileMode::READ)))
	{
		MSG_BOX("CDataStore::LoadFile_Json, open failed");
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	std::string text;
	if (FAILED(pFileUtil->ReadAllText(text)))
	{
		MSG_BOX("CDataStore::LoadFile_Json, read failed");
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	json j = json::parse(text, nullptr, false);
	if (j.is_discarded() == true)
	{
		MSG_BOX("CDataStore::LoadFile_Json, is_discarded");
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	T* pDoc = T::Create();
	if (FAILED(pDoc->FromJson(j)))
	{
		MSG_BOX("CDataStore::LoadFile_Json, read failed");
		Safe_Release(pDoc);
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	if (FAILED(Regist_Document(Engine_Utils::Make_DataFileKey(filePath), pDoc)))
	{
		MSG_BOX("CDataStore::LoadFile_Json, load failed");
		Safe_Release(pDoc);
		Safe_Release(pFileUtil);
		return E_FAIL;
	}

	Safe_Release(pFileUtil);
	return S_OK;
}
NS_END