#include "input.h"

#include <bitset>

#include "types.h"

namespace Input
{
    constexpr uint32 NUMBUTTONS = 256;

    constinit std::bitset<NUMBUTTONS> s_PrevButtonState; //! previous frame's states
    constinit std::bitset<NUMBUTTONS> s_ButtonState; //! 1 = button held down this frame.
    constinit std::bitset<NUMBUTTONS> s_ButtonDowns; //! 1 = button pressed this frame.
    constinit std::bitset<NUMBUTTONS> s_ButtonUps; //! 1 = button released this frame.

    void OnButton(uint8 button, bool pressed)
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

    bool ButtonCheck(uint8 button)
    {
        return s_ButtonState[button];
    }

    bool ButtonPressed(uint8 button)
    {
        return s_ButtonDowns[button] && !s_PrevButtonState[button];
    }

    bool ButtonReleased(uint8 button)
    {
        return s_ButtonUps[button];
    }

    void Clear()
    {
        s_ButtonDowns.reset();
        s_ButtonUps.reset();
        s_PrevButtonState.reset();
        s_ButtonState.reset();
    }

    void EndFrame()
    {
        s_PrevButtonState = s_ButtonState;
        s_ButtonDowns.reset();
        s_ButtonUps.reset();
    }
}
