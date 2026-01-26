#include "Engine_pch.h"
#include "DataDocumentBase.h"
#include "DataStruct_Example.h"

CDataDocumentBase::CDataDocumentBase()
{
}

const vector<DTO::IObjectDataBase*> CDataDocumentBase::Get_AllList() const
{
	vector<DTO::IObjectDataBase*> vecReturn;
	size_t iCount = Get_ObjectCount();
	if (iCount <= 0)
		return {};

	vecReturn.reserve(iCount);
	for (auto& [iType, umapTags] : m_Datas)
		for (auto& [strTag, object] : umapTags)
			vecReturn.push_back(object);

	return vecReturn;
}

const vector<DTO::IObjectDataBase*> CDataDocumentBase::Get_ListByType(_uint iType) const
{
	vector<DTO::IObjectDataBase*> vecReturn;
	auto itrType = m_Datas.find(iType);
	if (itrType == m_Datas.end())
		return {};

	vecReturn.reserve(itrType->second.size());
	for (const auto& [strTag, object] : itrType->second)
		vecReturn.push_back(object);

	return vecReturn;
}

const DTO::IObjectDataBase* CDataDocumentBase::Get_ByTag(_uint iType, const string& strTag) const
{
	auto itrType = m_Datas.find(iType);
	if (itrType == m_Datas.end())
		return nullptr;

	auto itrObject = itrType->second.find(strTag);
	if (itrObject == itrType->second.end())
		return nullptr;

	return itrObject->second;
}

size_t CDataDocumentBase::Get_ObjectCount() const
{
	size_t iCount = { 0 };
	for (auto& [iType, umapTags] : m_Datas)
		for (auto& [strTag, object] : umapTags)
			++iCount;
	return iCount;
}

void CDataDocumentBase::Clear()
{
	for (auto& [iType, umapTags] : m_Datas)
	{
		for (auto& [strTag, object] : umapTags)
			Safe_Release(object);
	}

	m_Datas.clear();
	m_AllTags.clear();
}

void CDataDocumentBase::Free()
{
	Clear();
	Super::Free();
}
