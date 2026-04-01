#pragma once

NS_BEGIN(Engine)

inline constexpr wchar_t g_MainActorCameraName[]{ L"GameObject_Camera_MainTargeter" };
inline constexpr wchar_t g_FreeCameraName[]{ L"GameObject_Camera_Free" };
inline constexpr wchar_t g_wszAssetRelativePath[]{ L"../../Resources/Assets/" };
inline constexpr wchar_t g_wszResourceRelativePath[]{ L"../../Resources/" };
inline constexpr wchar_t g_wszModelRelativePath[]{ L"../../Resources/Models/" };
inline constexpr wchar_t g_wszModelExtension[]{ L".fbx" };
inline constexpr wchar_t g_wszBoneExtension[]{ L".skel" };
inline constexpr wchar_t g_wszMaterialExtension[]{ L".json" };
inline constexpr wchar_t g_wszMeshExtension[]{ L".mesh" };
inline constexpr wchar_t g_wszAnimationExtension[]{ L".clip" };

inline constexpr wchar_t g_wszAttackPresetDataPath[]{ L"../../Resources/Data/AttackPresetDatas/" };

#pragma region Texture Splating


inline constexpr _tchar g_wszMapTexture_Path[]{ L"../../Resources/Textures/Map/LandScape" };
inline constexpr _char	g_szTexLandScape_Mix_RGB[]{ "g_TexLandScape_Mix_RGB" };
inline constexpr _char	g_szTexLandScape_Mix_RGBA[]{ "g_TexLandScape_Mix_RGBA" };

inline constexpr _char  g_szBase_Texture[]{ "g_Base_Texture" };
inline constexpr _char  g_szMix_RGBA_Texture[]{ "g_Mix_RGBA_Texture" };


inline constexpr _char	g_szMix_DH_Tile_Texture[]{ "g_Mix_DH_Tile_Texture" };
inline constexpr _char	g_szMix_NBR_Tile_Texture[]{ "g_Mix_NBR_Tile_Texture" };


inline constexpr _char	g_szCB_MIX_RGBA_INFO[]{ "CB_MIX_RGBA_INFO" };
inline constexpr _char	g_szUse_Mix_RGBA_Map_Count[]{ "g_iUse_Mix_RGBA_Count" };


#pragma endregion

inline constexpr _uint OF_Outline = 1u << 0;
inline constexpr _uint OF_Rim = 1u << 1;
inline constexpr _uint OF_NoSSAO = 1u << 2;
inline constexpr _uint OF_SpecialSkill = 1u << 3;

const unsigned int		 MIN = 0;
const unsigned int		 MAX = 1;

const int				 NONE_BONE_INDEX = -1;
const int                NONE_INDEX = - 1;

static bool				 g_ToolDebugRender = { true };

const std::string PhysicsJointNames[RAGDOLLJOINT::END] = {
    "pelvis",
    "spine_02",
    "head",
    "upperarm_l",
    "lowerarm_l",
    "upperarm_r",
    "lowerarm_r",
    "thigh_l",
    "calf_l",
    "foot_l",
    "thigh_r",
    "calf_r",
    "foot_r",
};

NS_END