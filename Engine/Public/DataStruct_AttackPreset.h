#pragma once
#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "json_forward.h"

NS_BEGIN(DTO)

/////////////////-------------------  AttackPreset  -------------------/////////////////
enum class EAttackPresetType : _uint
{
	NORMAL = 0,
	END
};
inline constexpr _uint g_AttackPresetTypeCount{ ENUM_TO_UINT(EAttackPresetType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EAttackPresetType,
	{
		{EAttackPresetType::NORMAL, "NORMAL"},
		{EAttackPresetType::END, "END"},
	}
)

enum class EAttackPresetCategory : _ushort
{
	PlayerBasic = 0,
	PlayerSkill,
	PlayerProjectile,
	MonsterBasic,
	MonsterSkill,
	MonsterPorjectile,
	BossBasic,
	BossSkill,
	BossProjectile,
	END
};
inline constexpr _uint g_AttackPresetCategoryCount{ ENUM_TO_UINT(EAttackPresetCategory::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EAttackPresetCategory,
	{
		{EAttackPresetCategory::PlayerBasic, "PlayerBasic"},
		{EAttackPresetCategory::PlayerSkill, "PlayerSkill"},
		{EAttackPresetCategory::PlayerProjectile, "PlayerProjectile"},
		{EAttackPresetCategory::MonsterBasic, "MonsterBasic"},
		{EAttackPresetCategory::MonsterSkill, "MonsterSkill"},
		{EAttackPresetCategory::MonsterPorjectile, "MonsterPorjectile"},
		{EAttackPresetCategory::BossBasic, "BossBasic"},
		{EAttackPresetCategory::BossSkill, "BossSkill"},
		{EAttackPresetCategory::BossProjectile, "BossProjectile"},
	}
)

enum class EHitType : _uint
{
	Light = 0,
	Heavy,
	Launch,
	Knockdown,
	None,
	END
};
inline constexpr _uint g_HitTypeCount{ ENUM_TO_UINT(EHitType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EHitType,
	{
		{EHitType::Light, "Light"},
		{EHitType::Heavy, "Heavy"},
		{EHitType::Launch, "Launch"},
		{EHitType::Knockdown, "Knockdown"},
		{EHitType::None, "None"},
		{EHitType::END, "END"},
	}
)

enum class EDamageType : _uint
{
	Physical = 0,
	Fire,
	Electric,
	END
};
inline constexpr _uint g_DamageTypeCount{ ENUM_TO_UINT(EDamageType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EDamageType,
	{
		{EDamageType::Physical, "Physical"},
		{EDamageType::Fire, "Fire"},
		{EDamageType::Electric, "Electric"},
		{EDamageType::END, "END"},
	}
)

enum class EHitPolicyType : _uint
{
	Once = 0,
	Interval,
	END
};
inline constexpr _uint g_HitPolicyTypeCount{ ENUM_TO_UINT(EHitPolicyType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EHitPolicyType,
	{
		{EHitPolicyType::Once, "Once"},
		{EHitPolicyType::Interval, "Interval"},
		{EHitPolicyType::END, "END"},
	}
)

/////////////////-------------------  Data Struct  -------------------/////////////////

struct TAttackPreset_CombatData
{
	EHitType eHitType{ EHitType::END };
	EDamageType eDamageType{ EDamageType::END };

	_float fBaseDamage{ 0.f };
	_float fHitStunSec{ 0.f };
	_float fHitStopSec{ 0.f };
	_float fImpulse{ 0.f };
	_float fPushPower{ 0.f };
};

struct TAttackPreset_HitPolicyData
{
	EHitPolicyType ePolicyType{ EHitPolicyType::END };

	_float fIntervalSec{ 0.f };
};

struct TAttackPreset_Data
{
	static constexpr EAttackPresetType eType = EAttackPresetType::NORMAL;
	string strTag{ "" };

	EAttackPresetCategory eCategory{ EAttackPresetCategory::END };
	_ushort iID{ 0 }; // 0 = invalid
	_uint iPresetKey{ 0 };

	TAttackPreset_CombatData tCombat{};
	TAttackPreset_HitPolicyData tPolicy{};

	void Make_Key()
	{
		iPresetKey = (static_cast<_ushort>(eCategory) << 16) | iID;
	}
	static void Resolve_Key(_uint iKey, OUT EAttackPresetCategory &eOutCategory, OUT _ushort &iOutID)
	{
		eOutCategory = static_cast<EAttackPresetCategory>(iKey >> 16);
		iOutID = static_cast<_ushort>(iKey & 0x0000ffff);
	}
};

/////////////////-------------------  to_json, from_json  -------------------/////////////////
inline void to_json(json& j, const TAttackPreset_HitPolicyData& data)
{
	j = json
	{
		{"PolicyType", data.ePolicyType},
		{"fIntervalSec", data.fIntervalSec},
	};
}
inline void from_json(const json& j, TAttackPreset_HitPolicyData& data)
{
	if (j.contains("PolicyType"))
		data.ePolicyType = j["PolicyType"].get<EHitPolicyType>();
	if (j.contains("fIntervalSec"))
		data.fIntervalSec = j["fIntervalSec"].get<_float>();
}

inline void to_json(json& j, const TAttackPreset_CombatData& data)
{
	j = json
	{
		{"HitType", data.eHitType},
		{"DamageType", data.eDamageType},
		{"fBaseDamage", data.fBaseDamage},
		{"fHitStunSec", data.fHitStunSec},
		{"fHitStopSec", data.fHitStopSec},
		{"fImpulse", data.fImpulse},
		{"fPushPower", data.fPushPower},
	};
}
inline void from_json(const json& j, TAttackPreset_CombatData& data)
{
	if (j.contains("HitType"))
		data.eHitType = j["HitType"].get<EHitType>();
	if (j.contains("DamageType"))
		data.eDamageType = j["DamageType"].get<EDamageType>();
	if (j.contains("fBaseDamage"))
		data.fBaseDamage = j["fBaseDamage"].get<_float>();
	if (j.contains("fHitStunSec"))
		data.fHitStunSec = j["fHitStunSec"].get<_float>();
	if (j.contains("fHitStopSec"))
		data.fHitStopSec = j["fHitStopSec"].get<_float>();
	if (j.contains("fImpulse"))
		data.fHitStunSec = j["fImpulse"].get<_float>();
	if (j.contains("fPushPower"))
		data.fHitStopSec = j["fPushPower"].get<_float>();
}

inline void to_json(json& j, const TAttackPreset_Data& data)
{
	j = json
	{
		{ "Type", TAttackPreset_Data::eType },
		{ "strTag", data.strTag },
		{ "eCategory", data.eCategory },
		{ "iID", data.iID },
		{ "tCombat", data.tCombat },
		{ "tPolicy", data.tPolicy },
	};
}
inline void from_json(const json& j, TAttackPreset_Data& data)
{
	j.at("strTag").get_to(data.strTag);
	if (j.contains("eCategory"))
		data.eCategory = j["eCategory"].get<EAttackPresetCategory>();
	if (j.contains("eID"))
		data.iID = j["eID"].get<_ushort>();
	if (j.contains("tCombat"))
		data.tCombat = j["tCombat"].get<TAttackPreset_CombatData>();
	if (j.contains("tPolicy"))
		data.tPolicy = j["tPolicy"].get<TAttackPreset_HitPolicyData>();
}

NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CDataStruct_AttackPreset final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CDataStruct_AttackPreset() = default;
	virtual ~CDataStruct_AttackPreset() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EAttackPresetType::NORMAL); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TAttackPreset_Data& Get_Data() const { return m_Data; }
	DTO::TAttackPreset_Data& Get_Data() { return m_Data; }

private:
	DTO::TAttackPreset_Data m_Data;
public:
	static CDataStruct_AttackPreset* Create() { return new CDataStruct_AttackPreset(); }
	virtual void Free() override { Super::Free(); }
};

NS_END

