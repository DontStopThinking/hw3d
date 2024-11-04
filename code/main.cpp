#include "cleanwindows.h"

bool g_Running = false;

static LRESULT CALLBACK WindowCallback(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    {
        g_Running = false;
    } break;

    case WM_DESTROY:
    {
        g_Running = false;
    } break;
    }

    return DefWindowProc(windowHandle, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    const LPCWSTR className = L"hw3d";

    // Register window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_OWNDC,
        .lpfnWndProc = WindowCallback,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = className
    };

    if (!RegisterClassEx(&wc))
    {
        // TODO: Logging
        return -1;
    }

    const LPCWSTR windowTitle = L"HW3D Window";

    constexpr int windowWidth = 700;
    constexpr int windowHeight = 400;

    const DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    RECT wr
    {
        .left = 100,
        .top = 100,
        .right = wr.left + windowWidth,
        .bottom = wr.top + windowHeight
    };

    if (!AdjustWindowRect(&wr, windowStyle, FALSE))
    {
        // TODO: Logging
        return -1;
    }

    // Create window instance
    HWND windowHandle = CreateWindow(
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

    if (!windowHandle)
    {
        // TODO: Logging
        return -1;
    }

    g_Running = true;

    ShowWindow(windowHandle, SW_SHOW);

    while (g_Running)
    {
        MSG msg = {};

        // PeekMessage() is non-blocking whereas GetMessage() will block.
        // We use PeekMessage() instead of GetMessage() because we want to keep running if there are no
        // messages.
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                g_Running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if (!DestroyWindow(windowHandle))
    {
        // TODO: Logging
    }

    if (!UnregisterClass(className, GetModuleHandle(nullptr)))
    {
        // TODO: Logging
    }

    return 0;
}
