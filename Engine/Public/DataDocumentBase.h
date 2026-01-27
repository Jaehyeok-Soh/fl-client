#pragma once
#include "Base.h"
#include "json_forward.h"
#include "DataEnum_JsonDecl.h"
#include "DataStruct_Example.h"

/// <summary>
/// 의의
/// 엔진상에서 문서를 대신하는 객체
/// 이걸 상속 받아서 각 자기 파트의 Document를 만들고 유틸 함수들을 자유롭게 선언하여도 됨
/// CDataDocumentBase는 GameObject에 국한되지 않고 ToolObject스타일에 따라 GameDataManager및 System에 데이터를 저장시켜
/// 런타임 데이터로 활용이 가능할것 같음
/// 
/// 역할
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
	// 각 Category를 직접 지정하여 반환할 것
	virtual DTO::ECategory Get_Category() const PURE;
	virtual json ToJson() const PURE;
	virtual HRESULT FromJson(const json& j) PURE;

	// 현재 문서에 저장된 모든 ObjectDataList를 반환
	const vector<IObjectDataBase*> Get_AllList() const;
	// 현재 문서의 특정 타입에 해당하는 ObjectDataList를 반환
	const vector<IObjectDataBase*> Get_ListByType(_uint iType) const;
	// 현재 문서의 특정 타입, 특정 Tag에 해당하는 ObjectData를 반환
	const IObjectDataBase* Get_ByTag(_uint iType, const string& strTag) const;
	size_t Get_ObjectCount() const;
	void Clear();
protected:
	map<_uint, unordered_map<string, IObjectDataBase*>> m_Datas;
	unordered_set<string> m_AllTags;
public:
	virtual void Free() override;
};

NS_END