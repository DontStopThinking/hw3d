#include "graphics.h"

#include <cmath>
#include <random>
#include "cleanwindows.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "asserts.h"
#include "input.h"
#include "mathutils.h"
#include "window.h"
#include "utils.h"
#include "graphics/box.h"
#include "graphics/graphicsutils.h"
#include "graphics/vertex.h"

using namespace DirectX;

namespace
{
    constinit DeviceResources g_DeviceResources = {};

    // NOTE(sbalse): The main window.
    Window g_Window = {};

    void InitDeviceAndSwapChain();
    void InitDepthStencilAndRenderTargetView();
    void InitShaders();

    void GraphicsClearBuffer(const float r, const float g, const float b);
} // namespace

constexpr int g_TotalNumberOfBoxes = 40;
constinit Box g_Boxes[g_TotalNumberOfBoxes] = {};

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

    // Init all cube positions
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<float> randomCubePosDistribution(-10.0f, 10.0f);
    std::uniform_real_distribution<float> randomCubeRotDistribution(-10.0f, 10.0f);

    for (int i = 0; i < g_TotalNumberOfBoxes; i++)
    {
        const float x = randomCubePosDistribution(rng);
        const float y = randomCubePosDistribution(rng);
        const float z = randomCubePosDistribution(rng);
        const float rot = randomCubeRotDistribution(rng);

        const XMFLOAT3 position(x, y, z);
        const XMFLOAT3 rotation(rot, rot, rot);

        g_Boxes[i] = CreateBox(position, rotation, &g_DeviceResources);
    }

    g_Window.Show();

    return true;
}

void GraphicsRunFrame()
{
    //static float i = 0;
    //const float color = std::sinf(i) / 2.0f + 0.5f;
    //constexpr float cauliflowerBlue[] = { 0.588f, 0.745f, 0.827f };
    constexpr float color = 0.0f;

    GraphicsClearBuffer(color, color, color);

    //i = PingPong(i, 0.0f, 10.0f, 0.02f); // NOTE(sbalse): Oscillate value between min and max.

    // NOTE(sbalse): Rotate boxes
    RotateBoxes(g_Boxes, g_TotalNumberOfBoxes, 0.01f, &g_DeviceResources);

    // NOTE(sbalse): Draw boxes.
    for (int j = 0; j < g_TotalNumberOfBoxes; j++)
    {
        DrawBox(&g_Boxes[j], &g_DeviceResources);
    }
}

bool GraphicsEndFrame()
{
    g_DeviceResources.m_SwapChain->Present(1, 0);

    return g_Window.IsRunning();
}

void GraphicsDestroy()
{
    for (int j = 0; j < g_TotalNumberOfBoxes; j++)
    {
        DestroyBox(&g_Boxes[j]);
    }

    g_DeviceResources.m_DeviceContext->ClearState();

    SAFE_RELEASE(g_DeviceResources.m_DepthStencilView);
    SAFE_RELEASE(g_DeviceResources.m_RenderTargetView);
    SAFE_RELEASE(g_DeviceResources.m_DeviceContext);
    SAFE_RELEASE(g_DeviceResources.m_SwapChain);
    SAFE_RELEASE(g_DeviceResources.m_Device);
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
        &g_DeviceResources.m_SwapChain,            // the swap chain used to rendering
        &g_DeviceResources.m_Device,               // the D3D device
        nullptr,                    // feature levels supported by the device
        &g_DeviceResources.m_DeviceContext         // get the device context
    );
    ValidateHRESULT(hr);
}

void InitDepthStencilAndRenderTargetView()
{
    // NOTE(sbalse): Get the back buffer of the swap chain
    ID3D11Resource* backBuffer = nullptr;
    DEFER(SAFE_RELEASE(backBuffer));

    HRESULT hr = g_DeviceResources.m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    HARDASSERT(backBuffer, "backBuffer is nullptr");
    ValidateHRESULT(hr);

    hr = g_DeviceResources.m_Device->CreateRenderTargetView(
        backBuffer,
        nullptr,
        &g_DeviceResources.m_RenderTargetView);
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

    hr = g_DeviceResources.m_Device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind depth state.
    g_DeviceResources.m_DeviceContext->OMSetDepthStencilState(depthStencilState, 1u);

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

    hr = g_DeviceResources.m_Device->CreateTexture2D(&depthDesc, nullptr, &depthStencil);
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

    hr = g_DeviceResources.m_Device->CreateDepthStencilView(
        depthStencil,
        &depthStencilViewDesc,
        &g_DeviceResources.m_DepthStencilView);
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind depth stencil view to OM.
    g_DeviceResources.m_DeviceContext->OMSetRenderTargets(
        1u,
        &g_DeviceResources.m_RenderTargetView,
        g_DeviceResources.m_DepthStencilView);

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
    g_DeviceResources.m_DeviceContext->RSSetViewports(1u, &viewport);
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

    hr = g_DeviceResources.m_Device->CreateVertexShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &vertexShader
    );
    ValidateHRESULT(hr);

    g_DeviceResources.m_DeviceContext->VSSetShader(vertexShader, nullptr, 0u);

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

    hr = g_DeviceResources.m_Device->CreateInputLayout(
        inputLayoutDesc,
        static_cast<u32>(ArraySize(inputLayoutDesc)),
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        &inputLayout
    );
    ValidateHRESULT(hr);

    // NOTE(sbalse): Bind vertex layout
    g_DeviceResources.m_DeviceContext->IASetInputLayout(inputLayout);

    // NOTE(sbalse): Create pixel shader
    ID3D11PixelShader* pixelShader = nullptr;
    DEFER(SAFE_RELEASE(pixelShader));

    hr = D3DReadFileToBlob(L"pixelshader.cso", &blob);
    ValidateHRESULT(hr);

    hr = g_DeviceResources.m_Device->CreatePixelShader(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &pixelShader
    );
    ValidateHRESULT(hr);

    g_DeviceResources.m_DeviceContext->PSSetShader(pixelShader, nullptr, 0u);
}

void GraphicsClearBuffer(const float r, const float g, const float b)
{
    const float color[] = { r, g, b, 1.0f };
    g_DeviceResources.m_DeviceContext->ClearRenderTargetView(g_DeviceResources.m_RenderTargetView, color);
    g_DeviceResources.m_DeviceContext->ClearDepthStencilView(
        g_DeviceResources.m_DepthStencilView,
        D3D11_CLEAR_DEPTH,
        1.0f,
        0u);
}
} // namespace
