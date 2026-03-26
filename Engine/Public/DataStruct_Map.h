#pragma once
#include "ObjectDataBase.h"
#include "DataEnum.h"
#include "json_forward.h"
#include "Quest_DataModel.h"
#include "CitizenData.h"

NS_BEGIN(DTO)
#pragma region Make Monster Type
enum class EMakeMonsterType
{
	/* Monster */
	Dog,
	Boomer,
	Shooter,

	/* Boss 관련 */
	Xibi,
	Lianhuo,
	Fly,
	Veteran,

	END,
};
inline std::string MakeMonsterType_ToString(EMakeMonsterType eType)
{
	switch (eType)
	{
	case DTO::EMakeMonsterType::Dog:			return "Dog";
	case DTO::EMakeMonsterType::Shooter:		return "Shooter";
	case DTO::EMakeMonsterType::Boomer:			return "Boomer";
	case DTO::EMakeMonsterType::Fly:			return "Fly";
	case DTO::EMakeMonsterType::Veteran:		return "Veteran";



		/* Boss관련 */
	case DTO::EMakeMonsterType::Xibi:			return "Xibi";
	case DTO::EMakeMonsterType::Lianhuo:		return "Lianhuo";
	default:									return "Unknown";
	}
}

inline EMakeMonsterType MakeMonsterType_ToEnum(const std::string strType)
{ 
	if (strType == "Dog")		return DTO::EMakeMonsterType::Dog;
	if (strType == "Shooter")	return DTO::EMakeMonsterType::Shooter;
	if (strType == "Boomer")	return DTO::EMakeMonsterType::Boomer;
	if (strType == "Fly")		return DTO::EMakeMonsterType::Fly;
	if (strType == "Veteran")	return DTO::EMakeMonsterType::Veteran;


	if (strType == "Xibi")		return DTO::EMakeMonsterType::Xibi;
	if (strType == "Lianhuo")	return DTO::EMakeMonsterType::Lianhuo;
	return EMakeMonsterType::END;
}
#pragma endregion

#pragma region Make Object Type
enum class EMakeObjectType
{
	/* Battle Filed */
	Battle_Field,
	PointLight,

	END,
};

inline std::string MakeObjectType_ToString(EMakeObjectType eType)
{
	switch (eType)
	{
	case DTO::EMakeObjectType::Battle_Field:	return "Battle_Field";
	case DTO::EMakeObjectType::PointLight:		return "PointLight";
	default:									return "Unknown";
	}
}
inline DTO::EMakeObjectType MakeObjectType_ToEnum(const std::string strType)
{
	if (strType == "Battle_Field")				return DTO::EMakeObjectType::Battle_Field;
	if (strType == "PointLight")				return DTO::EMakeObjectType::PointLight;

	return DTO::EMakeObjectType::END;
}

#pragma endregion

#pragma region Make TriggerBox Type

enum class EMakeTriggerBoxType
{
	Change_Level,
	Monster_Spawner,
	END,
};

#pragma endregion

#pragma region Make NPC Type
inline std::string MakeNPCType_ToString(OBJECT_ENUM_TAG::Enum eTag)
{
	switch (eTag)
	{
	case Engine::OBJECT_ENUM_TAG::NPC_DEFAULT:		return "NPC_Default";
	case Engine::OBJECT_ENUM_TAG::NPC_PAN:			return "NPC_Pan";
	case Engine::OBJECT_ENUM_TAG::NPC_BERENICA:		return "NPC_Berenica";
	case Engine::OBJECT_ENUM_TAG::NPC_TAVERN:		return "NPC_Tavern";
	case Engine::OBJECT_ENUM_TAG::NPC_VILLAGER_1:	return "NPC_Villager_1";
	case Engine::OBJECT_ENUM_TAG::NPC_KID_1:		return "NPC_Kid_1";
	case Engine::OBJECT_ENUM_TAG::NPC_KID_2:		return "NPC_Kid_2";
	case Engine::OBJECT_ENUM_TAG::NPC_VETERAN:		return "NPC_Veteran";
	case Engine::OBJECT_ENUM_TAG::NPC_CITIZEN:		return "NPC_Citizen";
	default:									return "Unknown";
	}
}

inline OBJECT_ENUM_TAG::Enum MakeNPCType_ToEnum(const std::string strType)
{
	if (strType == "NPC_Default")				return Engine::OBJECT_ENUM_TAG::NPC_DEFAULT;
	if (strType == "NPC_Pan")					return Engine::OBJECT_ENUM_TAG::NPC_PAN;
	if (strType == "NPC_Berenica")				return Engine::OBJECT_ENUM_TAG::NPC_BERENICA;
	if (strType == "NPC_Tavern")				return Engine::OBJECT_ENUM_TAG::NPC_TAVERN;
	if (strType == "NPC_Villager_1")			return Engine::OBJECT_ENUM_TAG::NPC_VILLAGER_1;
	if (strType == "NPC_Kid_1")					return Engine::OBJECT_ENUM_TAG::NPC_KID_1;
	if (strType == "NPC_Citizen")				return Engine::OBJECT_ENUM_TAG::NPC_CITIZEN;
	if (strType == "NPC_Veteran")				return Engine::OBJECT_ENUM_TAG::NPC_VETERAN;
	if (strType == "NPC_Kid_2")					return Engine::OBJECT_ENUM_TAG::NPC_KID_2;

	return Engine::OBJECT_ENUM_TAG::NPC_DEFAULT;
}
#pragma endregion

NS_END

NS_BEGIN(Engine)

class CCollider;

struct ENGINE_DLL CLIENT_MAKEPATH_DESC_BASE
{
public:
	explicit CLIENT_MAKEPATH_DESC_BASE()
	{

	}
	/* 작성 권장 */
	explicit CLIENT_MAKEPATH_DESC_BASE(const CLIENT_MAKEPATH_DESC_BASE& rhs)
	{
		return ;
	}
	virtual ~CLIENT_MAKEPATH_DESC_BASE() {};
public:
	virtual void	from_Json(const json& LoadJson)PURE;
	virtual void	to_Json(json& SaveJson)PURE;
};

#pragma region 파싱용 Description 작성

#pragma region Static Object

