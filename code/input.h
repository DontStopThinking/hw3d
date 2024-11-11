#pragma once
#include "types.h"

bool KeyboardButtonCheck(uint8 button);
bool KeyboardButtonPressed(uint8 button);
bool KeyboardButtonReleased(uint8 button);

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
void KeyboardInputUpdate(uint8 button, bool pressed);
void SetMousePosition(int32 x, int32 y);
void MouseInputUpdate(MouseButton button, bool pressed);
