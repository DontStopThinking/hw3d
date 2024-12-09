#include "graphics.h"

#include <cmath>
#include "cleanwindows.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "asserts.h"
#include "input.h"
#include "mathutils.h"
#include "window.h"
#include "utils.h"
#include "graphics/graphicsutils.h"
#include "graphics/vertex.h"

namespace
{
// TODO(sbalse): Should we ComPtr for these?
constinit ID3D11Device* g_D3DDevice = nullptr;
constinit IDXGISwapChain* g_D3DSwapChain = nullptr;
constinit ID3D11DeviceContext* g_D3DDeviceContext = nullptr;
constinit ID3D11RenderTargetView* g_D3DRenderTargetView = nullptr;
constinit ID3D11DepthStencilView* g_D3DDepthStencilView = nullptr;

Window g_Window;

void InitDeviceAndSwapChain();
void InitDepthStencilAndRenderTargetView();
void InitShaders();

void GraphicsClearBuffer(const float r, const float g, const float b);
void DrawTestCube(float angle, float cubeX, float cubeZ);

void ValidateHRESULT(const HRESULT result);
} // namespace

bool GraphicsInit()
{
    constexpr int windowWidth = 1280;
    constexpr int windowHeight = 720;
    constexpr LPCWSTR windowTitle = L"HW3D Engine";

    if (!g_Window.Init(windowWidth, windowHeight, windowTitle))
    {
        // TODO(sbalse): Logging
        return false;
    }

    InitDeviceAndSwapChain();

    InitDepthStencilAndRenderTargetView();

    InitShaders();

    g_Window.Show();

    return true;
}

void GraphicsRunFrame()
{
    static float i = 0;
    const float color = std::sinf(i) / 2.0f + 0.5f;
    //constexpr float cauliflowerBlue[] = { 0.588f, 0.745f, 0.827f };

    GraphicsClearBuffer(color, color, 1.0f);

    i = PingPong(i, 0.0f, 10.0f, 0.02f); // NOTE(sbalse): Oscillate value between min and max.

    const float mouseX = (static_cast<float>(InputMouseX()) / (g_Window.GetWidth() / 2.0f)) - 1.0f;
    const float mouseY = ((static_cast<float>(InputMouseY()) / (g_Window.GetHeight() / 2.0f)) - 1.0f) * -1.0f;

    DrawTestCube(-i, 0.0f, 0.0f);
    DrawTestCube(i, mouseX, mouseY);
}

bool GraphicsEndFrame()
{
    g_D3DSwapChain->Present(1, 0);

    return g_Window.IsRunning();
}

void GraphicsDestroy()
{
    g_D3DDeviceContext->ClearState();

    SAFE_RELEASE(g_D3DDepthStencilView);
    SAFE_RELEASE(g_D3DRenderTargetView);
    SAFE_RELEASE(g_D3DDeviceContext);
    SAFE_RELEASE(g_D3DSwapChain);
    SAFE_RELEASE(g_D3DDevice);
}

void GraphicsProcessWindowsMessages()
{
    g_Window.ProcessMessages();
}

