#ifndef Engine_Define_h__
#define Engine_Define_h__

#define MAX_BONE_TRANSFORMS 512
#define MAX_MODEL_KEYFRAMES 512
#define MAX_MODEL_INSTANCE 512
#define MAX_NAME 128
#define HITSTOP_TIME 0.1f
#define CAST_VOID_P(p) reinterpret_cast<void*>(p)
#define CAST_VOID_PP(p) reinterpret_cast<void**>(p)
#define SHADOW_CASCADE_COUNT 2
#define SHADOW_MAP_SIZE 2048
#define SHADOW_BAKE_SIZE 2048
#define BAKED_SECTION_COUNT_X 5
#define BAKED_SECTION_COUNT_Z 5
#define ACTIVE_SECTION_MAX 9
#define TO_HASH(str) Engine_Utils::ToHash(str)


// PhysX
#define PVD_HOST "127.0.0.1"

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_GlobalValue.h"
#include "VertexData.h"


constexpr const wchar_t* g_wszCinematicCameraTag = L"CinematicCamera";
constexpr const wchar_t* g_wszCinematicCamera_PrototypeTag = L"Prototype_GameObject_CinematicCamera";


/* Event µî·Ï */
struct CCS_EVENT { using Signature = void(const Engine::CCS_BROADCAST_DESC& tDesc); };


#pragma warning(disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

using namespace Engine;
#endif // Engine_Define_h__
