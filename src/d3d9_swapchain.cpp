#include "d3d9_swapchain.h"

#include "d3d9_device.h"

D3D9SwapChain::D3D9SwapChain(
    IDirect3DDevice9* device,
    ComObject<d3d8::IDirect3DSwapChain8>&& swapChain8,
    D3DPRESENT_PARAMETERS* pPresentationParameters)
  : m_device ( device )
  , m_d3d8 ( std::move(swapChain8) ) {
  if (likely(m_d3d8 == nullptr && m_device != nullptr)) {
    m_isImplicit = true;
  }
  if (unlikely(pPresentationParameters != nullptr)) {
    m_presentParams = *pPresentationParameters;
  }
}

D3D9SwapChain::~D3D9SwapChain() {
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DSwapChain9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9SwapChainEx::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::Present(
  const RECT*    pSourceRect,
  const RECT*    pDestRect,
        HWND     hDestWindowOverride,
  const RGNDATA* pDirtyRegion,
        DWORD    dwFlags) {
  if (unlikely(dwFlags != 0))
    Logger::warn("D3D9SwapChain::Present: Use of non-zero dwFlags");

  if (likely(m_isImplicit)) {
    return m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
  } else {
    return m_d3d8->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetFrontBufferData(IDirect3DSurface9* pDestSurface) {
  if (unlikely(pDestSurface == nullptr))
    return D3DERR_INVALIDCALL;

  if (likely(m_isImplicit))
    return m_device->GetFrontBufferData(0, pDestSurface);

  Logger::warn("D3D9SwapChain::GetFrontBufferData: Unsupported call!");

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetBackBuffer(
        UINT                iBackBuffer,
        D3DBACKBUFFER_TYPE  Type,
        IDirect3DSurface9** ppBackBuffer) {
  if (unlikely(ppBackBuffer == nullptr))
    return D3DERR_INVALIDCALL;

  if (likely(m_isImplicit)) {
    return m_device->GetBackBuffer(0, iBackBuffer, Type, ppBackBuffer);
  } else {
    ComObject<d3d8::IDirect3DSurface8> backBuffer8;
    HRESULT hr = m_d3d8->GetBackBuffer(iBackBuffer, d3d8::D3DBACKBUFFER_TYPE(Type), &backBuffer8);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9SwapChain::GetBackBuffer: Failed to get D3D8 back buffer");
      return hr;
    }

    *ppBackBuffer = ref(new D3D9Surface(m_device, std::move(backBuffer8), nullptr));
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) {
  if (unlikely(pRasterStatus == nullptr))
    return D3DERR_INVALIDCALL;

  if (likely(m_isImplicit))
    return m_device->GetRasterStatus(0, pRasterStatus);

  Logger::warn("D3D9SwapChain::GetRasterStatus: Unsupported call!");

  D3DRASTER_STATUS rasterStatus = { };
  *pRasterStatus = rasterStatus;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetDisplayMode(D3DDISPLAYMODE* pMode) {
  if (unlikely(pMode == nullptr))
    return D3DERR_INVALIDCALL;

  if (likely(m_isImplicit))
    return m_device->GetDisplayMode(0, pMode);

  Logger::warn("D3D9SwapChain::GetDisplayMode: Unsupported call!");

  D3DDISPLAYMODE displayMode = { };
  *pMode = displayMode;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters) {
  if (unlikely(pPresentationParameters == nullptr))
    return D3DERR_INVALIDCALL;

  if (likely(m_isImplicit)) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    *pPresentationParameters = *d3d9Device->GetPresentParameters();
  } else {
    *pPresentationParameters = m_presentParams;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetLastPresentCount(UINT* pLastPresentCount) {
  Logger::warn("D3D9SwapChain::GetLastPresentCount: Unsupported call!");

  if (likely(pLastPresentCount != nullptr))
    return 0;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9SwapChain::GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics) {
  Logger::warn("D3D9SwapChain::GetPresentStats: Unsupported call!");

  if (likely(pPresentationStatistics != nullptr)) {
    D3DPRESENTSTATS presentStats = { };
    *pPresentationStatistics = presentStats;
  }

  return D3D_OK;
}
