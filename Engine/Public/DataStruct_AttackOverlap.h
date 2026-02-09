#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  AttackOverlap  -------------------/////////////////

typedef struct tagAttackOverlap_Type
{
	enum Enum
	{
		NORMAL,
	};
}ATTACKOVERLAP_TYPE;

typedef struct tagHitBox
{
	enum Enum
	{
		BOX,
		SPHERE,
	};
}HITBOX;

/////////////////-------------------  Data Struct  -------------------/////////////////

typedef struct tagHitboxDesc
{
	string strName = {};

	HITBOX::Enum eType = HITBOX::Enum::SPHERE;
	Vec3 vExtents = {};
	_float fRadius = {};
	Vec3 vOffset = {};

	_float fDuration = {};
	_float fTickTime = { -1.f };

	_float fDamage = {};

	_uint iMaxHit = { 32 };

	PHYSICSFILTERGROUP::Enum eFilterLayer = PHYSICSFILTERGROUP::Enum::NONE;
	_uint iFilterMask = {};
}HITBOX_DESC;

typedef struct tagAttackEvent
{
	string strDescription = {};
	_uint fAnimIndex = {};
	_float fStartTrackPosition = {};
	HITBOX_DESC tHitboxDesc;
}ATTACKEVENT;

typedef struct tagAttackOverlapDesc
{
	string strTag = { "Sample" };
	_uint iNumPool = {};
	vector<ATTACKEVENT> attackEvents;
}ATTACKOVERLAP_DESC;

/////////////////-------------------  to_json, from_json  -------------------/////////////////
void to_json(json& j, const HITBOX_DESC& data);
void from_json(const json& j, HITBOX_DESC& data);

void to_json(json& j, const ATTACKEVENT& data);
void from_json(const json& j, ATTACKEVENT& data);

void to_json(json& j, const ATTACKOVERLAP_DESC& data);
void from_json(const json& j, ATTACKOVERLAP_DESC& data);
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