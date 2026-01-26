#pragma once
#include "Base.h"
#include "DataStruct_Example.h"

/// <summary>
/// 데이터 조회용 객체
/// 이걸 상속 받아서 각 자기 파트의 Document를 만들고 유틸 함수들을 자유롭게 선언하여도 됨
/// Doucment를 꺼내서 필요한 데이터를 자유롭게쓰고 Loading이 끝나면 (TargetLevel의 initialize가 끝나면) Clear 처리 될것
/// 때문에, 캐싱하거나 단순 조회용으로 사용할것이며 별도의 생명주기가 필요한 데이터들은 시스템을 만들어 위임할 것
/// </summary>
NS_BEGIN(Engine)
class ENGINE_DLL CDataDocumentBase abstract : public CBase
{
	using Super = CBase;
protected:
	CDataDocumentBase();
	virtual ~CDataDocumentBase() = default;
public:
	virtual DTO::ECategory Get_Category() const PURE;
	virtual json ToJson() const PURE;
	virtual HRESULT FromJson(const json& j) PURE;
	const vector<DTO::IObjectDataBase*> Get_AllList() const;
	const vector<DTO::IObjectDataBase*> Get_ListByType(_uint iType) const;
	const DTO::IObjectDataBase* Get_ByTag(_uint iType, const string& strTag) const;
	size_t Get_ObjectCount() const;
	void Clear();
protected:
	map<_uint, unordered_map<string, DTO::IObjectDataBase*>> m_Datas;
	unordered_set<string> m_AllTags;
public:
	virtual void Free() override;
};

NS_END