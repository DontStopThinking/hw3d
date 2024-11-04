#include <cstdint>

#include "cleanwindows.h"

bool g_Running = false;

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;

static LRESULT CALLBACK WindowCallback(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

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

    case WM_ACTIVATEAPP:
    {
        OutputDebugStringA("WM_ACTIVATEAPP");
    } break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        uint32 vkCode = wParam;

        bool wasDown = ((lParam & (1ll << 30)) != 0); //! Was the same key down before?
        bool isDown = ((lParam & (1ll << 31)) == 0);  //! Is the same key still down?

        //! Early exit when the key is held down
        if (wasDown == isDown)
        {
            break;
        }

        if (vkCode == 'W')
        {
        }
        else if (vkCode == VK_SPACE)
        {
            OutputDebugStringA("SPACE: ");
            if (isDown)
            {
                OutputDebugStringA("isDown ");
            }
            if (wasDown)
            {
                OutputDebugStringA("wasDown");
            }
            OutputDebugStringA("\n");
        }
        else if (vkCode == VK_ESCAPE)
        {
            g_Running = false;
        }
    } break;

    case WM_PAINT:
    {
        PAINTSTRUCT paint = {};
        HDC deviceContext = BeginPaint(windowHandle, &paint);
        EndPaint(windowHandle, &paint);
    } break;

    default:
    {
        result = DefWindowProc(windowHandle, msg, wParam, lParam);
    }
    }

    return result;
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
        .lpszClassName = className,
        // .hIcon
    };

    if (!RegisterClassEx(&wc))
    {
        // TODO: Logging
        return -1;
    }

    const LPCWSTR windowTitle = L"HW3D Window";

    constexpr int windowWidth = 700;
    constexpr int windowHeight = 400;

    const DWORD windowStyle = WS_CAPTION | WS_MINIMIZEBOX | CS_HREDRAW | CS_VREDRAW | CS_OWNDC |
                              WS_OVERLAPPEDWINDOW | WS_VISIBLE;

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

    //! Create window instance
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

        //! PeekMessage() is non-blocking whereas GetMessage() will block.
        //! We use PeekMessage() instead of GetMessage() because we want to keep running if there are no
        //! messages.
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