struct ENGINE_DLL STATICOBJECT_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	using Super = CLIENT_MAKEPATH_DESC_BASE;
	wstring			wstrTest{ L"" };
public:
	explicit STATICOBJECT_DESC()
		: wstrTest{ L"" }
	{
	}
	explicit STATICOBJECT_DESC(const STATICOBJECT_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), wstrTest(rhs.wstrTest)
	{
		return;
	}
	virtual ~STATICOBJECT_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};


#pragma endregion

#pragma region LandScape

struct ENGINE_DLL LANDSCAPE_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	using Super = CLIENT_MAKEPATH_DESC_BASE;

	_int		iIndex{0};
	/* 지형 Texture UV 좌표  */
	Vec2		vTextureUV_LT{};
	Vec2		vTextureUV_RB{};


	/* Texture */
	ID3D11ShaderResourceView* m_pTexture{};

public:
	explicit LANDSCAPE_DESC()
		: vTextureUV_LT{ Vec2::Zero }, vTextureUV_RB{Vec2::Zero}
	{
	}
	explicit LANDSCAPE_DESC(const LANDSCAPE_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), vTextureUV_RB(rhs.vTextureUV_RB) , vTextureUV_LT(rhs.vTextureUV_LT) , iIndex(rhs.iIndex)
	{
		return;
	}
	virtual ~LANDSCAPE_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion

#pragma region 식생 Plants 관련

#pragma region Plants_Desc Base

/* Plants 생성관련 Desc 작성 */
struct PLANTS_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	/* MI Free Type의 Color값을 바꿔줄 값 */
	Vec4		vMITint_Color{ 1.f, 1.f ,1.f ,1.f };
	float		fDiffuseColorPower{1.f};
