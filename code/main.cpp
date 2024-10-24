#include <Windows.h>

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
    } break;

    case WM_KEYDOWN:
    {
        if (wParam == 'C')
        {
            SetWindowText(hWnd, L"New Window Title");
        }
    } break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    const LPCWSTR className = L"hw3d";

    // Register window class
    WNDCLASSEX wc =
    {
        .cbSize = sizeof(wc),
        .style = CS_OWNDC,
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .lpszClassName = className
    };

    RegisterClassEx(&wc);

    const LPCWSTR windowTitle = L"HW3D Window";

    // Create window instance
    HWND hWnd = CreateWindowEx(
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

    if (gResult == -1)
    {
        return -1;
    }
    else
    {
        return (int)msg.wParam;  // This is the value returned from WndProc
    }
}
