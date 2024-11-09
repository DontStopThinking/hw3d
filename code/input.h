#pragma once
#include "types.h"

void InputUpdate(uint8 button, bool pressed);
bool KeyboardButtonCheck(uint8 button);
bool KeyboardButtonPressed(uint8 button);
bool KeyboardButtonReleased(uint8 button);
void InputClear();
void InputEndFrame();
