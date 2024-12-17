#include "graphics/box.h"

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
} // namespace

Box CreateBox(
    const XMFLOAT3 position,
    const XMFLOAT3 rotation,
    const DeviceResources* deviceResources)
{
    Box result = {};

    result.m_Position = position;
    result.m_Rotation = rotation;

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
    const TransformConstantBuffer transform =
    {
        .m_Transform = XMMatrixTranspose(
            XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
            XMMatrixTranslation(position.x, position.y, position.z) *
            XMMatrixTranslation(0.0f,0.0f,20.0f) *
            g_ProjectionMatrix)
    };

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

void DrawBox(const Box* box, const DeviceResources* deviceResources)
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

void DestroyBox(Box* box)
{
    SAFE_RELEASE(box->m_VertexBuffer);
    SAFE_RELEASE(box->m_IndexBuffer);
    SAFE_RELEASE(box->m_TransformConstantBuffer);
    SAFE_RELEASE(box->m_FaceColorsConstantBuffer);
}

void RotateBoxes(
    Box* boxes,
    const size_t numberOfBoxes,
    const float rotationSpeed,
    const DeviceResources* deviceResources)
{
    for (size_t i = 0; i < numberOfBoxes; i++)
    {
        const XMFLOAT3 currentBoxPos = boxes[i].m_Rotation;

        // NOTE(sbalse): Increase rotation by given rotation speed.
        const XMFLOAT3 currentBoxRotation = boxes[i].m_Rotation;
        const XMFLOAT3 newBoxRotation = XMFLOAT3(
            currentBoxRotation.x + rotationSpeed,
            currentBoxRotation.y + rotationSpeed,
            currentBoxRotation.z + rotationSpeed);
        boxes[i].m_Rotation = newBoxRotation;

        const TransformConstantBuffer transform =
        {
            .m_Transform = XMMatrixTranspose(
                XMMatrixRotationRollPitchYaw(boxes[i].m_Rotation.x, boxes[i].m_Rotation.y, boxes[i].m_Rotation.z) *
                XMMatrixTranslation(boxes[i].m_Position.x, boxes[i].m_Position.y, boxes[i].m_Position.z) *
                XMMatrixTranslation(0.0f, 0.0f, 20.0f) *
                g_ProjectionMatrix)
        };

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