public:
	PLANTS_DESC() 
		: CLIENT_MAKEPATH_DESC_BASE()
		, vMITint_Color{1.f, 1.f ,1.f ,1.f}
		, fDiffuseColorPower{1.f}
	{
	}
	PLANTS_DESC(const PLANTS_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), vMITint_Color{rhs.vMITint_Color}
		, fDiffuseColorPower{ rhs.fDiffuseColorPower }
	{

	}
	virtual ~PLANTS_DESC()
	{

	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion

#pragma region Tree
struct TREE_DESC : public PLANTS_DESC
{
public:

public:
	TREE_DESC()
		:PLANTS_DESC()
	{

	}
	TREE_DESC(const TREE_DESC& rhs)
		: PLANTS_DESC(rhs)
	{

	}
	virtual ~TREE_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion
#pragma region Moss
struct MOSS_DESC : public PLANTS_DESC
{
public:

public:
	MOSS_DESC()
		:PLANTS_DESC()
	{

	}
	MOSS_DESC(const MOSS_DESC& rhs)
		: PLANTS_DESC(rhs)
	{

	}
	virtual ~MOSS_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion
#pragma region Grass
struct GRASS_DESC : public PLANTS_DESC
{
public:
	_float fGrassDT{ 0.f };
	_float fGrassMaxHeight{ 1.f }; //이 모델의 잔디 MinMax중 Max의  Y값
	_float fGrassSwaySpeed{ 1.f }; //이 잔디가 Sway = 흔들리는 Speed
	_float fGrassWaveSize{ 1.f };  //이 잔디가 Power = 흔들리는 힘
public:
	GRASS_DESC()
		:PLANTS_DESC()
		, fGrassDT{0.f}
		, fGrassMaxHeight{1.f}
		, fGrassSwaySpeed{1.f}
		, fGrassWaveSize{1.f}
	{

	}
	GRASS_DESC(const GRASS_DESC& rhs)
		: PLANTS_DESC(rhs)
		, fGrassDT{ rhs.fGrassDT }
		, fGrassMaxHeight{ rhs.fGrassMaxHeight }
		, fGrassSwaySpeed{ rhs.fGrassSwaySpeed }
		, fGrassWaveSize{ rhs.fGrassWaveSize }
	{

	}
	virtual ~GRASS_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion
#pragma region Vine
struct VINE_DESC : public PLANTS_DESC
{
public:

public:
	VINE_DESC()
		:PLANTS_DESC()
	{

	}
	VINE_DESC(const VINE_DESC& rhs)
		: PLANTS_DESC(rhs)
	{

	}
	virtual ~VINE_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion
#pragma region Bush
struct BUSH_DESC : public PLANTS_DESC
{
public:

public:
	BUSH_DESC()
		:PLANTS_DESC()
	{

	}
	BUSH_DESC(const BUSH_DESC& rhs)
		: PLANTS_DESC(rhs)
	{

	}
	virtual ~BUSH_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion

#pragma endregion

#pragma region Batch 관련

#pragma region Batch Player = None Desc
#pragma endregion

#pragma region Batch Monster

struct ENGINE_DLL BATCH_MONSTER_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	DTO::EMakeMonsterType eBatchMonsterType{DTO::EMakeMonsterType::Dog};
public:
	BATCH_MONSTER_DESC()
		: eBatchMonsterType{DTO::EMakeMonsterType::Dog}
	{

	}
	BATCH_MONSTER_DESC(const BATCH_MONSTER_DESC& Copy)
		: eBatchMonsterType{ Copy .eBatchMonsterType }
	{

	}
	virtual ~BATCH_MONSTER_DESC() {}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion

#pragma region Batch Object

#pragma region Base
struct ENGINE_DLL BATCH_OBJECT_DESC_BASE
{
public:
	BATCH_OBJECT_DESC_BASE() {}
	BATCH_OBJECT_DESC_BASE(const BATCH_OBJECT_DESC_BASE& rhs) {}
	virtual ~BATCH_OBJECT_DESC_BASE() {}
public:
	virtual void from_Json(const json& LoadJson)	PURE;
	virtual void to_Json(json& SaveJson)			PURE;
};

inline BATCH_OBJECT_DESC_BASE* Make_BatchObject_Desc(DTO::EMakeObjectType eBatchObjectType, BATCH_OBJECT_DESC_BASE* pBase = nullptr);
#pragma endregion

#pragma region Desc
struct ENGINE_DLL BATCH_OBJECT_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	DTO::EMakeObjectType	eBatchObjectType{ DTO::EMakeObjectType::END };
	BATCH_OBJECT_DESC_BASE* pBatchObjectDesc{ nullptr };
public:
	explicit BATCH_OBJECT_DESC()
		: eBatchObjectType{ DTO::EMakeObjectType::END }, pBatchObjectDesc{ nullptr }
	{
		Change_BatchObjecType(DTO::EMakeObjectType::Battle_Field);
	}
	explicit BATCH_OBJECT_DESC(const BATCH_OBJECT_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs), eBatchObjectType{ rhs.eBatchObjectType }, pBatchObjectDesc{ nullptr }
	{
		Safe_Delete(pBatchObjectDesc);
		this->pBatchObjectDesc = Make_BatchObject_Desc(this->eBatchObjectType, rhs.pBatchObjectDesc);
		return;
	}
	virtual ~BATCH_OBJECT_DESC() { Safe_Delete(pBatchObjectDesc); }
public:
	void		 Change_BatchObjecType(DTO::EMakeObjectType eChangeType);
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion

#pragma region Battle Field

struct ENGINE_DLL BATTLE_FIELD_DESC : BATCH_OBJECT_DESC_BASE
{
public:
	enum class Field_Type { Sphere, Box ,END };
public:
	/* Radius 일때 사용할 변수 */
	BATTLE_FIELD_DESC::Field_Type	eFieldType{ Field_Type::Box};
	float							fRadius{ 1.f };
	Vec3							vExtents{1.f,1.f ,1.f};
	CCollider*						pBattleFieldColliderBox{ nullptr };
	CCollider*						pBattleFieldColliderSphere{ nullptr };
public:
	inline static	std::string		FieldType_ToString(BATTLE_FIELD_DESC::Field_Type eType)
	{
		switch (eType)
		{
		case Engine::BATTLE_FIELD_DESC::Field_Type::Sphere:		return "Sphere";
		case Engine::BATTLE_FIELD_DESC::Field_Type::Box:		return "Box";
		default:												return "Unknown";
		}

		return "Unknonw";
	}
	inline static BATTLE_FIELD_DESC::Field_Type FieldType_ToEnum(const std::string& strType)
	{
		if (strType == "Sphere")return BATTLE_FIELD_DESC::Field_Type::Sphere;
		if (strType == "Box")return BATTLE_FIELD_DESC::Field_Type::Box;

		return BATTLE_FIELD_DESC::Field_Type::END;

	}
public:
	BATTLE_FIELD_DESC();
	BATTLE_FIELD_DESC(const BATTLE_FIELD_DESC& rhs);
	virtual ~BATTLE_FIELD_DESC();
public:
	void Update_Collider(const Matrix* pWorldMatrix);
public:
	virtual void from_Json(const json& LoadJson)	override;
	virtual void to_Json(json& SaveJson)			override;
};
#pragma endregion

#pragma region Point Light
struct ENGINE_DLL POINTLIHGT_DESC : public BATCH_OBJECT_DESC_BASE
{
public:
	LIGHT_DESC	tLightDesc{};
	_bool		isFlicker{ false };		// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
	_float		fFlickerSpeed{ 1.f };	// 깜빡이는 속도
	_float		fFlickerMin{ 0.5f };	// 최소 밝이 비율
	_float		fBaseRange{1.f};		// 원래 빛 범위 저장 Update용 
	/* Debug용 */
	class CLight* pDebugLight{nullptr};
public:
	POINTLIHGT_DESC();
	POINTLIHGT_DESC(const POINTLIHGT_DESC& rhs);
	virtual ~POINTLIHGT_DESC();
public:
	void	Update_Light(const Vec4& vPos);
public:
	virtual void from_Json(const json& LoadJson)override;
	virtual void to_Json(json& SaveJson)override;
};
#pragma endregion

#pragma endregion


#pragma region Env
#pragma region Water
/* 나중에 Engine으로 옮길수도? Water관련 */
enum class EWaterTextureType
{
	Normal1,			/* 기본적인 물 움직임 1 */
	Normal2,			/* 기본적인 물 움직임 2*/
	StarLight,		/* 추가 찰랑거림 */
	Lighting,		/* 빛? */
	Noise,			/* 노이즈 석기 */

	/* Deco 최대 3개 */
	Deco1_D,
	Deco1_N,
	Deco1_ORH,

	Deco2_D,
	Deco2_N,
	Deco2_ORH,

	Deco3_D,
	Deco3_N,
	Deco3_ORH,

	END,
};

// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
static const char* g_szWaterTextureType[(int)EWaterTextureType::END] = {
	"Normal1",
	"Normal2",
	"StarLight",
	"Lighting",
	"Noise",

	"Deco1_D",
	"Deco1_N",
	"Deco1_ORH",

	"Deco2_D",
	"Deco2_N",
	"Deco2_ORH",

	"Deco3_D",
	"Deco3_N",
	"Deco3_ORH"
};

inline std::string WaterTextureType_ToString(EWaterTextureType eType)
{
	// 인덱스 초과 방지 안전장치
	if (eType >= EWaterTextureType::Normal1 && eType < EWaterTextureType::END)
		return g_szWaterTextureType[(int)eType];

	return "Unknown";
}

inline EWaterTextureType WaterTextureType_ToEnum(const std::string& strType)
{
	for (int i = 0; i < (int)EWaterTextureType::END; ++i)
	{
		if (strType == g_szWaterTextureType[i])
			return (EWaterTextureType)i;
	}
	return EWaterTextureType::END;
}

struct WATER_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	/* Binding 시켜주고있을 TextureBase 슬롯  */
	Vec4	vMI_TintColor{1.f,1.f,1.f,1.f};


	std::array<class CTextureBase*, ENUM_TO_UINT(EWaterTextureType::END)>	arrayTextureBase{};
	Vec2	vSpeed1{1.f,1.f};		/* 파동 2개 섞기 */
	Vec2	vSpeed2{1.f,1.f};		/* 파동 2개 섞기 */
	Vec2	vDistortionSpeed{ 1.f,1.f };

	Vec2    vWaterUVPower{ 1.f, 1.f };
	Vec2    vDistortionUVPower{ 1.f, 1.f };
	Vec2    vSparkleUVPower;                                              // 8 Byte (윤슬 자글자글함 크기 조절!)

	float	fDistortionPower{ 1.f };									// Noise 가 섞이는 강도
	float   fSparklePower;                                            // 4 Byte (윤슬 눈뽕 강도!)

public:
	WATER_DESC()
		: arrayTextureBase{}, vMI_TintColor{1.f,1.f,1.f,1.f }
		, vSpeed1{ 1.f }, vSpeed2{1.f}
		, vDistortionSpeed{1.f,1.f}
		, fDistortionPower{1.f}
		, vWaterUVPower{1.f,1.f}
		, vDistortionUVPower{1.f,1.f}
		, fSparklePower{1.f}
		, vSparkleUVPower{1.f,1.f}
	{
		arrayTextureBase.fill(nullptr);
	}
	WATER_DESC(const WATER_DESC& rhs);
	virtual ~WATER_DESC();
public:
	virtual void from_Json(const json& LoadJson)	override;
	virtual void to_Json(json& SaveJson)			override;
};
#pragma endregion


struct ENV_EFFECT_INFO
{
	EFFECT_ENV_DESC tDesc{};
	string			strTags{};
};

#pragma region Env Desc
struct ENGINE_DLL ENV_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
public:
	vector<ENV_EFFECT_INFO>		vecEnvEffectInfo{};
public:
	ENV_DESC()
		: vecEnvEffectInfo{}
	{ 
	}
	ENV_DESC(const ENV_DESC& rhs)
		: vecEnvEffectInfo{rhs.vecEnvEffectInfo }
	{

	}
	virtual ~ENV_DESC() {}
public:
	virtual void from_Json(const json& LoadJson)override;
	virtual void to_Json(json& SaveJson)override;
};
#pragma region Fog

enum class EFogTextureType
{
	MASK,
	Noise,
	END,
};

// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
static const char* g_szFogTextureType[(int)EWaterTextureType::END] = {
	"MASK",
	"Noise",
};

inline std::string FogTextureType_ToString(EFogTextureType eType)
{
	// 인덱스 초과 방지 안전장치
	if (eType >= EFogTextureType::MASK && eType < EFogTextureType::END)
		return g_szFogTextureType[(int)eType];

	return "Unknown";
}

inline EFogTextureType FogTextureType_ToEnum(const std::string& strType)
{
	for (int i = 0; i < (int)EFogTextureType::END; ++i)
	{
		if (strType == g_szFogTextureType[i])
			return (EFogTextureType)i;
	}
	return EFogTextureType::END;
}


struct FOG_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
	/* Binding 시켜주고있을 TextureBase 슬롯  */
	Vec4	vMI_TintColor{ 1.f,1.f,1.f,1.f };

	std::array<class CTextureBase*, ENUM_TO_UINT(EFogTextureType::END)>	arrayTextureBase{};
	Vec4	vUV[ENUM_TO_UINT(EFogTextureType::END)];

	float	fDistortionPower{ 1.f };

public:
	FOG_DESC()
		: vMI_TintColor{ 1.f , 1.f , 1.f , 1.f }
		, arrayTextureBase{}
		, fDistortionPower{ 1.f }
	{
		arrayTextureBase.fill(nullptr);
		for (_uint i = 0; i < ENUM_TO_UINT(EFogTextureType::END); ++i)
		{
			vUV[i] = Vec4(1.f,1.f,1.f,1.f);
		}
	}
	FOG_DESC(const FOG_DESC& rhs);
	virtual ~FOG_DESC();
public:
	virtual void from_Json(const json& LoadJson)	override;
	virtual void to_Json(json& SaveJson)			override;
};
#pragma endregion
#pragma endregion


#pragma region Light Object

struct ENGINE_DLL LIGHTOBJECT_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
public:
	LIGHT_DESC	tLightDesc{};

	/* 나중에 vector로 넣어줄듯  */

	_bool		isFlicker{ false };			// 깜빡일래 말래 할래 말래 할래 말래 애매하긴해~
	_float		fFlickerSpeed{ 1.f };		// 깜빡이는 속도
	_float		fFlickerMin{ 0.5f };		// 최소 밝이 비율
	_float		fBaseRange{ 1.f };			// 원래 빛 범위 저장 Update용 
	Vec3		vOffsetPosition{0.f,0.f,0.f};	// Model위치에 따른 Offset 포지션

	float       fEmissviePower{};

	/* Debug용 */
	class CLight* pDebugLight{ nullptr };
public:
	LIGHTOBJECT_DESC();
	LIGHTOBJECT_DESC(const LIGHTOBJECT_DESC& rhs);
	virtual ~LIGHTOBJECT_DESC();
public:
	void	Update_Light(const Vec4& vPos);
public:
	virtual void from_Json(const json& LoadJson)override;
	virtual void to_Json(json& SaveJson)override;
};


#pragma endregion

#pragma region Trigger Box

/* 이거 업데이트를 위차값만 받아가서하는업데트를 해야하긴 할듯 */
struct ENGINE_DLL TRIGGERBOX_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
public:
	Vec3		 vExtents{0.5f,0.5f ,0.5f};
	Vec3		 vRotation{0.f,0.f,0.f};

	_bool		 bHasQuest = { false };
	vector<DTO::QUEST_CHAPTERDESC>		tQuestObjectDesc = {};
public:
	explicit TRIGGERBOX_DESC()
		: CLIENT_MAKEPATH_DESC_BASE()
		, vExtents{ 0.5f,0.5f ,0.5f }
		, vRotation{ 0.f,0.f,0.f }
		, bHasQuest(false)
		, tQuestObjectDesc()
	{

	}
	explicit TRIGGERBOX_DESC(const TRIGGERBOX_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs)
		, vExtents(rhs.vExtents)
		, vRotation{rhs.vRotation}
		, bHasQuest(rhs.bHasQuest)
		, tQuestObjectDesc(rhs.tQuestObjectDesc)
	{
		return;
	}
	virtual ~TRIGGERBOX_DESC() {}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};


#pragma region ChangeLevel

struct ENGINE_DLL TRIGGERBOX_CHANGELEVEL_DESC : TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	std::string strChangeLevelTypeName{ "NONE" };
public:
	explicit TRIGGERBOX_CHANGELEVEL_DESC()
		: TRIGGERBOX_DESC(), strChangeLevelTypeName{"NONE"}
	{
	}
	explicit TRIGGERBOX_CHANGELEVEL_DESC(const TRIGGERBOX_CHANGELEVEL_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), strChangeLevelTypeName{rhs.strChangeLevelTypeName}
	{
		return;
	}
	virtual ~TRIGGERBOX_CHANGELEVEL_DESC() {};
public:
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion


#pragma region Spawner

class CModel;

struct  ENGINE_DLL MonsterSpawnData
{
	/* SRT */
	Vec3					vPosition{ Vec3::Zero };
	Vec3					vScale{1.f,1.f,1.f };
	Vec3					vPitchYawRoll{Vec3::Zero};

	/* 스폰할 몬스터 Type */
	DTO::EMakeMonsterType	eMakeMonsterType{DTO::EMakeMonsterType::Dog};

	/* 스폰되는 시간 값 조절 */
	float					fSpawnDelayTime{0.f};

	

	/* Debug 모델을 확인할지 말지 Checkbool값 */
	bool					isPreviewDebugModel{true};
	/* Debug용 모델 쓸미말지 미지수 */
	CModel*					pDebugModel{nullptr};

public:
	MonsterSpawnData()
		: vPosition{ Vec3::Zero }, vScale{ 1.f,1.f,1.f }, vPitchYawRoll{ Vec3::Zero }, eMakeMonsterType{ DTO::EMakeMonsterType::Dog }, fSpawnDelayTime{ 0.f }, isPreviewDebugModel{true},
		pDebugModel{nullptr}
	{

	}
	MonsterSpawnData(const MonsterSpawnData& Copy);
	virtual ~MonsterSpawnData();
public:
	void					from_Json(const json& LoadJson);
	void					to_Json(json& SaveJson);
};

struct ENGINE_DLL TRIGGERBOX_MONSTERSPAWNER_DESC : public TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	/* Monster Spawn Data */
	std::vector<MonsterSpawnData>	vecMonsterSpawnData{};

public:
	explicit TRIGGERBOX_MONSTERSPAWNER_DESC()
		: TRIGGERBOX_DESC(), vecMonsterSpawnData{}
	{
	}
	explicit TRIGGERBOX_MONSTERSPAWNER_DESC(const TRIGGERBOX_MONSTERSPAWNER_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), vecMonsterSpawnData{ rhs.vecMonsterSpawnData }
	{
		return;
	}
	virtual ~TRIGGERBOX_MONSTERSPAWNER_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

/// <summary>
/// 웨이브 스포너
/// </summary>

struct ENGINE_DLL MonsterWaveInfo
{
	_float fSpawnTime = { -1.f };
	_int iTotalSpawnCount = { -1 };
	_int iCurrentSpawnCount = { -1 };
	_float fSpawnInterval = { -1.f };
	_float fAccTime = { 0.f };
	std::vector<MonsterSpawnData>	vecMonsterSpawnData{};

public:
	explicit MonsterWaveInfo()
		: fSpawnTime{ -1.f },
		iTotalSpawnCount{ -1 },
		iCurrentSpawnCount{ -1 },
		fSpawnInterval{ -1.f },
		fAccTime{ 0.f },
		vecMonsterSpawnData{}
	{
	}
	explicit MonsterWaveInfo(const MonsterWaveInfo& Copy)
		: fSpawnTime{ Copy.fSpawnTime },
		iTotalSpawnCount{ Copy.iTotalSpawnCount },
		iCurrentSpawnCount{ Copy.iCurrentSpawnCount },
		fSpawnInterval{ Copy.fSpawnInterval },
		fAccTime{ Copy.fAccTime },
		vecMonsterSpawnData{ Copy.vecMonsterSpawnData }
	{
	}
	virtual ~MonsterWaveInfo() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

struct ENGINE_DLL TRIGGERBOX_MONSTERWAVESPAWNER_DESC : public TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	MONSTERSPAWN_WAVE_TYPE eType = MONSTERSPAWN_WAVE_TYPE::TIMER;
	_int iTotalWaveCount = { -1 };
	_int iCurrentWaveCount = { -1 };
	_float fWaveTime = { -1.f };
	_float fCurrentWaveTime = { -1.f };
	vector<MonsterWaveInfo> vecWaveInfo;

public:
	explicit TRIGGERBOX_MONSTERWAVESPAWNER_DESC()
		: TRIGGERBOX_DESC(),
		eType{ MONSTERSPAWN_WAVE_TYPE::TIMER },
		iTotalWaveCount{ -1 },
		iCurrentWaveCount{ -1 },
		fWaveTime{ -1.f },
		fCurrentWaveTime{ -1.f },
		vecWaveInfo{}
	{
	}
	explicit TRIGGERBOX_MONSTERWAVESPAWNER_DESC(const TRIGGERBOX_MONSTERWAVESPAWNER_DESC& rhs)
		: TRIGGERBOX_DESC(rhs),
		eType{rhs.eType },
		iTotalWaveCount{rhs.iTotalWaveCount },
		iCurrentWaveCount{rhs.iCurrentWaveCount },
		fWaveTime{rhs.fWaveTime },
		fCurrentWaveTime{rhs.fCurrentWaveTime },
		vecWaveInfo{ rhs.vecWaveInfo }
	{
		return;
	}
	virtual ~TRIGGERBOX_MONSTERWAVESPAWNER_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion


#pragma region GlobalEvent_BroadCaster

struct ENGINE_DLL TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC : public TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	vector<string>	vecGlobalEventBroadCasetNames{};
public:
	TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC()
		: TRIGGERBOX_DESC(), vecGlobalEventBroadCasetNames{}
	{
	}
	TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC(const TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), vecGlobalEventBroadCasetNames{ rhs.vecGlobalEventBroadCasetNames }
	{
	}
	virtual ~TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion

#pragma region TutorialUIEvent

struct ENGINE_DLL TRIGGERBOX_TUTORIALUIEVENT_DESC : public TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	string		strEventName{};
public:
	TRIGGERBOX_TUTORIALUIEVENT_DESC()
		: TRIGGERBOX_DESC(), strEventName{}
	{
	}
	TRIGGERBOX_TUTORIALUIEVENT_DESC(const TRIGGERBOX_TUTORIALUIEVENT_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), strEventName{rhs.strEventName }
	{
	}
	virtual ~TRIGGERBOX_TUTORIALUIEVENT_DESC() {};
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};

#pragma endregion

#pragma region CinematicPlayer

struct ENGINE_DLL TRIGGERBOX_CINEMATICPLAYER_DESC : public TRIGGERBOX_DESC
{
	using Super = TRIGGERBOX_DESC;
public:
	string		strCinematicName{};
public:
	TRIGGERBOX_CINEMATICPLAYER_DESC()
		: TRIGGERBOX_DESC(), strCinematicName{}
	{
	}
	TRIGGERBOX_CINEMATICPLAYER_DESC(const TRIGGERBOX_CINEMATICPLAYER_DESC& rhs)
		: TRIGGERBOX_DESC(rhs), strCinematicName{ rhs.strCinematicName }
	{
	}
	virtual ~TRIGGERBOX_CINEMATICPLAYER_DESC() {};
public:
	virtual void from_Json(const json& LoadJson)override;
	virtual void to_Json(json& SaveJson)override;
};

#pragma endregion

#pragma endregion

#pragma region NPC

struct ENGINE_DLL BATCH_NPC_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
public:
	OBJECT_ENUM_TAG::Enum eBatchNPCType{ OBJECT_ENUM_TAG::NPC_DEFAULT };

	_bool		 bHasQuest = { false };
	vector<DTO::QUEST_CHAPTERDESC>		tQuestObjectDesc = {};

	/* NPC Citizen 관련*/
	DTO::CITIZEN_DATA tNpcCitizenData{};

public:
	BATCH_NPC_DESC()
		:CLIENT_MAKEPATH_DESC_BASE(),
		eBatchNPCType(OBJECT_ENUM_TAG::NPC_DEFAULT),
		bHasQuest(false),
		tQuestObjectDesc()
		, tNpcCitizenData{}
	{

	}
	BATCH_NPC_DESC(const BATCH_NPC_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs),
		eBatchNPCType(rhs.eBatchNPCType),
		bHasQuest(rhs.bHasQuest),
		tQuestObjectDesc(rhs.tQuestObjectDesc),
		tNpcCitizenData{rhs.tNpcCitizenData}
	{

	}
	virtual ~BATCH_NPC_DESC()
	{
	}
public:
	virtual void from_Json(const json& LoadJson);
	virtual void to_Json(json& SaveJson);
};
#pragma endregion

