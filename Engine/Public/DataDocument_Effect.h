#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_Effect.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_Effect final :
    public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_Effect();
	virtual ~CDataDocument_Effect() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::EFFECT; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::TEFFECT_ContainerData& data);
private:
	IObjectDataBase* Create_ObjectData(DTO::EEffectType eType);
	HRESULT Try_Add(IObjectDataBase* pObject);
public:
	static CDataDocument_Effect* Create();
	virtual void Free() override;
};

NS_END

/* 문서단위 객체이며 문서를 대변하는 객체 /  각 Builder에서 static_cast하여 입맛대로 유틸 함수를 만들고 스코프를 땡겨서 사용 /  선언한 구조체만큼 Try_Add 오버로딩이 늘어남 */