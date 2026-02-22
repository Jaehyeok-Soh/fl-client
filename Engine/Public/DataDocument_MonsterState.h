#pragma once
#include "DataDocumentBase.h"
#include "DataStruct_MonsterState.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDataDocument_MonsterState final : public CDataDocumentBase
{
	using Super = CDataDocumentBase;
private:
	CDataDocument_MonsterState();
	virtual ~CDataDocument_MonsterState() = default;

	HRESULT Initialize();
public:
	virtual DTO::ECategory Get_Category() const override { return DTO::ECategory::MONSTER_STATE; }
	virtual json ToJson() const override;
	virtual HRESULT FromJson(const json& j) override;
	HRESULT Try_Add(const DTO::MONSTERSTATE_DESC& data);
private:
	IObjectDataBase* Create_ObjectData();
	HRESULT Try_Add(IObjectDataBase* pObject);
public:
	static CDataDocument_MonsterState* Create();
	virtual void Free() override;
};

NS_END