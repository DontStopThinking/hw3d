#pragma once

#include "cleanwindows.h"

#include <d3d11.h>

bool GraphicsInit();
void GraphicsDestroy();
void GraphicsProcessWindowsMessages();
void GraphicsRunFrame();
bool GraphicsEndFrame();