#pragma region Interactive Object

struct ENGINE_DLL BATCH_INTERACTIVEOBJECT_DESC : public CLIENT_MAKEPATH_DESC_BASE
{
public:
	OBJECT_ENUM_TAG::Enum eBatchInteractiveObejctType{ OBJECT_ENUM_TAG::OBJECT_INTERACT_DEFAULT };	/* 생성 Path 느낌 */
	vector<DTO::QUEST_CHAPTERDESC>		vecQuestDesc = {};											/* 관련된 Quest 느낌 */
	
	/* OBJECT_ENUM_TAG::OBJECT_INTERACT_TUTORIAL_WEAPON 관련 변수*/
	string	strWeaponType{""};		  /* Type을 String으로 설정 */
	_bool	isTutorialEvent{ false }; /* Tutoiral Event 인지 아닌지 */


	/* ---------------------------------------------------------- */


	/* OBJECT_ENUM_TAG::OBJECT_INTERACT_CHANGELEVEL 관련 변수*/
	std::string strChangeLevelTypeName{ "NONE" };


	/* ---------------------------------------------------------- */

public:
	BATCH_INTERACTIVEOBJECT_DESC()
		:CLIENT_MAKEPATH_DESC_BASE()
		, vecQuestDesc{}
		, eBatchInteractiveObejctType{ OBJECT_ENUM_TAG::OBJECT_INTERACT_DEFAULT }
		, strWeaponType{}
		, isTutorialEvent{ false }
		, strChangeLevelTypeName{ "NONE" }
	{

	}
	BATCH_INTERACTIVEOBJECT_DESC(const BATCH_INTERACTIVEOBJECT_DESC& rhs)
		: CLIENT_MAKEPATH_DESC_BASE(rhs)
		, vecQuestDesc{ rhs.vecQuestDesc }
		, eBatchInteractiveObejctType{ rhs.eBatchInteractiveObejctType }
		, strWeaponType{ rhs.strWeaponType }
		, isTutorialEvent{ rhs.isTutorialEvent }
		, strChangeLevelTypeName{ rhs.strChangeLevelTypeName }
	{

	}
	virtual ~BATCH_INTERACTIVEOBJECT_DESC()
	{
	}
public:
	void		 Change_BatchInteractiveObjectType(OBJECT_ENUM_TAG::Enum eType);
	void		 Reset_PlusDesc();
public:
	virtual void from_Json(const json& LoadJson)override;
	virtual void to_Json(json& SaveJson)override;
};


