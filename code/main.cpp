#include <cstdlib>
#include <cstdio>

#include "cleanwindows.h"

#include "window.h"
#include "input.h"

constinit bool g_Running = false;

int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    Window window(1280, 720, L"HW3D Engine");

    if (!window.Init())
    {
        // TODO(sbalse): Logging
        return EXIT_FAILURE;
    }

    g_Running = true;

    wchar_t windowTitle[32] = {};

    while (g_Running)
    {
        MSG msg = {};

        //! NOTE(sbalse): PeekMessage() is non-blocking whereas GetMessage() will block.
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

        // NOTE(sbalse): Set window title to mouse x and y.
        i32 mouseX = MouseX();
        i32 mouseY = MouseY();
        _snwprintf(windowTitle, 32, L"Mouse: (%d, %d)\n", mouseX, mouseY);
        window.SetTitle(windowTitle);

        if (KeyboardButtonPressed(VK_ESCAPE))
        {
            OutputDebugString(L"Escape pressed\n");
            PostQuitMessage(0); // Quit game on escape pressed
        }
        else if (KeyboardButtonPressed('W'))
        {
            OutputDebugString(L"W pressed\n");
        }
        else if (KeyboardButtonReleased('W'))
        {
            OutputDebugString(L"W released\n");
        }
        else if (KeyboardButtonPressed(VK_SPACE))
        {
            MessageBoxA(nullptr, "Something happened!", "Space Pressed", MB_OK);
        }
        else if (KeyboardButtonCheck('Z'))
        {
            OutputDebugString(L"Z held\n");
        }
        else if (KeyboardButtonPressed(VK_LEFT))
        {
            OutputDebugString(L"Left pressed\n");
        }
        else if (KeyboardButtonPressed(VK_RIGHT))
        {
            OutputDebugString(L"Right pressed\n");
        }
        else if (KeyboardButtonPressed(VK_UP))
        {
            OutputDebugString(L"Up pressed\n");
        }
        else if (KeyboardButtonPressed(VK_DOWN))
        {
            OutputDebugString(L"Down pressed\n");
        }
        else if (KeyboardButtonCheck('V') && KeyboardButtonCheck('C'))
        {
            OutputDebugStringA("C + V pressed\n");
        }
        else if (MouseButtonPressed(MouseButton::RBUTTON))
        {
            OutputDebugString(L"Mouse Right pressed\n");
        }
        else if (MouseButtonPressed(MouseButton::MBUTTON))
        {
            OutputDebugString(L"Mouse Middle pressed\n");
        }
        else if (MouseButtonReleased(MouseButton::LBUTTON))
        {
            OutputDebugString(L"Mouse Left released\n");
        }
        else if (MouseButtonReleased(MouseButton::RBUTTON))
        {
            OutputDebugString(L"Mouse Right released\n");
        }
        else if (MouseButtonReleased(MouseButton::MBUTTON))
        {
            OutputDebugString(L"Mouse Middle released\n");
        }
        else if (MouseButtonCheck(MouseButton::LBUTTON))
        {
            OutputDebugString(L"Mouse Left held\n");
        }

        InputEndFrame();
    }

    window.Destroy();

    return EXIT_SUCCESS;
}
