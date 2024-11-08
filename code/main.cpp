#include <cstdlib>

#include "cleanwindows.h"

#include "window.h"
#include "input.h"

constinit bool g_Running = false;

int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    Window window(1280, 720, L"HW3D Engine");

    if (!window.Init())
    {
        // TODO: Logging
        return EXIT_FAILURE;
    }

    g_Running = true;

    while (g_Running)
    {
        MSG msg = {};

        //! PeekMessage() is non-blocking whereas GetMessage() will block.
        //! We use PeekMessage() instead of GetMessage() because we want to keep running if there are no
        //! messages.
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            g_Running = false;
            break;
        }

        if (Input::ButtonPressed(VK_ESCAPE))
        {
            OutputDebugString(L"Escape pressed\n");
            PostQuitMessage(0);
        }
        else if (Input::ButtonPressed('W'))
        {
            OutputDebugString(L"W pressed\n");
        }
        else if (Input::ButtonReleased('W'))
        {
            OutputDebugString(L"W released\n");
        }
        else if (Input::ButtonCheck(VK_SPACE))
        {
            OutputDebugString(L"Space pressed\n");
        }
        else if (Input::ButtonPressed(VK_LEFT))
        {
            OutputDebugString(L"Left pressed\n");
        }
        else if (Input::ButtonPressed(VK_RIGHT))
        {
            OutputDebugString(L"Right pressed\n");
        }
        else if (Input::ButtonPressed(VK_UP))
        {
            OutputDebugString(L"Up pressed\n");
        }
        else if (Input::ButtonPressed(VK_DOWN))
        {
            OutputDebugString(L"Down pressed\n");
        }
        else if (Input::ButtonCheck('V') && Input::ButtonCheck('C'))
        {
            OutputDebugStringA("C + V pressed\n");
        }

        Input::EndFrame();
    }

    window.Destroy();

    return EXIT_SUCCESS;
}