#pragma endregion

#pragma endregion


NS_END

NS_BEGIN(DTO)
/////////////////-------------------  MAP  -------------------/////////////////


enum class EMapObject_Type : _uint
{
	MAPOBJECT,
	SCENEDATA,
	END
};

enum class EClientLevelType : unsigned int
{
	STATIC = 0,
	LOADING,
	LOGO,				/* 현재 임시 Test Level용 추후 Logo Scene으로 바뀔예정  */
	TUTORIAL_VILLAGE,	/* 튜토리얼 처음 진입되는 Level */
	TUTORIAL_BOSS,		/* 튜토리얼 진입 이후 Boss전 가는 Level Type */
	SQUARE,				/* 광장 */
	TAVERN,				/* 술집 */
	KUANGKENG,			/* 갱도 */
	LIANHUO,			/* 지옥불 교도소장 보스 */


	TEST,				/* Test Scene은 맨 마지막 */
	END
};

enum class EMapObject_DrawType
{
	Collider,
	Default,
	Instance,
	END
};

enum class EClientMakePath
{
	StaticObject,
	LandScape,
	Bush,
	Grass,
	Moss,
	Tree,
	Vine,
	Rock,
	Water,
	Env,
	LightObject,

	/* 몬스터 , Player 위치잡는 용도  */
	Batch_Player,
	Batch_Monster,
	Batch_Object,
	Batch_NPC,
	Batch_InteractiveObject,

