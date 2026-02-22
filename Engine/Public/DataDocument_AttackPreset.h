#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_AttackPreset.h"

NS_BEGIN(Engine)

class CDataDocument_AttackPreset final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_AttackPreset();
	virtual ~CDataDocument_AttackPreset() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::ATTACK_PRESET; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::TAttackPreset_Data& data);
private:
	IObjectDataBase* Create_ObjectData();
	HRESULT Try_Add(IObjectDataBase* pObject);
public:
	static CDataDocument_AttackPreset* Create();
	virtual void Free() override;
};

NS_END