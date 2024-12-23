#include "input.h"

#include <bitset>

#include "types.h"

// Keyboard
constexpr u32 NUMBUTTONS = 256;
constinit std::bitset<NUMBUTTONS> s_PrevButtonState; // NOTE(sbalse): previous frame's states
constinit std::bitset<NUMBUTTONS> s_ButtonState; // NOTE(sbalse): 1 = button held down this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonDowns; // NOTE(sbalse): 1 = button pressed this frame.
constinit std::bitset<NUMBUTTONS> s_ButtonUps; // NOTE(sbalse): 1 = button released this frame.

// Mouse
constexpr u8 NUMMOUSEBUTTONS = static_cast<u8>(MouseButton::COUNT);
constinit std::bitset<NUMMOUSEBUTTONS> s_PrevMouseButtonState; // NOTE(sbalse): previous frame's states
constinit std::bitset<NUMMOUSEBUTTONS> s_MouseButtonState; // NOTE(sbalse): 1 = button held down across multiple frames
constinit std::bitset<NUMMOUSEBUTTONS> s_MouseButtonDowns; // NOTE(sbalse): 1 = button pressed once this frame
constinit std::bitset<NUMMOUSEBUTTONS> s_MouseButtonUps; // NOTE(sbalse): 1 = button released this frame.
constinit i32 s_MouseX = 0; // NOTE(sbalse): The current mouse pointer x-coordinate. Updated constantly.
constinit i32 s_MouseY = 0; // NOTE(sbalse): The current mouse pointer y-coordinate. Updated constantly.

bool InputKeyboardButtonCheck(const u8 button)
{
    return s_ButtonState[button];
}

bool InputKeyboardButtonPressed(const u8 button)
{
    return s_ButtonDowns[button] && !s_PrevButtonState[button];
}

bool InputKeyboardButtonReleased(const u8 button)
{
    return s_ButtonUps[button];
}

int InputMouseX()
{
    return s_MouseX;
}

int InputMouseY()
{
    return s_MouseY;
}

bool InputMouseButtonCheck(const MouseButton button)
{
    const u8 buttonInt = static_cast<u8>(button);
    return s_MouseButtonState[buttonInt];
}

bool InputMouseButtonPressed(const MouseButton button)
{
    const u8 buttonInt = static_cast<u8>(button);
    return s_MouseButtonDowns[buttonInt] && !s_PrevMouseButtonState[buttonInt];
}

bool InputMouseButtonReleased(const MouseButton button)
{
    const u8 buttonInt = static_cast<u8>(button);
    return s_MouseButtonUps[buttonInt];
}

void InputClear(const bool resetPrevFrameInput)
{
    s_ButtonDowns.reset();
    s_ButtonUps.reset();
    if (resetPrevFrameInput)
    {
        s_PrevButtonState.reset();
        s_PrevMouseButtonState.reset();
    }
    s_ButtonState.reset();
    s_MouseButtonDowns.reset();
    s_MouseButtonUps.reset();

    /*
    * NOTE(sbalse) : We don't call s_MouseButtonState.reset() here because unlike keyboard messages, which we
    * get sent each frame that the key is held down, mouse events only get sent once when a mouse button is
    * first pressed. So we need to track when the mouse button was last pressed and then released to be able
    * to tell if it was held down.
    */
}

void InputEndFrame()
{
    s_PrevButtonState = s_ButtonState;
    s_PrevMouseButtonState = s_MouseButtonState;
    const bool resetPrevFrameInput = false;
    InputClear(resetPrevFrameInput);
}

void InputKeyboardUpdate(const u8 button, const bool pressed)
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

void InputSetMousePosition(const i32 x, const i32 y)
{
    s_MouseX = x;
    s_MouseY = y;
}

void InputMouseUpdate(const MouseButton button, const bool pressed)
{
    const u8 buttonInt = static_cast<u8>(button);
    s_MouseButtonState[buttonInt] = pressed;

    if (pressed)
    {
        s_MouseButtonDowns[buttonInt] = true;
    }
    else
    {
        s_MouseButtonUps[buttonInt] = true;
    }
}
