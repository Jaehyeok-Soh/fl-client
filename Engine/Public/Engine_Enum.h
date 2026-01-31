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
	enum class RENDER_CATEGORY : unsigned int { PRIORITY, BLEND, NONEBLEND, NONELIGHT, BLENDUI, UI, END };
	enum class DEFFERRED { DEBUG, DIRECTIONAL, POINT, COMBINED, END };
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
	// Component
	//===================
	enum class EComponentType : unsigned int
	{
		TRANSFORM = 0,
		MODEL,
		NAVIGATION,
		TEXTURE,
		COLLIDER,
		VIBUFFER,
		CONTROLCONTEXT,
		ACTIONSTATE,
		CAMERA,
		SHADER,
		//
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
	// CollideMesh
	//===================
	enum class ESurfaceType : unsigned int
	{
		NONE = 0,
		GROUND,
		WALL,
		CEILING,
	};

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
}
#endif // Engine_Enum_h__
