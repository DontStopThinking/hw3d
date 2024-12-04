#include "control.h"

#include "graphics.h"
#include "input.h"

namespace
{
    void InputTest()
    {
        if (InputKeyboardButtonPressed(VK_ESCAPE))
        {
            OutputDebugString(L"Escape pressed\n");
            PostQuitMessage(0); // NOTE(sbalse): Quit game on escape pressed.
        }
        else if (InputKeyboardButtonPressed('W'))
        {
            OutputDebugString(L"W pressed\n");
        }
        else if (InputKeyboardButtonReleased('W'))
        {
            OutputDebugString(L"W released\n");
        }
        else if (InputKeyboardButtonPressed(VK_SPACE))
        {
            MessageBoxA(nullptr, "Something happened!", "Space Pressed", MB_OK);
        }
        else if (InputKeyboardButtonCheck('Z'))
        {
            OutputDebugString(L"Z held\n");
        }
        else if (InputKeyboardButtonPressed(VK_LEFT))
        {
            OutputDebugString(L"Left pressed\n");
        }
        else if (InputKeyboardButtonPressed(VK_RIGHT))
        {
            OutputDebugString(L"Right pressed\n");
        }
        else if (InputKeyboardButtonPressed(VK_UP))
        {
            OutputDebugString(L"Up pressed\n");
        }
        else if (InputKeyboardButtonPressed(VK_DOWN))
        {
            OutputDebugString(L"Down pressed\n");
        }
        else if (InputKeyboardButtonCheck('V') && InputKeyboardButtonCheck('C'))
        {
            OutputDebugStringA("C + V pressed\n");
        }
        else if (InputMouseButtonPressed(MouseButton::RBUTTON))
        {
            OutputDebugString(L"Mouse Right pressed\n");
        }
        else if (InputMouseButtonPressed(MouseButton::MBUTTON))
        {
            OutputDebugString(L"Mouse Middle pressed\n");
        }
        else if (InputMouseButtonReleased(MouseButton::LBUTTON))
        {
            OutputDebugString(L"Mouse Left released\n");
        }
        else if (InputMouseButtonReleased(MouseButton::RBUTTON))
        {
            OutputDebugString(L"Mouse Right released\n");
        }
        else if (InputMouseButtonReleased(MouseButton::MBUTTON))
        {
            OutputDebugString(L"Mouse Middle released\n");
        }
        else if (InputMouseButtonCheck(MouseButton::LBUTTON))
        {
            OutputDebugString(L"Mouse Left held\n");
        }
    }

    void GameLogic()
    {
        InputTest();
    }

    void RunFrame()
    {
        GraphicsProcessWindowsMessages();

        GameLogic();

        GraphicsDoFrame();
    }

    bool EndFrame()
    {
        InputEndFrame();
        const bool isRunning = GraphicsEndFrame();
        return isRunning;
    }

    bool Run()
    {
        RunFrame();
        const bool isRunning = EndFrame(); // NOTE(sbalse): Has quit been requested at the end of a frame.
        return isRunning;
    }
}

bool ControlInit()
{
    if (!GraphicsInit())
    {
        // TODO(sbalse): Logging
        return false;
    }

    return true;
}

bool ControlRun()
{
    return Run();
}

void ControlShutdown()
{
    GraphicsDestroy();
}
