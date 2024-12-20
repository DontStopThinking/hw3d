#include "graphics/rotatingbox.h"

#include <cstring>

#include "utils.h"
#include "types.h"
#include "graphics/graphicsutils.h"

namespace
{
    constexpr XMFLOAT3 g_CubeVertices[] =
    {
        // NOTE(sbalse): Cube
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
    };

    constexpr u16 g_CubeIndices[] =
    {
        0, 2, 1, 2, 3, 1,
        1, 3, 5, 3, 7, 5,
        2, 6, 3, 3, 6, 7,
        4, 5, 7, 4, 7, 6,
        0, 4, 2, 2, 4, 6,
        0, 1, 4, 1, 5, 4,
    };
    constexpr u32 g_CubeIndicesCount = static_cast<u32>(ArraySize(g_CubeIndices));

    struct TransformConstantBuffer
    {
        XMMATRIX m_Transform;
    };

    struct FaceColorsConstantBuffer
    {
        // TODO(sbalse): Use a XMFLOAT4 here.
        struct
        {
            float m_R;
            float m_G;
            float m_B;
            float m_A;
        } m_FaceColors[6];
    };

    TransformConstantBuffer ApplyTransformation(const RotatingBox* box)
    {
        const TransformConstantBuffer result =
        {
            .m_Transform = XMMatrixTranspose(
                XMMatrixRotationRollPitchYaw(
                    box->m_SelfRotation.x,
                    box->m_SelfRotation.y,
                    box->m_SelfRotation.z) *
                XMMatrixTranslation(box->m_Position.x, box->m_Position.y, box->m_Position.z) *
                XMMatrixRotationRollPitchYaw(
                    box->m_WorldRotation.x,
                    box->m_WorldRotation.y,
                    box->m_WorldRotation.z) *
                XMMatrixTranslation(0.0f, 0.0f, 20.0f) *
                g_ProjectionMatrix)
        };

        return result;
    }
} // namespace

RotatingBox CreateRotatingBox(
    const float distanceFromCenterOfWorld,
    const float selfRotation,
    const float selfRotationSpeed,
    const float worldRotation,
    const float worldRotationSpeed,
    const DeviceResources* const deviceResources)
{
    RotatingBox result = {};

    result.m_Position = XMFLOAT3(distanceFromCenterOfWorld, 0.0f, 0.0f);
    result.m_SelfRotation = XMFLOAT3(
        selfRotation,
        selfRotation,
        selfRotation);
    result.m_SelfRotationSpeed = selfRotationSpeed;
    result.m_WorldRotation = XMFLOAT3(
        worldRotation,
        worldRotation,
        worldRotation);
    result.m_WorldRotationSpeed = worldRotationSpeed;

    // NOTE(sbalse): Create vertex buffer.
    D3D11_BUFFER_DESC bufferDesc =
    {
        .ByteWidth = sizeof(g_CubeVertices),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = sizeof(XMFLOAT3)
    };

    D3D11_SUBRESOURCE_DATA subResourceVertices =
    {
        .pSysMem = g_CubeVertices
    };

    HRESULT hr = deviceResources->m_Device->CreateBuffer(
        &bufferDesc,
        &subResourceVertices,
        &result.m_VertexBuffer);
    ValidateHRESULT(hr);

    // Note(sbalse): Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc =
    {
        .ByteWidth = sizeof(g_CubeIndices),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = sizeof(u16),
    };

    D3D11_SUBRESOURCE_DATA subResourceIndices =
    {
        .pSysMem = g_CubeIndices,
    };

    hr = deviceResources->m_Device->CreateBuffer(
        &indexBufferDesc,
        &subResourceIndices,
        &result.m_IndexBuffer);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Create the transformation constant buffer
    const TransformConstantBuffer transform = ApplyTransformation(&result);

    D3D11_SUBRESOURCE_DATA transformInitData =
    {
        .pSysMem = &transform
    };

    D3D11_BUFFER_DESC transformDesc =
    {
        .ByteWidth = sizeof(TransformConstantBuffer),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0u,
        .StructureByteStride = 0u,
    };

    hr = deviceResources->m_Device->CreateBuffer(
        &transformDesc,
        &transformInitData,
        &result.m_TransformConstantBuffer);

    ValidateHRESULT(hr);

    // NOTE(sbalse): Create the box face colors constant buffer
    const FaceColorsConstantBuffer faceColorsCB =
    {
        .m_FaceColors =
        {
            { .m_R = 1.0f, .m_G = 0.0f, .m_B = 1.0f },
            { .m_R = 1.0f, .m_G = 0.0f, .m_B = 0.0f },
            { .m_R = 0.0f, .m_G = 1.0f, .m_B = 0.0f },
            { .m_R = 0.0f, .m_G = 0.0f, .m_B = 1.0f },
            { .m_R = 1.0f, .m_G = 1.0f, .m_B = 0.0f },
            { .m_R = 0.0f, .m_G = 1.0f, .m_B = 1.0f },
        }
    };

    const D3D11_SUBRESOURCE_DATA faceColorsInitData =
    {
        .pSysMem = &faceColorsCB
    };

    const D3D11_BUFFER_DESC faceColorsDesc =
    {
        .ByteWidth = sizeof(FaceColorsConstantBuffer),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = 0u,
    };

    hr = deviceResources->m_Device->CreateBuffer(
        &faceColorsDesc,
        &faceColorsInitData,
        &result.m_FaceColorsConstantBuffer
    );
    ValidateHRESULT(hr);

    return result;
}

