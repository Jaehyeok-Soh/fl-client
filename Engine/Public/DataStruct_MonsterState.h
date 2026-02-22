#pragma once
#include "ObjectDataBase.h"
#include "DTO_MonsterState.h"

NS_BEGIN(DTO)

/////////////////-------------------  AttackOverlap  -------------------/////////////////

typedef struct tagMonsterState_Type
{
	enum Enum
	{
		NORMAL,
	};
}MONSTERSTATE_TYPE;

/////////////////-------------------  Data Struct  -------------------/////////////////

/////////////////-------------------  to_json, from_json  -------------------/////////////////

NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CDataStruct_MonsterState final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CDataStruct_MonsterState() = default;
	virtual ~CDataStruct_MonsterState() = default;
public:
	_uint Get_Type() const override { return DTO::MONSTERSTATE_TYPE::NORMAL; }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::MONSTERSTATE_DESC& Get_Data() const { return m_Data; }
	DTO::MONSTERSTATE_DESC& Get_Data() { return m_Data; }

private:
	DTO::MONSTERSTATE_DESC m_Data;
public:
	static CDataStruct_MonsterState* Create() { return new CDataStruct_MonsterState(); }
	virtual void Free() override { Super::Free(); }
};

NS_END