namespace
{
namespace dx = DirectX;

void InitDeviceAndSwapChain()
{
    DXGI_SWAP_CHAIN_DESC swapChainDescription =
    {
        .BufferDesc =   // Details of the buffer that will hold the window's contents.
        {
            .Width = 0, // Width of the buffer in pixels.
            .Height = 0, // Height of the buffer in pixels.
            .RefreshRate =  // Refresh rate of the contents of the window.
            {
                .Numerator = 0,
                .Denominator = 0
            },
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM, // Format that the texture resource should use.
            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, // The raster technique for image generation.
            .Scaling = DXGI_MODE_SCALING_UNSPECIFIED // How the buffer will be applied to the window's client area.
        },
        .SampleDesc = // Options for multisampling anti-aliasing (MSAA)
        {
            .Count = 1, // The number of samples.
            .Quality = 0 // The quality of their sampling pattern.
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT, // The intended usage of the swap chain.
        .BufferCount = 1, // How many buffers in the swap chain.
        .OutputWindow = g_Window.GetWindowHandle(), // Handle of the window that will receive the swap chain.
        .Windowed = true, // Whether the window should appear windowed or fullscreen.
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD, // What is done to the buffer contents after they are presented to the window.
        .Flags = 0, // Misc. flags to further configure the buffer.
    };

    u32 d3dRuntimeLayerFlags = 0;
#if _DEBUG
    d3dRuntimeLayerFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    // NOTE(sbalse): Create D3D device, front/back buffers, swap chain and rendering context.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // the video adapter to use
        D3D_DRIVER_TYPE_HARDWARE,   // the driver type
        nullptr,                    // handle to dll that implements a software rasterizer
        d3dRuntimeLayerFlags,       // runtime layers
        nullptr,                    // feature levels
        0,                          // no. of feature levels
        D3D11_SDK_VERSION,          // sdk version,
        &swapChainDescription,      // the swap chain description
        &g_D3DSwapChain,            // the swap chain used to rendering
        &g_D3DDevice,               // the D3D device
        nullptr,                    // feature levels supported by the device
        &g_D3DDeviceContext         // get the device context
    );
    ValidateHRESULT(hr);
}

void InitDepthStencilAndRenderTargetView()
{
    // NOTE(sbalse): Get the back buffer of the swap chain
    ID3D11Resource* backBuffer = nullptr;
    DEFER(SAFE_RELEASE(backBuffer));

    HRESULT hr = g_D3DSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    HARDASSERT(backBuffer, "backBuffer is nullptr");
    ValidateHRESULT(hr);

    hr = g_D3DDevice->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &g_D3DRenderTargetView);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Create depth stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc =
    {
        .DepthEnable = true,
        .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
        .DepthFunc = D3D11_COMPARISON_LESS
    };

    ID3D11DepthStencilState* depthStencilState = nullptr;
    DEFER(SAFE_RELEASE(depthStencilState));

    hr = g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind depth state.
    g_D3DDeviceContext->OMSetDepthStencilState(depthStencilState, 1u);

    // NOTE(sbalse): Create depth stencil texture.
    ID3D11Texture2D* depthStencil = nullptr;
    DEFER(SAFE_RELEASE(depthStencil));

    D3D11_TEXTURE2D_DESC depthDesc =
    {
        .Width = static_cast<u32>(g_Window.GetWidth()),
        .Height = static_cast<u32>(g_Window.GetHeight()),
        .MipLevels = 1u,
        .ArraySize = 1u,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .SampleDesc =
        {
            .Count = 1u,
            .Quality = 0u,
        },
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
    };

    hr = g_D3DDevice->CreateTexture2D(&depthDesc, nullptr, &depthStencil);
    HARDASSERT(depthStencil, "Failed to create a depth stencil texture");
    ValidateHRESULT(hr);

    // NOTE(sbalse): Create view of depth stencil texture.
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc =
    {
        .Format = DXGI_FORMAT_D32_FLOAT,
        .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
        .Texture2D =
        {
            .MipSlice = 0u,
        },
    };

    hr = g_D3DDevice->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &g_D3DDepthStencilView);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind depth stencil view to OM.
    g_D3DDeviceContext->OMSetRenderTargets(1u, &g_D3DRenderTargetView, g_D3DDepthStencilView);

    // Configure viewport
    D3D11_VIEWPORT viewport =
    {
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width = static_cast<float>(g_Window.GetWidth()),
        .Height = static_cast<float>(g_Window.GetHeight()),
        .MinDepth = 0,
        .MaxDepth = 1
    };
    g_D3DDeviceContext->RSSetViewports(1u, &viewport);
}

