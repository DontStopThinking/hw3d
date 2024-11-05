#include <stdint.h>
#include <stdlib.h>

#include "cleanwindows.h"

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;

constinit bool g_Running = false;

constinit BITMAPINFO g_BitmapInfo = {}; //! The Bitmap that we will draw onto the screen
constinit void* g_BitmapMemory = nullptr;   //! The location where Windows will store the bitmap memory
int g_BitmapWidth = 0;
int g_BitmapHeight = 0;

//! DIB = "Device Independent Bitmap". We create a bitmap.
static void ResizeDIBSection(int width, int height)
{
    if (g_BitmapMemory)
    {
        //! If memory was allocated before then free it first before resizing the bitmap.
        VirtualFree(g_BitmapMemory, 0, MEM_RELEASE);
    }

    g_BitmapWidth = width;
    g_BitmapHeight = height;

    BITMAPINFOHEADER bitmapInfoHeader =
    {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = g_BitmapWidth,   // width of the bitmap in pixels
        .biHeight = -g_BitmapHeight, // height of the bitmap in pixels
        .biPlanes = 1,  // always 1
        .biBitCount = 32,   // no. of bits per pixel. 32 bits = RGBA
        .biCompression = BI_RGB, // we don't want any compression. BI_RGB means no compression.
    };

    g_BitmapInfo = { .bmiHeader = bitmapInfoHeader };

    int bytesPerPixel = 4;
    int bitmapMemorySize = bytesPerPixel * width * height;
    g_BitmapMemory = VirtualAlloc(nullptr, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    int pitch = width * bytesPerPixel;
    uint8* row = (uint8*)g_BitmapMemory;

    for (int y = 0; y < g_BitmapHeight; y++)
    {
        uint8* pixel = (uint8*)row;

        for (int x = 0; x < g_BitmapWidth; x++)
        {
            *pixel = (uint8)x;
            ++pixel;

            *pixel = (uint8)y;
            ++pixel;

            *pixel = 0;
            ++pixel;

            *pixel = 0;
            ++pixel;
        }

        row += pitch;
    }
}

//! Display the bitmap using GDI.
static void UpdateMyWindow(HDC deviceContext, RECT* windowRect, int x, int y, int width, int height)
{
    int windowWidth = windowRect->right - windowRect->left;
    int windowHeight = windowRect->bottom - windowRect->top;

    StretchDIBits(
        deviceContext,
        /*x, y, width, height,
        x, y, width, height,*/
        0, 0, g_BitmapWidth, g_BitmapHeight,
        0, 0, windowWidth, windowHeight,
        g_BitmapMemory, &g_BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

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
        OutputDebugStringA("WM_ACTIVATEAPP\n");
    } break;

    case WM_SIZE:   //! Window was resized
    {
        OutputDebugStringA("WM_SIZE\n");

        RECT clientRect = {};
        GetClientRect(windowHandle, &clientRect);

        const int width = clientRect.right - clientRect.left;
        const int height = clientRect.bottom - clientRect.top;

        ResizeDIBSection(width, height);
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

    case WM_PAINT:  //! The application needs to be re-painted
    {
        PAINTSTRUCT paint = {};
        HDC deviceContext = BeginPaint(windowHandle, &paint);

        int x = paint.rcPaint.left;
        int y = paint.rcPaint.top;

        int width = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;

        RECT clientRect = {};
        GetClientRect(windowHandle, &clientRect);

        UpdateMyWindow(deviceContext, &clientRect, x, y, width, height);

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
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
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

    return EXIT_SUCCESS;
}