	/* Trigger Box 관련 */
	TriggerBox_ChangeLevel,
	TriggerBox_MonsterSpawner,
	TriggerBox_MonsterWaveSpawner,
	TriggerBox_GlobalEvent_BroadCaster,
	TriggerBox_TutorialUIEvent,
	TriggerBox_CinematicPlayer,


	/* 맵 기능 관련 */
	Invisible_Wall,			/* 플레이어나 오브젝들이 못가게막아주는 투명벽 */
	END
};


inline constexpr _uint g_MapObjectTypeCount{ ENUM_TO_UINT(EMapObject_Type::END) };
inline constexpr _uint g_MapObjecTDrawTypeCount{ ENUM_TO_UINT(EMapObject_DrawType::END) };
inline constexpr _uint g_ClientLevelType{ ENUM_TO_UINT(EClientLevelType::END) };
inline constexpr _uint g_ClientMakePath{ ENUM_TO_UINT(EClientMakePath::END)};

/* String으로 자동 변환되어서 저당된다  */

NLOHMANN_JSON_SERIALIZE_ENUM(EMapObject_Type,
	{
		{EMapObject_Type::MAPOBJECT, "MAPOBJECT"},
		{EMapObject_Type::END, "UnKnown"},
	}
	)

NLOHMANN_JSON_SERIALIZE_ENUM(EClientLevelType,
		{
			{EClientLevelType::STATIC,				"STATIC"},
			{EClientLevelType::LOADING,				"LOADING"},
			{EClientLevelType::LOGO,				"LOGO"},
			{EClientLevelType::TUTORIAL_VILLAGE,	"TUTORIAL_VILLAGE"},
			{EClientLevelType::TUTORIAL_BOSS,		"TUTORIAL_BOSS"},
			{EClientLevelType::SQUARE,				"SQUARE"},
			{EClientLevelType::KUANGKENG,			"KUANGKENG"},
			{EClientLevelType::TAVERN,				"TAVERN"},
			{EClientLevelType::LIANHUO,				"LIANHUO"},
			{EClientLevelType::TEST,				"TEST"},
		}
		)