void InitShaders()
{
    ID3DBlob* blob = nullptr;
    DEFER(SAFE_RELEASE(blob));

    // NOTE(sbalse): Create vertex shader
    ID3D11VertexShader* vertexShader = nullptr;
    DEFER(SAFE_RELEASE(vertexShader));

    HRESULT hr = D3DReadFileToBlob(L"vertexshader.cso", &blob);
    ValidateHRESULT(hr);

    hr = g_D3DDevice->CreateVertexShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &vertexShader
    );
    ValidateHRESULT(hr);

    g_D3DDeviceContext->VSSetShader(vertexShader, nullptr, 0u);

    // NOTE(sbalse): Create Input Layout
    ID3D11InputLayout* inputLayout = nullptr;
    DEFER(SAFE_RELEASE(inputLayout));

    const D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
    {
        {
            .SemanticName = "Position",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = 0,
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0
        },
    };

    hr = g_D3DDevice->CreateInputLayout(
        inputLayoutDesc,
        static_cast<u32>(ArraySize(inputLayoutDesc)),
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        &inputLayout
    );
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind vertex layout
    g_D3DDeviceContext->IASetInputLayout(inputLayout);

    // NOTE(sbalse): Create pixel shader
    ID3D11PixelShader* pixelShader = nullptr;
    DEFER(SAFE_RELEASE(pixelShader));

    hr = D3DReadFileToBlob(L"pixelshader.cso", &blob);
    ValidateHRESULT(hr);

    hr = g_D3DDevice->CreatePixelShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &pixelShader
    );
    ValidateHRESULT(hr);

    g_D3DDeviceContext->PSSetShader(pixelShader, nullptr, 0u);
}

