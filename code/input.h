#pragma once
#include "types.h"

namespace Input
{
    void OnButton(uint8 button, bool pressed);
    bool ButtonCheck(uint8 button);
    bool ButtonPressed(uint8 button);
    bool ButtonReleased(uint8 button);
    void EndFrame();
}
