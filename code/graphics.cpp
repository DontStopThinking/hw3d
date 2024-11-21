#include "graphics.h"

#include <cmath>
#include <d3dcompiler.h>

#include "utils.h"
#include "asserts.h"
#include "window.h"

namespace
{
    // TODO(sbalse): We're currently using asserts and crash the game in case of serious errors; And we don't
    // use exceptions. Should we still use a ComPtr for these pointers?
    constinit ID3D11Device* g_D3DDevice = nullptr;
    constinit IDXGISwapChain* g_D3DSwapChain = nullptr;
    constinit ID3D11DeviceContext* g_D3DDeviceContext = nullptr;
    constinit ID3D11RenderTargetView* g_D3DRenderTargetView = nullptr;

    Window g_Window;

    void GraphicsClearBuffer(const float r, const float g, const float b)
    {
        const float color[] = { r, g, b, 1.0f };
        g_D3DDeviceContext->ClearRenderTargetView(g_D3DRenderTargetView, color);
    }

    void ValidateHRESULT(const HRESULT result)
    {
        HARDASSERT(SUCCEEDED(result), "Operation resulted in a failed HRESULT");
    }

    void DrawTestTriangle()
    {
        struct Vertex
        {
            float m_X;
            float m_Y;
            float m_R;
            float m_G;
            float m_B;
        };

        // Create vertex buffer (one 2D triangle at center of the window).
        const Vertex vertices[] =
        {
            // Triangle 1
            { 0.0f, 0.5f, 1.0f, 0.0f, 0.0f },
            { 0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
            { -0.5f, -0.5f, 0.0f, 0.0f, 1.0f },
        };

        ID3D11Buffer* vertexBuffer = nullptr;
        DEFER(vertexBuffer->Release());

        D3D11_BUFFER_DESC bufferDesc =
        {
            .ByteWidth = sizeof(vertices),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u,
            .StructureByteStride = sizeof(Vertex)
        };

        D3D11_SUBRESOURCE_DATA subResource =
        {
            .pSysMem = vertices
        };

        HRESULT hr = g_D3DDevice->CreateBuffer(&bufferDesc, &subResource, &vertexBuffer);
        ValidateHRESULT(hr);

        // Bind vertex buffer to pipeline.
        const u32 stride = sizeof(Vertex);
        const u32 offset = 0u;

        g_D3DDeviceContext->IASetVertexBuffers(0u, 1u, &vertexBuffer, &stride, &offset);

        // Create pixel shader
        ID3D11PixelShader* pixelShader = nullptr;
        DEFER(pixelShader->Release());

        ID3DBlob* blob = nullptr;
        DEFER(blob->Release());

        hr = D3DReadFileToBlob(L"pixelshader.cso", &blob);
        ValidateHRESULT(hr);

        hr = g_D3DDevice->CreatePixelShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            &pixelShader
        );
        ValidateHRESULT(hr);

        // Bind pixel shader
        g_D3DDeviceContext->PSSetShader(pixelShader, nullptr, 0u);

        // Create vertex shader
        ID3D11VertexShader* vertexShader = nullptr;
        DEFER(vertexShader->Release());

        hr = D3DReadFileToBlob(L"vertexshader.cso", &blob);
        ValidateHRESULT(hr);

        hr = g_D3DDevice->CreateVertexShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr,
            &vertexShader
        );
        ValidateHRESULT(hr);

        // Bind vertex shader
        g_D3DDeviceContext->VSSetShader(vertexShader, nullptr, 0u);

        // Input (vertex) layout (2D positon only)
        ID3D11InputLayout* inputLayout = nullptr;
        DEFER(inputLayout->Release());

        const D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[] =
        {
            {
                .SemanticName = "Position",
                .SemanticIndex = 0,
                .Format = DXGI_FORMAT_R32G32_FLOAT,
                .InputSlot = 0,
                .AlignedByteOffset = 0,
                .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
                .InstanceDataStepRate = 0
            },
            {
                .SemanticName = "Color",
                .SemanticIndex = 0,
                .Format = DXGI_FORMAT_R32G32B32_FLOAT,
                .InputSlot = 0,
                .AlignedByteOffset = 8u,
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

        // Bind vertex layout
        g_D3DDeviceContext->IASetInputLayout(inputLayout);

        // Bind render target
        g_D3DDeviceContext->OMSetRenderTargets(1u, &g_D3DRenderTargetView, nullptr);

        // Set primitive topology to triangle list (groups of 3 vertices).
        g_D3DDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // Configure viewport
        D3D11_VIEWPORT viewport =
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = static_cast<FLOAT>(g_Window.GetWidth()),
            .Height = static_cast<FLOAT>(g_Window.GetHeight()),
            .MinDepth = 0,
            .MaxDepth = 1
        };
        g_D3DDeviceContext->RSSetViewports(1u, &viewport);

        constexpr u32 vertexCount = static_cast<u32>(ArraySize(vertices));
        g_D3DDeviceContext->Draw(vertexCount, 0u);
    }
}

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

    DXGI_SWAP_CHAIN_DESC swapChainDescription =
    {
        .BufferDesc =
        {
            .Width = 0,
            .Height = 0,
            .RefreshRate =
            {
                .Numerator = 0,
                .Denominator = 0
            },
            .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
            .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
            .Scaling = DXGI_MODE_SCALING_UNSPECIFIED
        },
        .SampleDesc =
        {
            .Count = 1,
            .Quality = 0
        },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 1,
        .OutputWindow = g_Window.GetWindowHandle(),
        .Windowed = true,
        .SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
        .Flags = 0
    };

    u32 d3dRuntimeLayerFlags = 0;
#if _DEBUG
    d3dRuntimeLayerFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

    HRESULT hr = {};

    // NOTE(sbalse): Create D3D device, front/back buffers, swap chain and rendering context.
    hr = D3D11CreateDeviceAndSwapChain(
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

    // NOTE(sbalse): Get the back buffer of the swap chain
    ID3D11Resource* backBuffer = nullptr;
    hr = g_D3DSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer));
    HARDASSERT(backBuffer, "backBuffer is nullptr");
    ValidateHRESULT(hr);

    hr = g_D3DDevice->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &g_D3DRenderTargetView);
    ValidateHRESULT(hr);

    backBuffer->Release();

    g_Window.Show();

    return true;
}

void GraphicsDestroy()
{
    if (g_D3DRenderTargetView)
    {
        g_D3DRenderTargetView->Release();
    }

    if (g_D3DDeviceContext)
    {
        g_D3DDeviceContext->Release();
    }

    if (g_D3DSwapChain)
    {
        g_D3DSwapChain->Release();
    }

    if (g_D3DDevice)
    {
        g_D3DDevice->Release();
    }
}

void GraphicsProcessWindowsMessages()
{
    g_Window.ProcessMessages();
}

void GraphicsDoFrame()
{
    static float i = 0;
    const float c = std::sinf(i) / 2.0f + 0.5f;
    //constexpr float cauliflowerBlue[] = { 0.588f, 0.745f, 0.827f };
    GraphicsClearBuffer(c, c, 1.0f);
    i += 0.02;
    if (i >= 10.0f)
    {
        i = 0.0f; // NOTE(sbalse): stop i from increasing uncontrollably. Seems like the right thing to do?
    }

    DrawTestTriangle();
}

bool GraphicsEndFrame()
{
    g_D3DSwapChain->Present(1, 0);

    return g_Window.IsRunning();
}
