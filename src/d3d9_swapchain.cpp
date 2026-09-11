#include "d3d9_swapchain.h"

D3D9SwapChain::D3D9SwapChain(IDirect3DDevice9* device, d3d8::IDirect3DSwapChain8* swapChain8)
: m_device ( device )
, m_d3d8 ( swapChain8 ) {
}

D3D9SwapChain::~D3D9SwapChain() {
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DSwapChain9)) {
    //this->AddRef();
    *ppvObject = this;
    return S_OK;
  }

  Logger::warn("D3D9SwapChainEx::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::Present(
  const RECT*    pSourceRect,
  const RECT*    pDestRect,
        HWND     hDestWindowOverride,
  const RGNDATA* pDirtyRegion,
        DWORD    dwFlags) {
  Logger::warn("D3D9SwapChain::Present: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetFrontBufferData(IDirect3DSurface9* pDestSurface) {
  Logger::warn("D3D9SwapChain::GetFrontBufferData: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetBackBuffer(
        UINT                iBackBuffer,
        D3DBACKBUFFER_TYPE  Type,
        IDirect3DSurface9** ppBackBuffer) {
  Logger::warn("D3D9SwapChain::GetBackBuffer: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) {
  Logger::warn("D3D9SwapChain::GetRasterStatus: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetDisplayMode(D3DDISPLAYMODE* pMode) {
  Logger::warn("D3D9SwapChain::GetDisplayMode: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters) {
  Logger::warn("D3D9SwapChain::GetPresentParameters: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetLastPresentCount(UINT* pLastPresentCount) {
  Logger::warn("D3D9SwapChain::GetLastPresentCount: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics) {
  Logger::warn("D3D9SwapChain::GetPresentStats: Stub!");
  return D3D_OK;
}