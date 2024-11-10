#pragma once
#include "types.h"

bool KeyboardButtonCheck(uint8 button);
bool KeyboardButtonPressed(uint8 button);
bool KeyboardButtonReleased(uint8 button);

int MouseX();
int MouseY();
bool MouseLeftButtonPressed();
bool MouseLeftButtonReleased();
bool MouseRightButtonPressed();
bool MouseRightButtonReleased();
bool MouseMiddleButtonPressed();
bool MouseMiddleButtonReleased();

void InputClear(bool resetPrevFrameInput);
void InputEndFrame();
void KeyboardInputUpdate(uint8 button, bool pressed);
void SetMousePosition(int32 x, int32 y);

enum class MouseButton
{
    LBUTTON,
    RBUTTON,
    MBUTTON
};
void MouseInputUpdate(bool pressed, MouseButton button);
