#pragma once
#include "types.h"

bool InputKeyboardButtonCheck(u8 button);
bool InputKeyboardButtonPressed(u8 button);
bool InputKeyboardButtonReleased(u8 button);

enum class MouseButton
{
    LBUTTON,
    RBUTTON,
    MBUTTON,
    COUNT
};
int InputMouseX();
int InputMouseY();
bool InputMouseButtonCheck(MouseButton button);
bool InputMouseButtonPressed(MouseButton button);
bool InputMouseButtonReleased(MouseButton button);

void InputClear(bool resetPrevFrameInput);
void InputEndFrame();
void InputKeyboardUpdate(u8 button, bool pressed);
void InputSetMousePosition(i32 x, i32 y);
void InputMouseUpdate(MouseButton button, bool pressed);
