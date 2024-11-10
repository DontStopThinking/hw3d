#include "input.h"

#include <bitset>

#include "types.h"

// Keyboard
constexpr uint32 NUMBUTTONS = 256;
constinit std::bitset<NUMBUTTONS> s_PrevButtonState; //! NOTE(sbalse): previous frame's states
constinit std::bitset<NUMBUTTONS> s_ButtonState; //! NOTE(sbalse): 1 = button held down this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonDowns; //! NOTE(sbalse): 1 = button pressed this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonUps; //! NOTE(sbalse): 1 = button released this frame.

// Mouse
constinit bool s_LeftPressed = false;
constinit bool s_LeftReleased = false;
constinit bool s_RightPressed = false;
constinit bool s_RightReleased = false;
constinit bool s_MiddlePressed = false;
constinit bool s_MiddleReleased = false;
constinit int32 s_MouseX = 0;
constinit int32 s_MouseY = 0;

bool KeyboardButtonCheck(uint8 button)
{
    return s_ButtonState[button];
}

bool KeyboardButtonPressed(uint8 button)
{
    return s_ButtonDowns[button] && !s_PrevButtonState[button];
}

bool KeyboardButtonReleased(uint8 button)
{
    return s_ButtonUps[button];
}

int MouseX()
{
    return s_MouseX;
}

int MouseY()
{
    return s_MouseY;
}

bool MouseButtonPressed(MouseButton button)
{
    bool result = false;

    switch (button)
    {
    case MouseButton::LBUTTON:
    {
        result = s_LeftPressed;
    } break;

    case MouseButton::RBUTTON:
    {
        result = s_RightPressed;
    } break;

    case MouseButton::MBUTTON:
    {
        result = s_MiddlePressed;
    } break;

    default:
    {
        result = false;
    } break;
    }

    return result;
}

bool MouseButtonReleased(MouseButton button)
{
    bool result = false;

    switch (button)
    {
    case MouseButton::LBUTTON:
    {
        result = s_LeftReleased;
    } break;

    case MouseButton::RBUTTON:
    {
        result = s_RightReleased;
    } break;

    case MouseButton::MBUTTON:
    {
        result = s_MiddleReleased;
    } break;

    default:
    {
        result = false;
    } break;
    }

    return result;
}

void InputClear(bool resetPrevFrameInput)
{
    s_ButtonDowns.reset();
    s_ButtonUps.reset();
    if (resetPrevFrameInput)
    {
        s_PrevButtonState.reset();
    }
    s_ButtonState.reset();
    s_LeftPressed = false;
    s_LeftReleased = false;
    s_RightPressed = false;
    s_RightReleased = false;
    s_MiddlePressed = false;
    s_MiddleReleased = false;
}

void InputEndFrame()
{
    s_PrevButtonState = s_ButtonState;
    InputClear(false);
}

void KeyboardInputUpdate(uint8 button, bool pressed)
{
    s_ButtonState[button] = pressed;
    if (pressed)
    {
        s_ButtonDowns[button] = true;
    }
    else
    {
        s_ButtonUps[button] = true;
    }
}

void SetMousePosition(int32 x, int32 y)
{
    s_MouseX = x;
    s_MouseY = y;
}

void MouseInputUpdate(bool pressed, MouseButton button)
{
    switch (button)
    {
    case MouseButton::LBUTTON:
    {
        if (pressed)
        {
            s_LeftPressed = true;
        }
        else
        {
            s_LeftReleased = true;
        }
    } break;

    case MouseButton::RBUTTON:
    {
        if (pressed)
        {
            s_RightPressed = true;
        }
        else
        {
            s_RightReleased = true;
        }
    } break;

    case MouseButton::MBUTTON:
    {
        if (pressed)
        {
            s_MiddlePressed = true;
        }
        else
        {
            s_MiddleReleased = true;
        }
    } break;
    }
}
