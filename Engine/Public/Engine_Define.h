#ifndef Engine_Define_h__
#define Engine_Define_h__

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/SimpleMath.h>

#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <FX11/d3dx11effect.h>

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include "FMOD/fmod.hpp"

//#pragma push_macro("new")
//#undef new
//#include <PxPhysicsAPI.h>
//#include <PxPhysics.h>
//#pragma pop_macro("new")


#include <string>
#include <span>
#include <vector>
#include <array>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <filesystem>
#include <typeindex>
#include <type_traits>
#include <cstring>
#include <utility>
#include <functional>
#include <ctime>
#include <variant>

using std::span;
using std::string;
using std::variant;
using std::wstring;
using std::vector;
using std::array;
using std::list;
using std::map;
using std::unordered_map;
using std::unordered_set;
using std::filesystem::path;

using namespace DirectX;
using namespace SimpleMath;

#define MAX_BONE_TRANSFORMS 512
#define MAX_MODEL_KEYFRAMES 512
#define MAX_MODEL_INSTANCE 512
#define MAX_NAME 128

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_GlobalValue.h"

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

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
