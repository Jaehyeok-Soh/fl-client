#pragma once
#include "pch.h"
#include "Engine_Define.h"
#include <windows.h>

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

struct LoadAnimModel { using Signature = void(path animModelPath); };
