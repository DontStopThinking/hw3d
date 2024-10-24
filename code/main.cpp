#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    const LPCWSTR className = L"hw3d";

    // Register window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_OWNDC,
        .lpfnWndProc = DefWindowProc,
        .hInstance = hInstance,
        .lpszClassName = className
    };

    RegisterClassEx(&wc);

    const LPCWSTR windowTitle = L"HW3D Window";

    // Create window instance
    HWND hwnd = CreateWindowEx(
        0,
        className,
        windowTitle, // window title
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, // style
        200, // x
        200, // y
        640, // width
        480, // height
        nullptr, // parent
        nullptr, // menu
        hInstance, // instance
        nullptr // lpParam
    );

    if (!hwnd)
    {
        return -1;
    }

    ShowWindow(hwnd, SW_SHOW);

    while (true);

    return 0;
}
