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
            if (msg.message == WM_QUIT)
            {
                g_Running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            Input::EndFrame();
        }
    }

    window.Destroy();

    return EXIT_SUCCESS;
}
