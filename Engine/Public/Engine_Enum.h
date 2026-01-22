#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	//===================
	// Input
	//===================
	enum class MOUSEKEYSTATE : unsigned int { LB, RB, WHEEL, XB, END };
	enum class MOUSEMOVESTATE : unsigned int { X, Y, WHEEL, END };
	enum class MATERIALSLOT { DIFFUSE = 0, NORMAL, SPECULAR, EMISSIVE, END };
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
}
#endif // Engine_Enum_h__
