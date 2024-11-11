#pragma once
#include "types.h"

bool KeyboardButtonCheck(u8 button);
bool KeyboardButtonPressed(u8 button);
bool KeyboardButtonReleased(u8 button);

enum class MouseButton
{
    LBUTTON,
    RBUTTON,
    MBUTTON,
    COUNT
};
int MouseX();
int MouseY();
bool MouseButtonCheck(MouseButton button);
bool MouseButtonPressed(MouseButton button);
bool MouseButtonReleased(MouseButton button);

void InputClear(bool resetPrevFrameInput);
void InputEndFrame();
void KeyboardInputUpdate(u8 button, bool pressed);
void SetMousePosition(i32 x, i32 y);
void MouseInputUpdate(MouseButton button, bool pressed);
