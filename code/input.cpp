#include "input.h"

#include <bitset>

#include "types.h"

constexpr uint32 NUMBUTTONS = 256;

constinit std::bitset<NUMBUTTONS> s_PrevButtonState; //! NOTE(sbalse): previous frame's states
constinit std::bitset<NUMBUTTONS> s_ButtonState; //! NOTE(sbalse): 1 = button held down this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonDowns; //! NOTE(sbalse): 1 = button pressed this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonUps; //! NOTE(sbalse): 1 = button released this frame.

void InputUpdate(uint8 button, bool pressed)
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

void InputClear()
{
    s_ButtonDowns.reset();
    s_ButtonUps.reset();
    s_PrevButtonState.reset();
    s_ButtonState.reset();
}

void InputEndFrame()
{
    s_PrevButtonState = s_ButtonState;
    s_ButtonDowns.reset();
    s_ButtonUps.reset();
}
