#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "graphics/graphicsutils.h"

using namespace DirectX;

struct RotatingBox
{
    XMFLOAT3 m_Position;
    // NOTE(sbalse): Rotation is in the form of pitch, yaw roll.
    XMFLOAT3 m_SelfRotation;
    float m_SelfRotationSpeed;
    XMFLOAT3 m_WorldRotation;
    float m_WorldRotationSpeed;
    ID3D11Buffer* m_VertexBuffer;
    ID3D11Buffer* m_IndexBuffer;
    ID3D11Buffer* m_TransformConstantBuffer;
    ID3D11Buffer* m_FaceColorsConstantBuffer;
};

// NOTE(sbalse): Rotation is in the form of pitch, yaw roll.
RotatingBox CreateRotatingBox(
    const float distanceFromCenterOfWorld,
    const float selfRotation,
    const float selfRotationSpeed,
    const float worldRotation,
    const float worldRotationSpeed,
    const DeviceResources* const deviceResources);
void DrawRotatingBox(const RotatingBox* const box, const DeviceResources* const deviceResources);
void DestroyRotatingBox(RotatingBox* box);
void UpdateRotatingBoxes(
    RotatingBox* boxes,
    const size_t numberOfBoxes,
    const DeviceResources* deviceResources);
