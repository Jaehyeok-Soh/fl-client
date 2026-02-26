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
	enum class LIGHT_TYPE : unsigned int { DIRECTIONAL, STATICPOINT, DYNAMICPOINT, END };
	enum class EPOINT { A, B, C, END };
	enum class ELINE { AB, BC, CA, END };
	enum class RENDER_CATEGORY : unsigned int { PRIORITY, BLEND,NONEBLEND, NONELIGHT, ENVIRONMENT, DISTOTION, BLENDUI, UI, END };
	enum class DEFFERRED { DEBUG, DIRECTIONAL, POINT, OUTLINE, SSAO_GEN, SSAO_BLURH, SSAO_BLURV, SSAO_UPSAMPLE, COMBINED, BLOOM_EXTRACT, BLOOM_BLURH, BLOOM_BLURV, TONEMAP, END };
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
		"CB_MAPPING_RGB"
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
		RT_AO,
		RT_SceneHDR,
		RT_SceneHDR_Copy,
		RT_Bloom,
		LUT_Stand,
		Transform,
		Materials,
		Textures,
		Cube,
		SSAONoise,
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
		"g_RenderTargetAOTexture",
		"g_RenderTargetSceneHDRTexture",
		"g_RenderTargetSceneHDRCopyTexture",
		"g_RenderTargetBloomTexture",
		"g_LUT_Stand",
		"g_TransformMap",
		"g_MaterialTextures",
		"g_DefaultTextures",
		"g_TextureCube",
		"g_SSAONoiseTexture"
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
		//
		PX_RIGIDBODY,
		PX_COLLIDER,
		PX_CCT,
		PX_ATTACKOVERLAP,
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

	typedef struct tagCollisionEvent
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

	typedef struct tagPhysicsFilterGroup
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

			NONE = 1 << 17,
			END
		};

		static bool IsPlayer(unsigned int iFlag) { return (iFlag & PLAYER) != 0; }
		static bool IsMonster(unsigned int iFlag) { return (iFlag & MONSTER) != 0; }
		static bool IsMap(unsigned int iFlag) { return (iFlag & MAP) != 0; }

		static bool IsTrigger(unsigned int iFlag)
		{
			static constexpr unsigned int iTriggerLayer
			{
				TRIGGER_UI | TRIGGER_QUEST | TRIGGER_SPAWN | TRIGGER_DIRECTION
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

			const unsigned int iVictim = bAttack_A == true ? iFlagA : iFlagB;

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
		END
	};
	//===================
	// Font Shader Type
	//===================
	enum class EFontShaderType
	{
		NORMAL,OUTLINE, NOISE, NOISE_KOR, OUTLINE_NOISE, OUTLINE_NOISE_KOR, END
	};

	//===================
	// Skill Type
	//===================
	enum class SKILL_TYPE { DAMAGE, BUFF, SUMMON, CURE, DEFENSE, END }; // skill의 타입

}
#endif // Engine_Enum_h__