NLOHMANN_JSON_SERIALIZE_ENUM(EMapObject_DrawType,
		{
			{EMapObject_DrawType::Collider, "Collider"},
			{EMapObject_DrawType::Default,	"Default"},
			{EMapObject_DrawType::Instance,	"Instance"},
			{EMapObject_DrawType::END,		"END"},
		}
		)

	NLOHMANN_JSON_SERIALIZE_ENUM(EClientMakePath,
		{
			{EClientMakePath::StaticObject,							"StaticObject"},
			{EClientMakePath::LandScape,							"LandScape"},
			{EClientMakePath::Bush,									"Bush"},
			{EClientMakePath::Grass,								"Grass"},
			{EClientMakePath::Moss,									"Moss"},
			{EClientMakePath::Tree,									"Tree"},
			{EClientMakePath::Vine,									"Vine"},
			{EClientMakePath::Rock,									"Rock"},
			{EClientMakePath::Water,								"Water"},
			{EClientMakePath::Env,									"Env"},
			{EClientMakePath::LightObject,							"LightObject"},


			{EClientMakePath::Batch_Player,							"Batch_Player"},
			{EClientMakePath::Batch_Monster,						"Batch_Monster"},
			{EClientMakePath::Batch_Object,							"Batch_Object"},
			{EClientMakePath::Batch_InteractiveObject,				"Batch_InteractiveObject"},


			{EClientMakePath::TriggerBox_ChangeLevel,				"TriggerBox_ChangeLevel"},
			{EClientMakePath::TriggerBox_MonsterSpawner,			"TriggerBox_MonsterSpawner"},
			{EClientMakePath::TriggerBox_MonsterWaveSpawner,		"TriggerBox_MonsterWaveSpawner"},
			{EClientMakePath::TriggerBox_GlobalEvent_BroadCaster,	"TriggerBox_GlobalEvent_BroadCaster"},
			{EClientMakePath::TriggerBox_TutorialUIEvent,			"TriggerBox_TutorialUIEvent"},
			{EClientMakePath::TriggerBox_CinematicPlayer,			"TriggerBox_CinematicPlayer"},

			{EClientMakePath::Invisible_Wall,						"Invisible_Wall"},

			{EClientMakePath::Batch_NPC,							"Batch_NPC"},

			{EClientMakePath::END,									"Unknown"},
		}
		)

	/////////////////-------------------  ObjectStruct  -------------------/////////////////

#pragma region Map Data 구조체


#pragma region SRT Data
	struct SRT_DATA
{
	Vec3	vScale{1.f,1.f,1.f};
	Quat	vQuat{0.f,0.f,0.f,1.f};
	Vec3	vPosition{ 0.f,0.f,0.f };
	Vec3	vScale_Isolated{}; //TEST: 소재혁 임시 추가
public:
	Matrix  Get_World() const
	{
		return Matrix::CreateScale(vScale) * Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(vPosition);
	}
};
#pragma endregion

#pragma region Using Material 

typedef struct tagOverrideMaterials
{
	bool	isNull{ false };

	/* Override 할때 참조하고 있는 파일 Path 값 */
	wstring wstrMtl_JsonFile_Name{};
	wstring wstrMtl_JsonFile_Path{};
	/* 그 안에서 뜯어낸 Texutre 바인딩 이름 : Texutre 경로 [ 메테리얼 Json 경로에 꽃아줄 이름 ] */
	vector < std::pair<wstring, wstring>> vecUsingTextureInfo{};
}OVERRIDE_MATERIALS;
#pragma endregion
#pragma region Using Model
typedef struct tagUsingModelInfo
{
	wstring wstrName{};
	wstring wstrPath{};

	/* 모델이 생성되고 난 이후에 저장되는 메테리얼 경로 */
	wstring wstrMtl_JsonFile_Path{};

	vector<OVERRIDE_MATERIALS> vecOverrideMaterial{};
public:

}USING_MODEL_INFO;
#pragma endregion

