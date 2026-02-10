#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_UI.h"

NS_BEGIN(Engine)

/* 문서단위 객체이며 문서를 대변하는 객체 /  각 Builder에서 static_cast하여 입맛대로 유틸 함수를 만들고 스코프를 땡겨서 사용 /  선언한 구조체만큼 Try_Add 오버로딩이 늘어남 */
class ENGINE_DLL CDataDocument_UI final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_UI();
	virtual ~CDataDocument_UI() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::UI; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::TUI_CanvasData& data);
	HRESULT Try_Add(const DTO::TUI_GenericUIData& data);
	HRESULT Try_Add(const DTO::TUI_TextData& data);
	HRESULT Try_Add(const DTO::TUI_TriggerData& data);
private:
	IObjectDataBase* Create_ObjectData(DTO::EUIType eType);
	HRESULT Try_Add(IObjectDataBase* pObject);
public:
	static CDataDocument_UI* Create();
	virtual void Free() override;
};

NS_END