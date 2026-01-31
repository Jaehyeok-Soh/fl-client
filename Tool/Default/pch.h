#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <DirectXTex/DirectXTex.h>

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

#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

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
#include <thread>
#include <mutex>
#include <condition_variable>
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
using std::thread;
using std::mutex;
using std::condition_variable;
using std::filesystem::path;

using namespace DirectX;
using namespace SimpleMath;

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

#include "Tool_Defines.h"
#include "DataDefine_Json.h"