void DrawRotatingBox(const RotatingBox* const box, const DeviceResources* const deviceResources)
{
    // NOTE(sbalse): Set primitive topology to triangle list (groups of 3 vertices).
    deviceResources->m_DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    constexpr u32 stride = sizeof(XMFLOAT3);
    constexpr u32 offset = 0u;
    deviceResources->m_DeviceContext->IASetVertexBuffers(0u, 1u, &box->m_VertexBuffer, &stride, &offset);
    deviceResources->m_DeviceContext->IASetIndexBuffer(box->m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0u);
    deviceResources->m_DeviceContext->VSSetConstantBuffers(0u, 1u, &box->m_TransformConstantBuffer);
    deviceResources->m_DeviceContext->PSSetConstantBuffers(0u, 1u, &box->m_FaceColorsConstantBuffer);
    deviceResources->m_DeviceContext->DrawIndexed(g_CubeIndicesCount, 0u, 0);
}

void DestroyRotatingBox(RotatingBox* box)
{
    SAFE_RELEASE(box->m_VertexBuffer);
    SAFE_RELEASE(box->m_IndexBuffer);
    SAFE_RELEASE(box->m_TransformConstantBuffer);
    SAFE_RELEASE(box->m_FaceColorsConstantBuffer);
}

void UpdateRotatingBoxes(
    RotatingBox* boxes,
    const size_t numberOfBoxes,
    const DeviceResources* const deviceResources)
{
    for (size_t i = 0; i < numberOfBoxes; i++)
    {
        const XMFLOAT3 currentBoxPos = boxes[i].m_Position;

        // NOTE(sbalse): Increase rotation by given rotation speed.
        const XMFLOAT3 currentBoxSelfRotation = boxes[i].m_SelfRotation;
        const XMFLOAT3 newBoxSelfRotation = XMFLOAT3(
            currentBoxSelfRotation.x + boxes[i].m_SelfRotationSpeed,
            currentBoxSelfRotation.y + boxes[i].m_SelfRotationSpeed,
            currentBoxSelfRotation.z + boxes[i].m_SelfRotationSpeed);
        boxes[i].m_SelfRotation = newBoxSelfRotation;

        const XMFLOAT3 currentBoxWorldRotation = boxes[i].m_WorldRotation;
        const XMFLOAT3 newBoxWorldRotation = XMFLOAT3(
            currentBoxWorldRotation.x + boxes[i].m_WorldRotationSpeed,
            currentBoxWorldRotation.y + boxes[i].m_WorldRotationSpeed,
            currentBoxWorldRotation.z + boxes[i].m_WorldRotationSpeed);
        boxes[i].m_WorldRotation = newBoxWorldRotation;

        const TransformConstantBuffer transform = ApplyTransformation(&boxes[i]);

        D3D11_MAPPED_SUBRESOURCE mappedResource = {};
        HRESULT hr = deviceResources->m_DeviceContext->Map(
            boxes[i].m_TransformConstantBuffer,
            0u,
            D3D11_MAP_WRITE_DISCARD,
            0u,
            &mappedResource
        );

        std::memcpy(mappedResource.pData, &transform, sizeof(TransformConstantBuffer));

        deviceResources->m_DeviceContext->Unmap(boxes[i].m_TransformConstantBuffer, 0u);
    }
}
