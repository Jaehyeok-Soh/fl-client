#include "pch.h"
#include "Tool.h"
#include "framework.h"
#include "MainApplication.h"
#include "GameInstance.h"

#define MAX_LOADSTRING 100

HWND                g_hWnd;
HINSTANCE           g_hInstance;
WCHAR               szTitle[MAX_LOADSTRING];
WCHAR               szWindowClass[MAX_LOADSTRING];
CMainApplication*   pApplication = { nullptr };

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
#ifdef  _DEBUG
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TOOL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    pApplication = CMainApplication::Create();
    if (pApplication == nullptr)
        return FALSE;

    HACCEL hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TOOL));

    MSG msg;
    msg.message = WM_NULL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(pGameInstance->Add_Timer(L"Timer_Default")))
        return FALSE;

    if (FAILED(pGameInstance->Add_Timer(L"Timer_60")))
        return FALSE;

    _float fTimeAcc = { 0.0f };

    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }

        pGameInstance->Compute_TimeDelta(TEXT("Timer_Default"));
        fTimeAcc += pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

        if (fTimeAcc >= 1.f / 60.0f)
        {
            pGameInstance->Compute_TimeDelta(TEXT("Timer_60"));

            pApplication->Update(pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
            pApplication->Render();
            fTimeAcc -= 1.f / 60.0f;
        }
    }

    Safe_Release(pGameInstance);

    if (Safe_Release(pApplication) != 0)
    {
        MSG_BOX("MainApplication Release Failed");
        return FALSE;
    }

    return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TOOL));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance;

    // 최정우용 
    MONITORINFO tMonitorInfo = { sizeof(MONITORINFO) };
    ::GetMonitorInfo(MonitorFromPoint({ 0, 0 }, MONITOR_DEFAULTTONEAREST), &tMonitorInfo);
    // RECT rcWork = tMonitorInfo.rcWork;

    RECT rcWork = { 100, 100, 1700, 1000 };
    RECT rc = { 0, 0, rcWork.right - rcWork.left,  rcWork.bottom - rcWork.top };

    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = ::CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        rcWork.left + rc.left,
        rcWork.top + rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;

    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    RECT client = {};
    GetClientRect(hWnd, &client);

    g_iWinSizeX = client.right - client.left;
    g_iWinSizeY = client.bottom - client.top;
    g_fAspectio = (_float)g_iWinSizeX / g_iWinSizeY;

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            ::DestroyWindow(hWnd);
            break;
        default:
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        if (pApplication == nullptr)
            break;

        RECT rect = { 0, 0, 100, 100 };
        ::GetWindowRect(hWnd, &rect);

        _int x = rect.left;
        _int y = rect.top;
        _int iWidth = rect.right - rect.left;
        _int iHeight = rect.bottom - rect.top;

        g_iWinSizeX = iWidth;
        g_iWinSizeY = iHeight;
        g_fAspectio = (_float)g_iWinSizeX / g_iWinSizeY;

        RECT winRect;
        ::GetClientRect(hWnd, &winRect);

        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<float>(winRect.right - winRect.left);
        viewport.Height = static_cast<float>(winRect.bottom - winRect.top);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        CGameInstance::GetInstance()->Resize_Viewport(viewport);
    } break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hWnd, &ps);
        ::EndPaint(hWnd, &ps);
    }
    break;
    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE:
        {
            ::DestroyWindow(hWnd);
        } break;
        }
    } break;
    case WM_DPICHANGED:
    {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top
                , suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top
                , SWP_NOZORDER | SWP_NOACTIVATE);
        }
    } break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}