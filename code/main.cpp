#include <string>
#include <format>

#include "cleanwindows.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
    } break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    const LPCWSTR className = L"hw3d";

    // Register window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_OWNDC,
        .lpfnWndProc = WndProc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = className
    };

    RegisterClassEx(&wc);

    const LPCWSTR windowTitle = L"HW3D Window";

    constexpr int windowWidth = 800;
    constexpr int windowHeight = 300;

    const DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    RECT wr
    {
        .left = 100,
        .top = 100,
        .right = wr.left + windowWidth,
        .bottom = wr.top + windowHeight
    };

    AdjustWindowRect(&wr, windowStyle, FALSE);

    // Create window instance
    HWND hWnd = CreateWindow(
        className,
        windowTitle, // window title
        windowStyle, // style
        CW_USEDEFAULT, // x
        CW_USEDEFAULT, // y
        wr.right - wr.left, // width
        wr.bottom - wr.top, // height
        nullptr, // parent
        nullptr, // menu
        GetModuleHandle(nullptr), // instance
        nullptr // lpParam
    );

    if (!hWnd)
    {
        return -1;
    }

    ShowWindow(hWnd, SW_SHOW);

    // Message pump
    MSG msg;
    BOOL gResult;

    // If GetMessage returns negative value, then that means there was an error
    while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(hWnd);
    UnregisterClass(className, GetModuleHandle(nullptr));

    if (gResult == -1)
    {
        return -1;
    }
    else
    {
        return (int)msg.wParam;  // This is the value returned from WndProc
    }
}
