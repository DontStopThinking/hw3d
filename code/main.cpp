#include <cstdlib>
#include <cstdio>

#include "cleanwindows.h"

#include "window.h"
#include "input.h"

static void DoFrame()
{
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
}

static void EndFrame()
{
    InputEndFrame();
}

int APIENTRY WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    Window window(1280, 720, L"HW3D Engine");

    if (!window.InitAndShow())
    {
        // TODO(sbalse): Logging
        return EXIT_FAILURE;
    }

    while (window.ProcessMessages())
    {
        DoFrame();

        EndFrame();
    }

    window.Destroy();

    return EXIT_SUCCESS;
}
