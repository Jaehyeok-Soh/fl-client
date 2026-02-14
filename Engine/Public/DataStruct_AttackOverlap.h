#pragma once
#include "ObjectDataBase.h"
#include "AttackOverlap_Desc.h"

NS_BEGIN(DTO)

/////////////////-------------------  AttackOverlap  -------------------/////////////////

typedef struct tagAttackOverlap_Type
{
	enum Enum
	{
		NORMAL,
	};
}ATTACKOVERLAP_TYPE;

/////////////////-------------------  Data Struct  -------------------/////////////////

/////////////////-------------------  to_json, from_json  -------------------/////////////////

NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CDataStruct_AttackOverlap final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CDataStruct_AttackOverlap() = default;
	virtual ~CDataStruct_AttackOverlap() = default;
public:
	_uint Get_Type() const override { return DTO::ATTACKOVERLAP_TYPE::NORMAL; }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::ATTACKOVERLAP_DESC& Get_Data() const { return m_Data; }
	DTO::ATTACKOVERLAP_DESC& Get_Data() { return m_Data; }
private:
	DTO::ATTACKOVERLAP_DESC m_Data;
public:
	static CDataStruct_AttackOverlap* Create() { return new CDataStruct_AttackOverlap(); }
	virtual void Free() override { Super::Free(); }
};

NS_END