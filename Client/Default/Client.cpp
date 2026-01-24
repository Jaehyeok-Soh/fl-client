#include "pch.h"
#include "Client.h"
#include "framework.h"

#include "MainApplication.h"
#include "GameInstance.h"

#define MAX_LOADSTRING 100

_bool               g_bStart{ false };
HWND                g_hWnd;
HINSTANCE           g_hInstance;
WCHAR               szTitle[MAX_LOADSTRING];
WCHAR               szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
#ifdef _DEBUG
    ::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ::LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    ::LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    ::MyRegisterClass(hInstance);

    if (!::InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    CMainApplication* pMainApplication = CMainApplication::Create();
    if (!pMainApplication)
        return FALSE;

    HACCEL hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
    msg.message = WM_NULL;

    ::ShowCursor(FALSE);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (FAILED(pGameInstance->Add_Timer(L"Timer_Default")))
        return FALSE;

    if (FAILED(pGameInstance->Add_Timer(L"Timer_60")))
        return FALSE;

    _float fTimeAcc = { 0.0f };
    g_bStart = true;
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

            pMainApplication->Update(pGameInstance->Get_TimeDelta(TEXT("Timer_60")));
            pMainApplication->Render();

            fTimeAcc -= 1.f / 60.0f;
        }
    }

    Safe_Release(pGameInstance);

    if (Safe_Release(pMainApplication) != 0)
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
    wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return ::RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance;

    RECT rc = { 0, 0, g_iWinSizeX, g_iWinSizeY };

    ::AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hWnd = ::CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        100, 100,
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

    ::SetWindowPos(hWnd, HWND_TOP, 100, 100, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_SHOWWINDOW);

    g_hWnd = hWnd;

    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
    case WM_ACTIVATEAPP:
    {
        if (g_bStart)
        {
            _bool bActive = wParam != 0;
            if (bActive == false)
            {
                CGameInstance::GetInstance()->Set_Capture(false);
            }
            else
            {
                ::ShowCursor(FALSE);
                CGameInstance::GetInstance()->Set_Capture(true);
            }
        }
    } break;
    case WM_KILLFOCUS:
    {
        if (g_bStart)
        {
            CGameInstance::GetInstance()->Set_Capture(false);
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
