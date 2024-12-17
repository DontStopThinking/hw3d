#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

bool GraphicsInit();
void GraphicsRunFrame();
bool GraphicsEndFrame();
void GraphicsProcessWindowsMessages();
void GraphicsDestroy();

void GraphicsDrawCube(
    XMFLOAT3 position,
    float pitch,
    float yaw,
    float roll
);
