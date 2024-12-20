#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "asserts.h"

using namespace DirectX;

// NOTE(sbalse): If x is not nullptr then Release() x and set it to nullptr.
#define SAFE_RELEASE(x) \
    do { \
        if (x) \
        { \
            (x)->Release(); \
            (x) = nullptr; \
        } \
    } while (0) \


inline void ValidateHRESULT(const HRESULT result)
{
    HARDASSERT(SUCCEEDED(result), "Operation resulted in a failed HRESULT");
}

struct DeviceResources
{
    // TODO(sbalse): Should we use ComPtr for these?
    ID3D11Device* m_Device;
    ID3D11DeviceContext* m_DeviceContext;
    IDXGISwapChain* m_SwapChain;
    ID3D11RenderTargetView* m_RenderTargetView;
    ID3D11DepthStencilView* m_DepthStencilView;
};

// NOTE(sbalse): The projection matrix used for all transformations.
const static FXMMATRIX g_ProjectionMatrix = XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f);
