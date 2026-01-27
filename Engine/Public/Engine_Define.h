#ifndef Engine_Define_h__
#define Engine_Define_h__

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
#include "VertexData.h"

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
