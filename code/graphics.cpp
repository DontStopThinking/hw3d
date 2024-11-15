#include "graphics.h"

#include <cmath>

#include "asserts.h"
#include "window.h"

namespace
{
    constinit ID3D11Device* g_D3DDevice = nullptr;
    constinit IDXGISwapChain* g_D3DSwapChain = nullptr;
    constinit ID3D11DeviceContext* g_D3DDeviceContext = nullptr;
    constinit ID3D11RenderTargetView* g_D3DRenderTargetView = nullptr;

    Window g_Window;

    void GraphicsClearBuffer(float r, float g, float b)
    {
        float color[] = { r, g, b, 1.0f };
        g_D3DDeviceContext->ClearRenderTargetView(g_D3DRenderTargetView, color);
    }

    void ValidateHRESULT(HRESULT result)
    {
        HARDASSERT(SUCCEEDED(result), "Operation resulted in a failed HRESULT");
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
        i = 10.0f; // NOTE(sbalse): stop i from increasing uncontrollably. Seems like the right thing to do?
    }
}

bool GraphicsEndFrame()
{
    g_D3DSwapChain->Present(1, 0);

    return g_Window.IsRunning();
}
