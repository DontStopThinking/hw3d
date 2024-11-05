#include <cstdint>
#include <cstdlib>

#include "cleanwindows.h"

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;

struct OffscreenBuffer
{
    BITMAPINFO info; //! The Bitmap that we will draw onto the screen
    void* memory;   //! The location where Windows will store the bitmap memory
    int pitch;
    int bytesPerPixel;  //! How many bytes is each pixel?
    int width;    //! width of the bitmap in pixels
    int height;   //! height of the bitmap in pixels
};

struct WindowDimensions
{
    int width;
    int height;
};

constinit bool g_Running = false;
constinit OffscreenBuffer g_BackBuffer = {};

static WindowDimensions GetWindowDimensions(HWND window)
{
    WindowDimensions result = {};

    RECT clientRect = {};
    GetClientRect(window, &clientRect);

    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;

    return result;
}

static void RenderGradient(OffscreenBuffer* buffer, int xOffset, int yOffset)
{
    uint8* row = (uint8*)buffer->memory;

    for (int y = 0; y < buffer->height; y++)
    {
        uint32* pixel = (uint32*)row;

        for (int x = 0; x < buffer->width; x++)
        {
            uint8 blue = x + xOffset;
            uint8 green = y + yOffset;

            *pixel++ = ((green << 8) | blue);
        }

        row += buffer->pitch;
    }
}

//! DIB = "Device Independent Bitmap". We create a bitmap.
static void ResizeDIBSection(OffscreenBuffer* buffer, int width, int height)
{
    if (buffer->memory)
    {
        //! If memory was allocated before then free it first before resizing the bitmap.
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;
    buffer->bytesPerPixel = 4;

    BITMAPINFOHEADER bitmapInfoHeader =
    {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = buffer->width,   // width of the bitmap in pixels
        .biHeight = -buffer->height, // height of the bitmap in pixels
        .biPlanes = 1,  // always 1
        .biBitCount = 32,   // no. of bits per pixel. 32 bits = RGBA
        .biCompression = BI_RGB, // we don't want any compression. BI_RGB means no compression.
    };

    buffer->info = { .bmiHeader = bitmapInfoHeader };

    int bitmapMemorySize = buffer->bytesPerPixel * width * height;
    buffer->memory = VirtualAlloc(nullptr, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    buffer->pitch = width * buffer->bytesPerPixel;
}

//! Display the bitmap using GDI.
static void DisplayBufferInWindow(
    OffscreenBuffer* buffer, HDC deviceContext, int windowWidth, int windowHeight, int x, int y, int width, int height)
{
    StretchDIBits(
        deviceContext,
        /*x, y, width, height,
        x, y, width, height,*/
        0, 0, buffer->width, buffer->height,
        0, 0, windowWidth, windowHeight,
        buffer->memory, &buffer->info, DIB_RGB_COLORS, SRCCOPY);
}

static LRESULT CALLBACK WindowCallback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
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

        WindowDimensions dimensions = GetWindowDimensions(window);

        ResizeDIBSection(&g_BackBuffer, dimensions.width, dimensions.height);
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
        HDC deviceContext = BeginPaint(window, &paint);

        int x = paint.rcPaint.left;
        int y = paint.rcPaint.top;

        int width = paint.rcPaint.right - paint.rcPaint.left;
        int height = paint.rcPaint.bottom - paint.rcPaint.top;

        WindowDimensions dimensions = GetWindowDimensions(window);

        DisplayBufferInWindow(&g_BackBuffer, deviceContext, dimensions.width, dimensions.height, x, y, width, height);

        EndPaint(window, &paint);
    } break;

    default:
    {
        result = DefWindowProc(window, msg, wParam, lParam);
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
    HWND window = CreateWindow(
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

    if (!window)
    {
        // TODO: Logging
        return EXIT_FAILURE;
    }

    g_Running = true;

    ShowWindow(window, SW_SHOW);

    int xOffset = 0;
    int yOffset = 0;

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

        RenderGradient(&g_BackBuffer, xOffset, yOffset);

        HDC deviceContext = GetDC(window);

        WindowDimensions dimensions = GetWindowDimensions(window);

        DisplayBufferInWindow(
            &g_BackBuffer,
            deviceContext,
            dimensions.width,
            dimensions.height,
            0, 0,
            windowWidth, windowHeight);

        ReleaseDC(window, deviceContext);

        ++xOffset;
    }

    if (!DestroyWindow(window))
    {
        // TODO: Logging
    }

    if (!UnregisterClass(className, GetModuleHandle(nullptr)))
    {
        // TODO: Logging
    }

    return EXIT_SUCCESS;
}