#pragma region MapObject
typedef struct TMap_MapObjectData
{
	/* UE Load Check */
	_uint								iSectionNum{};
	bool								isUELoaded{ false };
	string								strUERawDataPath{""};
	string								strTag{};
	string								strModelPath{ "" };

	/* Client Make Level Type */
	EClientMakePath						eClientMakePath{EClientMakePath::StaticObject};
	EClientLevelType					eClientLevelType{EClientLevelType::STATIC};
	EMapObject_DrawType					eMapObjectDrawType{EMapObject_DrawType::Default};

	/* SRT Data , Cient Make Path Desc */
	vector<SRT_DATA>					vecSRTs{};
	vector<CLIENT_MAKEPATH_DESC_BASE*>	vecClientMakePathDesc{};
}MAPOBJECTDATA;
#pragma endregion



#pragma endregion


#pragma region Level Data

typedef struct TLevelData
{
	string								strTag{};
	string								strTextureSplatingInfoName{ "None" };
	string								strLevelTypeName{ "STATIC" };


	/* 환경 데이터 */

	/* 환경 Color */
	Vec4								vEnvColor{ 1.f,1.f,1.f,1.f };				// 16Byte 환경 Color

	/* Wind */
	Vec3								vWindDirection{ 1.f,0.f, 1.f }; //바람이 부는 방향
	_float								fWindPower{ 1.f }; //바람이 부는 새기

	/* SkyBox */

	Vec4								vSkyColor{ 0.3f,0.7f,0.8f,1.f };
	Vec4								vCloudBaseColor{ 0.8f,0.8f,0.8f,1.f };
	Vec4								vCloudHighlight{ 1.f,1.f,1.f,1.f };
	Vec4								vCloudShadowColor{ 1.f,1.f,1.f,1.f };

	_float								fCloudHighlightPower{1.f};
	_float								fCloudShadowPower{1.f};


	Vec3								vSkyBoxScale{1.f,1.f,1.f};
	Vec3								vSkyBoxPositionOffset{0.f,0.f,0.f};
	Vec3								vSkyBoxPitchYawRoll{0.f,0.f,0.f};
	string								strSkyBoxModelName{ "None" };
	string								strSKyBoxTextureName{ "None" };
	Vec2								vSkyBoxTextureUVSpeed{1.f,1.f};
	_int								iSkyBoxTextureType{ 0 };					// 4Byte 텍스처가 원형용텍스처인지 , 사각형용 텍스처인지
	_bool								isSkyBoxChannelPacking{ false };			// 4Byte 채널 패킹을 사용하는지 않나는지
	_float								fPolarRadiusScale{ 1.f };                   // 4Byte 여백 조절용 , 기본값 1.0


	/* Map Min Max Box */
	Vec3								vMapMinMaxBox_Center{ 0.f,0.f,0.f };
	Vec3								vMapMinMaxBox_extents{ 1.f,1.f,1.f };

	/*-------*/

}SCENEDATA;

#pragma endregion

/////////////////-------------------  to_json, from_json  -------------------/////////////////

#pragma region SRT Data
inline void to_json(json& SaveJson, const SRT_DATA& tData);
inline void from_json(const json& LoadJson, SRT_DATA& tdata);
#pragma endregion 

#pragma region Using Material 

void from_json(const json& LoadJson, OVERRIDE_MATERIALS& tData);

void to_json(json& SaveJson, const OVERRIDE_MATERIALS& tData);

#pragma endregion

#pragma region Using Model
void from_json(const json& LoadJson, USING_MODEL_INFO& tData);
void to_json(json& SaveJson, const USING_MODEL_INFO& tData);

#pragma endregion

#pragma region Map Object
inline void to_json(json& SaveJson, const TMap_MapObjectData& tData);
inline void from_json(const json& LoadJson, TMap_MapObjectData& tData);
#pragma endregion


#pragma region Level Data
inline void to_json(json& SaveJson, const TLevelData& tData);
inline void from_json(const json& LoadJson, TLevelData& tData);
#pragma endregion


NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////
NS_BEGIN(Engine)

#pragma region Mapobject

class CData_MapObject final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
	using Super = IObjectDataBase;
private:
	CData_MapObject() = default;
	virtual ~CData_MapObject() = default;
public:

public:
	_uint							Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT);}
	const string&					Get_Tag() const override { return m_tData.strTag; }
	
	json							ToJson() const override;
	HRESULT							FromJson(const json& j) override;

	const DTO::TMap_MapObjectData&	Get_Data() const { return m_tData; }
	DTO::TMap_MapObjectData&		Get_Data() { return m_tData; }
private:
	DTO::TMap_MapObjectData			m_tData{};
public:
	static CData_MapObject* Create() { return new CData_MapObject(); }
	virtual void Free() override;
};

#pragma endregion

#pragma region Level Data

class CData_LevelData final : public IObjectDataBase
{
	// IObjectDataBase을(를) 통해 상속됨
	using Super = IObjectDataBase;
private:
	CData_LevelData() = default;
	virtual ~CData_LevelData() = default;
public:

public:
	_uint							Get_Type() const override { return ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA); }
	const string&					Get_Tag() const override { return m_tData.strTag; }

	json							ToJson() const override;
	HRESULT							FromJson(const json& j) override;

	const DTO::TLevelData&			Get_Data() const { return m_tData; }
	DTO::TLevelData&				Get_Data() { return m_tData; }
private:
	DTO::TLevelData					m_tData{};
public:
	static CData_LevelData* Create()
	{
		return new CData_LevelData();
	}
	virtual void Free() override;
};
#pragma endregion

NS_END




NS_BEGIN(Engine)

ENGINE_DLL CLIENT_MAKEPATH_DESC_BASE* Create_ClientMakePathDesc(DTO::EClientMakePath ePath, CLIENT_MAKEPATH_DESC_BASE* pSource =nullptr);
ENGINE_DLL _bool					  IsExist_ClientMakePathDesc(DTO::EClientMakePath ePath);

NS_END


