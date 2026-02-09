#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_AttackOverlap.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_AttackOverlap final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_AttackOverlap();
	virtual ~CDataDocument_AttackOverlap() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::OVERLAP_SCRIPT; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::ATTACKOVERLAP_DESC& data);
private:
	IObjectDataBase* Create_ObjectData();
	HRESULT Try_Add(IObjectDataBase* pObject);
public:
	static CDataDocument_AttackOverlap* Create();
	virtual void Free() override;
};

NS_END