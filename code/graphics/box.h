#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "graphics/graphicsutils.h"

using namespace DirectX;

struct Box
{
    XMFLOAT3 m_Position;
    XMFLOAT3 m_Rotation;
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    ID3D11Buffer* m_TransformConstantBuffer;
    ID3D11Buffer* m_FaceColorsConstantBuffer;
};

// NOTE(sbalse): Rotation is in the form of pitch, yaw roll.
Box CreateBox(
    const XMFLOAT3 position,
    const XMFLOAT3 rotation,
    const DeviceResources* deviceResources);
void DrawBox(const Box* box, const DeviceResources* deviceResources);
void DestroyBox(Box* box);
void RotateBoxes(
    Box* boxes,
    const size_t numberOfBoxes,
    const float rotationSpeed,
    const DeviceResources* deviceResources);
