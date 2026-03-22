#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	//===================
	// Input
	//===================
	enum class MOUSEKEYSTATE : unsigned int { LB, RB, WHEEL, XB, END };
	enum class MOUSEMOVESTATE : unsigned int { X, Y, WHEEL, END };
	enum class WINMODE { FULL, WIN };
	enum class TRANSFORM_INFO_STATE { RIGHT, UP, LOOK, POS, END };
	enum class LIGHT_TYPE : unsigned int { DIRECTIONAL, POINT , END };
	enum class EPOINT { A, B, C, END };
	enum class ELINE { AB, BC, CA, END };
	enum class RENDER_CATEGORY : unsigned int { PRIORITY, BLEND,NONEBLEND , COMPUTELIGHT_BLEND , NONELIGHT, ENVIRONMENT, DISTOTION, BLENDUI, UI, SHADOW_DYNAMIC, END };
	enum class DEFFERRED { DEBUG, DIRECTIONAL, POINT, OUTLINE, SSAO_GEN, SSAO_BLURH, SSAO_BLURV, SSAO_UPSAMPLE, COMBINED, BLOOM_EXTRACT, BLOOM_BLURH, BLOOM_BLURV, TONEMAP, WBOIT, END };
	enum class ECursorMode : unsigned int
	{
		LockedHiddenCenter = 0,
		VisibleClipped,
		VisibleFree,
		Tool,
		END
	};

	//===================
	// GameObject
	//===================
	enum class GAMEOBJECT_TYPE : unsigned int
	{
		TERRAIN = 0,
		END
	};
	//===================
	// Camera
	//===================
	enum class EProjectionType : unsigned int
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
		END
	};
	//===================
	// Camera
	//===================
	enum class EColliderType : unsigned int
	{
		SPHERE = 0,
		AABB,
		OBB,
		//SPHERE_GROUP,
		END
	};
	//===================
	// Prototype
	//===================
	enum class EPrototypeType : unsigned int
	{
		GAMEOBJECT,
		COMPONENT,
		END
	};
	//===================
	// FX Scalar
	//===================
	enum class EFXScalar : unsigned int
	{
		MaterialMask = 0,
		GlobalMask,
		COUNT
	};
	constexpr const char* g_ScalarNames[static_cast<unsigned int>(EFXScalar::COUNT)] =
	{
		"g_iMaterialMask",
		"g_iGlobalMask"
	};
	//===================
	// FX CB
	//===================
	enum class EFXCB : unsigned int
	{
		Global = 0,
		Inv,
		Transform,
		Light,
		Material,
		MaterialInst,
		ObjectInfo,
		Keyframe,
		Bone,
		Effect,
		SSAOkernal,
		SSAOparam,
		HDRparam,
		Bloomparam,
		Outlineparam,
		RGBMapping,
		RenderFx,
		Fogparam,
		Toonparam,
		Cascadeparam,
		BakedShadowparam,
		SectionShadowparam,
		PlayerInfoBuffer,
		COUNT
	};
	constexpr const char* g_CBNames[static_cast<unsigned int>(EFXCB::COUNT)] =
	{
		"GlobalBuffer",
		"InvBuffer",
		"TransformBuffer",
		"LightBuffer",
		"MaterialBuffer",
		"MaterialInstanceBuffer",
		"ObjectInfoBuffer",
		"KeyframeBuffer",
		"BoneBuffer",
		"ConstantBuffer_Effect",
		"SSAOKernelBuffer",
		"SSAOParamBuffer",
		"HDRParamBuffer",
		"BLOOMParamBuffer",
		"OUTLINEParamBuffer",
		"CB_MAPPING_RGB",
		"RenderFxParamBuffer",
		"FogParamBuffer",
		"ToonParamBuffer",
		"CascadeParamBuffer",
		"BakedShadowParamBuffer",
		"ShaderBakedSectionBuffer",
		"PlayerInfoBuffer"
	};
	//===================
	// FX SRV
	//===================
	enum class EFXSRV : unsigned int
	{
		RT = 0,
		RT_Diffuse,
		RT_Normal,
		RT_Shade,
		RT_SpecularMask,
		RT_Specular,
		RT_Depth,
		RT_ObjectInfo,
		RT_Emissive,
		RT_AO,
		RT_SceneHDR,
		RT_SceneHDR_Copy,
		RT_Bloom,
		RT_Cascade0,
		RT_Cascade1,
		RT_ShadowBaked,
		LUT_Stand,
		Transform,
		Materials,
		Textures,
		Cube,
		SSAONoise,
		PerlinNoise,
		RT_OIT_Accum,
		RT_OIT_REVEAL,
		COUNT
	};
	constexpr const char* g_SRVNames[static_cast<unsigned int>(EFXSRV::COUNT)] =
	{
		"g_RenderTargetTexture",
		"g_RenderTargetDiffuseTexture",
		"g_RenderTargetNormalTexture",
		"g_RenderTargetShadeTexture",
		"g_RenderTargetSpecularMaskTexture",
		"g_RenderTargetSpecularTexture",
		"g_RenderTargetDepthTexture",
		"g_RenderTargetObjInfoTexture",
		"g_RenderTargetEmissiveTexture",
		"g_RenderTargetAOTexture",
		"g_RenderTargetSceneHDRTexture",
		"g_RenderTargetSceneHDRCopyTexture",
		"g_RenderTargetBloomTexture",
		"g_RenderTargetCascadeShadowmap0",
		"g_RenderTargetCascadeShadowmap1",
		"g_RenderTargetShadowBaked",
		"g_LUT_Stand",
		"g_TransformMap",
		"g_MaterialTextures",
		"g_DefaultTextures",
		"g_TextureCube",
		"g_SSAONoiseTexture",
		"g_PerlinNoise",
		"g_RenderTargetOITAccumTexture",
		"g_RenderTargetOITRevealTexture"
	};
	//===================
	// Sound
	//===================
	enum class ESoundCategory : int
	{
		BGM = 0,
		SFX,
		UI,
		Voice,
		Ambient,
		END
	};

	enum class EControlledChannel : unsigned int
	{
		BGM = 0,
		AMBIENT,
		COUNT,
	};

	//===================
	// AnimEvent
	//===================
	enum class EAnimNotifyId : unsigned int
	{
		CollisionOn = 0,
		CollisionOff,
		FootStepL,
		FootStepR,
		Vfx_Oneshot,
		Vfx_Attach_On,
		Vfx_Attach_Off,
		Hitbox,
		Trigger_Gimmik,
		Sound,
		END
	};
	//===================
	// AnimEventPhase
	//===================
	enum class EAnimNotifyPhase : unsigned int
	{
		Immediatley = 0,
		Late,
		PreRender,
		END
	};
	inline constexpr size_t g_AnimNotifyPhaseTypeCount = static_cast<size_t>(EAnimNotifyPhase::END);

	//===================
	// Component
	//===================
	enum class EComponentType : unsigned int
	{
		TRANSFORM = 0,
		NAVIGATION,
		TEXTURE,
		COLLIDER,
		VIBUFFER,
		CONTROLCONTEXT,
		ACTIONSTATE,
		CAMERA,
		SHADER,
		BOUND,
		STAT,
		ACTIONSKILL,
		GIMMIKCTRL,
		RENDER_FX,
		SOUNDHANDLER,
		//
		PX_RIGIDBODY,
		PX_COLLIDER,
		PX_CCT,
		PX_ATTACKOVERLAP,
		PX_ATTACKRAYCAST,
		PX_RAGDOLL,
		PX_SPRINGARM,
		// 이펙트
		EF_ANIMHANDLER,
		//
		MODEL,
		SCRIPT,
		
		END
	}; 
	inline constexpr size_t g_ComponentTypeCount = static_cast<size_t>(EComponentType::END) - 1;
	//===================
	// ResourceType
	//===================
	enum class EResourceType : unsigned int
	{
		TEXTURE,
		MATERIAL,
		MATERIAL_INSTANCE,
		ANIMATION,
		MODEL_ANIMATION,
		END
	};
	inline constexpr size_t g_ResourceTypeCount = static_cast<size_t>(EResourceType::END);

	//===================
	// Model
	//===================
	enum class EModelType : unsigned int
	{
		NONANIM,
		ANIM,
		CUSTOMPARTS,
		BONE,
		STATIC,
		END
	};
	//===================
	// MaterialInstance
	//===================
	enum class EMaterialInstanceType : unsigned int
	{
		Default,
		Concrete,
		Mirror,
		Water,
		Dirt,
		Red,
		Blue,
		Green,
		Grass,
		Orange,
		Brown,
		Pupple,
		WinterGreen,
		BurnishedBrown,
		ConcreteLight,
		ConcreteMid,
		ConcreteWarm,
		MetalCool,
		MetalDark,
		CourtBlue,
		

		/* 자유타입 Render직전 내가 지정한 컬러값으로 덮어써줄것 */
		Free,
		END
	};
	//===================
	// MovementMode
	//===================
	enum class EMovementMode : unsigned int
	{
		GROUND,
		WALL,
		END
	};
	//===================
	// MovementMode
	//===================
	enum class EFrustrumTier : unsigned int
	{
		Near = 0,
		Mid,
		Far,
		None
	};
	//===================
	// MaterialType
	//===================
	enum class EMaterialTextureType : unsigned int
	{
		DIFFUSE = 0,
		SPECULAR = 1,
		AMBIENT = 2,
		EMISSIVE = 3,
		HEIGHT = 4,
		NORMALS = 5,
		SHININESS = 6,
		OPACITY = 7,
		DISPLACEMENT = 8,
		LIGHTMAP = 9,
		REFLECTION = 10,
		BASE_COLOR = 11,
		NORMAL_CAMERA = 12,
		EMISSION_COLOR = 13,
		METALNESS = 14,
		DIFFUSE_ROUGHNESS = 15,
		AMBIENT_OCCLUSION = 16,
		UNKNOWN = 17,
		MAX_COUNT = 18
	};
	// ===================
	// PhysicsShape
	// ===================
	enum class EPhysicsShape : unsigned int
	{
		SPHERE,
		CAPSULE,
		BOX,
		PLANE,
		HEIGHT_FIELD,
		END
	};
	//===================
	// PhysicsConvexShape
	//===================
	enum class EPhysicsConvexShape : unsigned int
	{
		POINT,
		SEGMENT,
		BOX,
		ELLIPSOID,
		CYLINDER,
		CONE,
		END
	};
	//===================
	// PhysicsMaterial
	//===================
	enum class EPhysicsMaterial : unsigned int
	{
		CUSTOM,
		PLAYER,
		DEFAULT,
		CONCRETE,
		ICE,
		WOOD,
		RUBBER,
		METAL,
		GLASS,
		NONE,
		END
	};
	//===================
	// PhysicsActorType
	//===================
	enum class EPhysicsActorType : unsigned int
	{
		STATIC,
		DYNAMIC,
		KINEMATIC,
		END
	};
	//===================
	// PhysicsCCTType
	//===================
	enum class EPhysicsCCTType : unsigned int
	{
		BOX,
		CAPSULE,
		END
	};
	//===================
	// PhysicsCollisionDetection
	//===================
	enum class EPhysicsCollisionDetection : unsigned int
	{
		DISCRETE,
		CONTINNUOUS,
		CONTINUOUS_DYNAMIC,
		CONTINUOUS_SPECULATIVE,
		END
	};

	typedef struct ECollisionEvent
	{
		enum Enum
		{
			ON_COLLISION_ENTER,
			ON_COLLISION_STAY,
			ON_COLLISION_EXIT,
			ON_TRIGGER_ENTER,
			ON_TRIGGER_EXIT,
			END
		};
	}COLLISIONEVENT;

	typedef struct EPhysicsFilterGroup
	{
		enum Enum : unsigned int
		{
			PLAYER = 1 << 0,
			ATTACK = 1 << 1,
			SKILL = 1 << 2,
			ATTACK_PROJECTTILE = 1 << 3,
			SKILL_PROJECTTILE = 1 << 4,

			MONSTER = 1 << 5,
			MONSTER_ATTACK = 1 << 6,
			MONSTER_SKILL = 1 << 7,
			MONSTER_ATTACK_PROJECTTILE = 1 << 8,
			MONSTER_SKILL_PROJECTTILE = 1 << 9,

			MAP = 1 << 10,

			OBJECT1 = 1 << 11,
			OBJECT2 = 1 << 12,

			TRIGGER_UI = 1 << 13,
			TRIGGER_QUEST = 1 << 14,
			TRIGGER_SPAWN = 1 << 15,
			TRIGGER_DIRECTION = 1 << 16,
			TRIGGER_BOX = 1 << 17,

			INVISIBLE_WALL = 1 << 18,
			DETECT_MONSTER = 1 << 19,

			RAGDOLL = 1 << 20,

			NPC = 1 << 21,

			DETECT_INTERACT = 1 << 22,

			GENIEMON = 1 << 23,

			NONE = 1 << 24,
			END
		};

		static bool IsPlayer(unsigned int iFlag) { return (iFlag & PLAYER) != 0; }
		static bool IsMonster(unsigned int iFlag) { return (iFlag & MONSTER) != 0; }
		static bool IsMap(unsigned int iFlag) { return (iFlag & MAP) != 0; }

		static bool IsTrigger(unsigned int iFlag)
		{
			static constexpr unsigned int iTriggerLayer
			{
				TRIGGER_UI | TRIGGER_QUEST | TRIGGER_SPAWN | TRIGGER_DIRECTION | TRIGGER_BOX
			};
			return (iFlag & iTriggerLayer) != 0;
		}

		static bool IsAttackLayer(unsigned int iFlag)
		{
			static constexpr unsigned int iAttackLayer
			{
				ATTACK | SKILL | ATTACK_PROJECTTILE | SKILL_PROJECTTILE |
				MONSTER_ATTACK | MONSTER_SKILL | MONSTER_ATTACK_PROJECTTILE | MONSTER_SKILL_PROJECTTILE
			};
			return (iFlag & iAttackLayer) != 0;
		}

		// 공격 쪽이냐? ( 한쪽은 Attack관련, 한쪽은 Monster, Player 몸체 )
		static bool IsAttackPair(unsigned int iFlagA, unsigned int iFlagB)
		{
			const bool bAttack_A = IsAttackLayer(iFlagA);
			const bool bAttack_B = IsAttackLayer(iFlagB);
			// 둘다 공격쪽이거나 아니라면
			if (bAttack_A == bAttack_B)
				return false;

			const unsigned int iVictim = bAttack_A != true ? iFlagA : iFlagB;

			return IsPlayer(iVictim) || IsMonster(iVictim);
		}

		static bool IsTriggerPair(unsigned int iFlagA, unsigned int iFlagB)
		{
			return IsTrigger(iFlagA) || IsTrigger(iFlagB);
		}
	}PHYSICSFILTERGROUP;

	struct EPhysicsFilterType
	{
		enum Enum : unsigned int
		{
			PLAYER,
			ATTACK,
			SKILL,
			ATTACK_PROJECTTILE,
			SKILL_PROJECTTILE,

			MONSTER,
			MONSTER_ATTACK,
			MONSTER_SKILL,
			MONSTER_ATTACK_PROJECTTILE,
			MONSTER_SKILL_PROJECTTILE,

			MAP,

			OBJECT1,
			OBJECT2,

			TRIGGER_UI,
			TRIGGER_QUEST,
			TRIGGER_SPAWN,
			TRIGGER_DIRECTION,

			NONE,
			END
		};
	};

	//===================
	// Animation Event
	//===================
	struct EAnimEvent
	{
		enum Enum
		{
			OVERLAP,
			EFFECT,
			SOUND,
			NONE,
			END
		};
	};

	//===================
	// Attack overlap type
	//===================
	struct EOverlapType
	{
		enum Enum
		{
			BOX,
			SPHERE,
			CAPSULE,
			END
		};
	};

	//===================
	// Font Pivot Type
	//===================
	enum class EFontPivotType
	{
		CENTER,
		LEFT,
		RIGHT,
		UP,
		DOWN,

		LT,CT,RT,LC,RC,LD,CD,RD,

		END
	};
	//===================
	// Font Shader Type
	//===================
	enum class EFontShaderType
	{
		NORMAL, 
		OUTLINE, 
		NOISE,
		NOISE_KOR,
		OUTLINE_NOISE, 
		OUTLINE_NOISE_KOR,
		GRADATION, 
		OUTLINE_GRADATION,
		HIT, 
		NOISE_NOSCROLL,
		OUTLINE_NOISE_NOSCROLL,
		END
	};

	//===================
	// Skill Type
	//===================
	enum class SKILL_TYPE { DAMAGE, BUFF, SUMMON, CURE, DEFENSE, END }; // skill의 타입

	//===================
	// EFFECT Type
	//===================
	enum class E_EFFECT_TYPE { LOCAL, WORLD, END }; // skill의 타입

	//===================
	// Physics ragdoll joint
	//===================
	typedef struct ERagdollJoint
	{
		enum Enum
		{
			PELVIS,
			SPINE_02,
			HEAD,
			UPPERARM_L,
			LOWERARM_L,
			UPPERARM_R,
			LOWERARM_R,
			THIGH_L,
			CALF_L,
			FOOT_L,
			THIGH_R,
			CALF_R,
			FOOT_R,
			END
		};
	}RAGDOLLJOINT;

	//===================
	// Object enum tag
	//===================
	// https://docs.google.com/spreadsheets/d/1CGzyk6tjHByXM0LA-vXfRlovtc3Dpag1Fr7vBfB51BE/edit?usp=sharing
	typedef struct EObjectEnumTag
	{
		enum Enum
		{
			// 0 플레이어
			PLAYER = 0,

			// 1 ~ 999 NPC
			// NPC_
			NPC_DEFAULT = 1,
			NPC_PAN = 2,
			NPC_BERENICA = 3,

			// 1000 ~ 1999 오브젝트
			// OBJECT_
			OBJECT_DEFAULT = 1000,

			// 2000 ~ 2499 채집형 오브젝트
			// OBJECT_PICKING_
			OBJECT_PICKING_DEFAULT = 2000,
			OBJECT_PICKING_FLOWER_1 = 2001,

			// 2500 ~ 2999 상호작용 오브젝트
			// OBJECT_INTERACT_
			OBJECT_INTERACT_DEFAULT = 2500,
			OBJECT_INTERACT_WEAPONPICKUP = 2501,

			OBJECT_INTERACT_END = 2999,
			// 3000 ~ 3999 TRIGGER BOX
			// 3100 ~ 3199 TRIGGER BOX : LEVEL CAHNGE
			// 3200 ~ 3299 TRIGGER BOX : GLOBAL EVENT BROAD CASTER
			// 3300 ~ 3399 TRIGGER BOX : TUTORIAL UI EVENT
			// 3400 ~ 3499 TRIGGER BOX : MILESTONE 퀘스트 이정표
			// TRIGGER_BOX_
			TRIGGER_BOX_DEFAULT = 3000,

			TRIGGER_BOX_TO_DEFAULT = 3100,
			TRIGGER_BOX_TO_TUTORIAL_VILAGE = 3101,
			TRIGGER_BOX_TO_TUTORIAL_BOSS = 3102,

			TRIGGER_BOX_GLOBAL_EVENT_DEFAULT = 3200,

			TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT = 3300,

			TRIGGER_BOX_MILESTONE_DEFAULT = 3400,
			
			// 10000 ~ 19999 몬스터
			// MONSTER_
			MONSTER_DEFAULT = 10000,
			MONSTER_DOG = 10001,
			MONSTER_FLY = 10002,
			MONSTER_BOOMER = 10003,

			// 30000 ~ 39999 앨리트 몬스터
			MONSTER_ELITE_DEFAULT = 30000,

			// 40000 ~ 49999 보스 몬스터
			MONSTER_BOSS_DEFAULT = 40000,
			MONSTER_BOSS_XIBI = 40001,

			// 99999 Default
			DEFAULT = 99999,
		};
	public:
		static Enum		ToEnum(const string& strType)
		{
			if (strType == "PLAYER") return OBJECT_ENUM_TAG::PLAYER;

			// NPC
			if (strType == "NPC_DEFAULT") return OBJECT_ENUM_TAG::NPC_DEFAULT;
			if (strType == "NPC_PAN") return OBJECT_ENUM_TAG::NPC_PAN;
			if (strType == "NPC_BERENICA") return OBJECT_ENUM_TAG::NPC_BERENICA;

			// OBJECT
			if (strType == "OBJECT_DEFAULT") return OBJECT_ENUM_TAG::OBJECT_DEFAULT;

			// OBJECT_PICKING
			if (strType == "OBJECT_PICKING_DEFAULT") return OBJECT_ENUM_TAG::OBJECT_PICKING_DEFAULT;
			if (strType == "OBJECT_PICKING_FLOWER_1") return OBJECT_ENUM_TAG::OBJECT_PICKING_FLOWER_1;

			// OBJECT_INTERACT
			if (strType == "OBJECT_INTERACT_DEFAULT") return OBJECT_ENUM_TAG::OBJECT_INTERACT_DEFAULT;
			if (strType == "OBJECT_INTERACT_WEAPONPICKUP") return OBJECT_ENUM_TAG::OBJECT_INTERACT_WEAPONPICKUP;

			// TRIGGER_BOX
			if (strType == "TRIGGER_BOX_DEFAULT") return OBJECT_ENUM_TAG::TRIGGER_BOX_DEFAULT;
			if (strType == "TRIGGER_BOX_TO_DEFAULT") return OBJECT_ENUM_TAG::TRIGGER_BOX_TO_DEFAULT;
			if (strType == "TRIGGER_BOX_TO_TUTORIAL_VILAGE") return OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_VILAGE;
			if (strType == "TRIGGER_BOX_TO_TUTORIAL_BOSS") return OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_BOSS;
			if (strType == "TRIGGER_BOX_GLOBAL_EVENT_DEFAULT") return OBJECT_ENUM_TAG::TRIGGER_BOX_GLOBAL_EVENT_DEFAULT;
			if (strType == "TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT") return OBJECT_ENUM_TAG::TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT;
			if (strType == "TRIGGER_BOX_MILESTONE_DEFAULT") return OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT;

			// MONSTER
			if (strType == "MONSTER_DEFAULT") return OBJECT_ENUM_TAG::MONSTER_DEFAULT;
			if (strType == "MONSTER_DOG") return OBJECT_ENUM_TAG::MONSTER_DOG;
			if (strType == "MONSTER_FLY") return OBJECT_ENUM_TAG::MONSTER_FLY;
			if (strType == "MONSTER_BOOMER") return OBJECT_ENUM_TAG::MONSTER_BOOMER;

			// MONSTER_ELITE
			if (strType == "MONSTER_ELITE_DEFAULT") return OBJECT_ENUM_TAG::MONSTER_ELITE_DEFAULT;

			// MONSTER_BOSS
			if (strType == "MONSTER_BOSS_DEFAULT") return OBJECT_ENUM_TAG::MONSTER_BOSS_DEFAULT;
			if (strType == "MONSTER_BOSS_XIBI") return OBJECT_ENUM_TAG::MONSTER_BOSS_XIBI;

			// DEFAULT
			if (strType == "DEFAULT") return OBJECT_ENUM_TAG::DEFAULT;

			return OBJECT_ENUM_TAG::DEFAULT; // 매핑되지 않은 문자열이 들어올 경우의 방어 코드
		}
		static string	ToString(Enum eType)
		{
			switch (eType)
			{
			case OBJECT_ENUM_TAG::PLAYER: return "PLAYER";

			case OBJECT_ENUM_TAG::NPC_DEFAULT:								return "NPC_DEFAULT";
			case OBJECT_ENUM_TAG::NPC_PAN:									return "NPC_PAN";
			case OBJECT_ENUM_TAG::NPC_BERENICA:								return "NPC_BERENICA";

			case OBJECT_ENUM_TAG::OBJECT_DEFAULT:							return "OBJECT_DEFAULT";

			case OBJECT_ENUM_TAG::OBJECT_PICKING_DEFAULT:					return "OBJECT_PICKING_DEFAULT";
			case OBJECT_ENUM_TAG::OBJECT_PICKING_FLOWER_1:					return "OBJECT_PICKING_FLOWER_1";

			case OBJECT_ENUM_TAG::OBJECT_INTERACT_DEFAULT:					return "OBJECT_INTERACT_DEFAULT";
			case OBJECT_ENUM_TAG::OBJECT_INTERACT_WEAPONPICKUP:				return "OBJECT_INTERACT_WEAPONPICKUP";

			case OBJECT_ENUM_TAG::TRIGGER_BOX_DEFAULT:						return "TRIGGER_BOX_DEFAULT";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_TO_DEFAULT:					return "TRIGGER_BOX_TO_DEFAULT";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_VILAGE:			return "TRIGGER_BOX_TO_TUTORIAL_VILAGE";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_BOSS:				return "TRIGGER_BOX_TO_TUTORIAL_BOSS";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_GLOBAL_EVENT_DEFAULT:			return "TRIGGER_BOX_GLOBAL_EVENT_DEFAULT";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT:	return "TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT";
			case OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT:			return "TRIGGER_BOX_MILESTONE_DEFAULT";

			case OBJECT_ENUM_TAG::MONSTER_DEFAULT:							return "MONSTER_DEFAULT";
			case OBJECT_ENUM_TAG::MONSTER_DOG:								return "MONSTER_DOG";
			case OBJECT_ENUM_TAG::MONSTER_FLY:								return "MONSTER_FLY";
			case OBJECT_ENUM_TAG::MONSTER_BOOMER:							return "MONSTER_BOOMER";

			case OBJECT_ENUM_TAG::MONSTER_ELITE_DEFAULT:					return "MONSTER_ELITE_DEFAULT";

			case OBJECT_ENUM_TAG::MONSTER_BOSS_DEFAULT:						return "MONSTER_BOSS_DEFAULT";
			case OBJECT_ENUM_TAG::MONSTER_BOSS_XIBI:						return "MONSTER_BOSS_XIBI";

			case OBJECT_ENUM_TAG::DEFAULT:									return "DEFAULT";

			default: return "Unknown";
			}
		}
	}OBJECT_ENUM_TAG;

	// 툴에서 콤보박스 순회용으로 쓸 배열입니다. (순서대로 띄워집니다)
	inline static const OBJECT_ENUM_TAG::Enum g_arrAllObjectTags[] = {
		OBJECT_ENUM_TAG::PLAYER,
		OBJECT_ENUM_TAG::NPC_DEFAULT,
		OBJECT_ENUM_TAG::NPC_PAN,
		OBJECT_ENUM_TAG::NPC_BERENICA,
		OBJECT_ENUM_TAG::OBJECT_DEFAULT,
		OBJECT_ENUM_TAG::OBJECT_PICKING_DEFAULT,
		OBJECT_ENUM_TAG::OBJECT_PICKING_FLOWER_1,
		OBJECT_ENUM_TAG::OBJECT_INTERACT_DEFAULT,
		OBJECT_ENUM_TAG::OBJECT_INTERACT_WEAPONPICKUP,
		OBJECT_ENUM_TAG::OBJECT_INTERACT_END,
		OBJECT_ENUM_TAG::TRIGGER_BOX_DEFAULT,
		OBJECT_ENUM_TAG::TRIGGER_BOX_TO_DEFAULT,
		OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_VILAGE,
		OBJECT_ENUM_TAG::TRIGGER_BOX_TO_TUTORIAL_BOSS,
		OBJECT_ENUM_TAG::TRIGGER_BOX_GLOBAL_EVENT_DEFAULT,
		OBJECT_ENUM_TAG::TRIGGER_BOX_TUTORIAL_UI_EVENT_DEFAULT,
		OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT,
		OBJECT_ENUM_TAG::MONSTER_DEFAULT,
		OBJECT_ENUM_TAG::MONSTER_DOG,
		OBJECT_ENUM_TAG::MONSTER_FLY,
		OBJECT_ENUM_TAG::MONSTER_BOOMER,
		OBJECT_ENUM_TAG::MONSTER_ELITE_DEFAULT,
		OBJECT_ENUM_TAG::MONSTER_BOSS_DEFAULT,
		OBJECT_ENUM_TAG::MONSTER_BOSS_XIBI,
		OBJECT_ENUM_TAG::DEFAULT
	};


#pragma region Lerp Type

	enum class ELerpType
	{
		NONE,           /* 보간 안 함 (Cut 연출. 즉시 텔레포트) */
		Linear,         /* 등속 이동 (기계처럼 처음부터 끝까지 똑같은 속도) */
		SlowStart,      /* 서서히 출발 (점점 빨라짐. 상용 엔진의 EaseIn) */
		SlowEnd,        /* 서서히 도착 (목적지에서 스르륵 멈춤. 상용 엔진의 EaseOut) */
		SmoothStep,     /* 서서히 출발 + 서서히 도착 (컷신의 꽃. 상용 엔진의 EaseInOut) */
		Curve,			/* 국선 주행 */
		END
	};

#pragma endregion


#pragma region Cinematic Camera Sequence BroadCast_Type

	enum class CCS_BROADCAST_TYPE
	{
		BEGIN_CCS,
		DEPART,
		ON_REACH,
		END_CCS,
		END,
	};

#pragma endregion
}
#endif // Engine_Enum_h__