void GraphicsClearBuffer(const float r, const float g, const float b)
{
    const float color[] = { r, g, b, 1.0f };
    g_D3DDeviceContext->ClearRenderTargetView(g_D3DRenderTargetView, color);
    g_D3DDeviceContext->ClearDepthStencilView(g_D3DDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void DrawTestCube(float angle, float cubeX, float cubeZ)
{
    // NOTE(sbalse): Create vertex buffer
    constexpr Vertex vertices[] =
    {
        // NOTE(sbalse): Cube
        { .m_Pos = { -1.0f, -1.0f, -1.0f } },
        { .m_Pos = { 1.0f, -1.0f, -1.0f } },
        { .m_Pos = { -1.0f, 1.0f, -1.0f } },
        { .m_Pos = { 1.0f, 1.0f, -1.0f } },
        { .m_Pos = { -1.0f, -1.0f, 1.0f } },
        { .m_Pos = { 1.0f, -1.0f, 1.0f } },
        { .m_Pos = { -1.0f, 1.0f, 1.0f } },
        { .m_Pos = { 1.0f, 1.0f, 1.0f } },
    };

    ID3D11Buffer* vertexBuffer = nullptr;
    DEFER(SAFE_RELEASE(vertexBuffer));

    D3D11_BUFFER_DESC bufferDesc =
    {
        .ByteWidth = sizeof(vertices),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = sizeof(Vertex)
    };

    D3D11_SUBRESOURCE_DATA subResourceVertices =
    {
        .pSysMem = vertices
    };

    HRESULT hr = g_D3DDevice->CreateBuffer(&bufferDesc, &subResourceVertices, &vertexBuffer);
    ValidateHRESULT(hr);

    constexpr u32 stride = sizeof(Vertex);
    constexpr u32 offset = 0u;

    g_D3DDeviceContext->IASetVertexBuffers(0u, 1u, &vertexBuffer, &stride, &offset);

    // NOTE(sbalse): Create index buffer
    constexpr u16 indices[] =
    {
        0, 2, 1, 2, 3, 1,
        1, 3, 5, 3, 7, 5,
        2, 6, 3, 3, 6, 7,
        4, 5, 7, 4, 7, 6,
        0, 4, 2, 2, 4, 6,
        0, 1, 4, 1, 5, 4,
    };

    ID3D11Buffer* indexBuffer = nullptr;
    DEFER(SAFE_RELEASE(indexBuffer));

    D3D11_BUFFER_DESC indexBufferDesc =
    {
        .ByteWidth = sizeof(indices),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = sizeof(u16),
    };

    D3D11_SUBRESOURCE_DATA subResourceIndices =
    {
        .pSysMem = indices,
    };

    hr = g_D3DDevice->CreateBuffer(&indexBufferDesc, &subResourceIndices, &indexBuffer);
    ValidateHRESULT(hr);

    g_D3DDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0u);

    // NOTE(sbalse): Create constant buffer for transformation matrix
    struct TransformCB
    {
        dx::XMMATRIX m_Transform;
    };

    const TransformCB transformCBData =
    {
        .m_Transform = dx::XMMatrixTranspose(
            dx::XMMatrixRotationZ(angle)
            * dx::XMMatrixRotationX(angle)
            * dx::XMMatrixTranslation(cubeX, 0.0f, cubeZ + 4.0f)
            * dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)),
    };

    ID3D11Buffer* transformCB = nullptr;
    DEFER(SAFE_RELEASE(transformCB));

    D3D11_BUFFER_DESC transformCBDesc =
    {
        .ByteWidth = sizeof(transformCBData),
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        .MiscFlags = 0u,
        .StructureByteStride = 0u,
    };

    D3D11_SUBRESOURCE_DATA transformCBSubres =
    {
        .pSysMem = &transformCBData
    };

    hr = g_D3DDevice->CreateBuffer(&transformCBDesc, &transformCBSubres, &transformCB);
    ValidateHRESULT(hr);

    g_D3DDeviceContext->VSSetConstantBuffers(0u, 1u, &transformCB);

    // NOTE(sbalse): Create constant buffer for cube face colors
    struct FaceColorsCB
    {
        struct
        {
            float m_R;
            float m_G;
            float m_B;
            float m_A;
        } m_FaceColors[6];
    };

    FaceColorsCB faceColorsCBData =
    {
        .m_FaceColors =
        {
            {.m_R = 1.0f, .m_G = 0.0f, .m_B = 1.0f },
            {.m_R = 1.0f, .m_G = 0.0f, .m_B = 0.0f },
            {.m_R = 0.0f, .m_G = 1.0f, .m_B = 0.0f },
            {.m_R = 0.0f, .m_G = 0.0f, .m_B = 1.0f },
            {.m_R = 1.0f, .m_G = 1.0f, .m_B = 0.0f },
            {.m_R = 0.0f, .m_G = 1.0f, .m_B = 1.0f },
        }
    };

    ID3D11Buffer* faceColorsCB = nullptr;
    DEFER(SAFE_RELEASE(faceColorsCB));

    D3D11_BUFFER_DESC faceColorsCBDesc =
    {
        .ByteWidth = sizeof(faceColorsCBData),
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = 0u,
        .MiscFlags = 0u,
        .StructureByteStride = 0u,
    };

    D3D11_SUBRESOURCE_DATA faceColorsSubres =
    {
        .pSysMem = &faceColorsCBData
    };

    hr = g_D3DDevice->CreateBuffer(
        &faceColorsCBDesc,
        &faceColorsSubres,
        &faceColorsCB);
    ValidateHRESULT(hr);

    g_D3DDeviceContext->PSSetConstantBuffers(0u, 1u, &faceColorsCB);

    // Set primitive topology to triangle list (groups of 3 vertices).
    g_D3DDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    constexpr u32 indexCount = static_cast<u32>(ArraySize(indices));
    g_D3DDeviceContext->DrawIndexed(indexCount, 0u, 0);
}

void ValidateHRESULT(const HRESULT result)
{
    HARDASSERT(SUCCEEDED(result), "Operation resulted in a failed HRESULT");
}
} // namespace
