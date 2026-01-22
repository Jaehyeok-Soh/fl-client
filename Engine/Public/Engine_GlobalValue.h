#pragma once
#include "Engine_Define.h"


NS_BEGIN(Engine)

inline constexpr wchar_t g_MainActorCameraName[]{ L"GameObject_Camera_MainTargeter\0" };
inline constexpr wchar_t g_FreeCameraName[]{ L"GameObject_Camera_Free\0" };
inline constexpr wchar_t g_wszAssetRelativePath[]{ L"../../Resources/Assets/\0" };
inline constexpr wchar_t g_wszResourceRelativePath[]{ L"../../Resources/\0" };
inline constexpr wchar_t g_wszModelRelativePath[]{ L"../../Resources/Models/\0" };
inline constexpr wchar_t g_wszModelExtension[]{ L".fbx\0" };
inline constexpr wchar_t g_wszBoneExtension[]{ L".skel\0" };
inline constexpr wchar_t g_wszMaterialExtension[]{ L".json\0" };
inline constexpr wchar_t g_wszMeshExtension[]{ L".mesh\0" };
inline constexpr wchar_t g_wszAnimationExtension[]{ L".clip\0" };

NS_END