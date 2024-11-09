#pragma once
#include "types.h"

bool KeyboardButtonCheck(uint8 button);
bool KeyboardButtonPressed(uint8 button);
bool KeyboardButtonReleased(uint8 button);

void InputClear();
void InputEndFrame();
void InputUpdate(uint8 button, bool pressed);
