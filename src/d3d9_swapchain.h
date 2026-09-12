#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9SwapChain final : public ComObjectClamp<IDirect3DSwapChain9> {

public:

  D3D9SwapChain(IDirect3DDevice9* device, d3d8::IDirect3DSwapChain8* swapChain8);

  ~D3D9SwapChain();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE Present(
    const RECT*    pSourceRect,
    const RECT*    pDestRect,
          HWND     hDestWindowOverride,
    const RGNDATA* pDirtyRegion,
          DWORD    dwFlags);

  HRESULT STDMETHODCALLTYPE GetFrontBufferData(IDirect3DSurface9* pDestSurface);

  HRESULT STDMETHODCALLTYPE GetBackBuffer(
          UINT                iBackBuffer,
          D3DBACKBUFFER_TYPE  Type,
          IDirect3DSurface9** ppBackBuffer);

  HRESULT STDMETHODCALLTYPE GetRasterStatus(D3DRASTER_STATUS* pRasterStatus);

  HRESULT STDMETHODCALLTYPE GetDisplayMode(D3DDISPLAYMODE* pMode);

  HRESULT STDMETHODCALLTYPE GetPresentParameters(D3DPRESENT_PARAMETERS* pPresentationParameters);

  HRESULT STDMETHODCALLTYPE GetLastPresentCount(UINT* pLastPresentCount);

  HRESULT STDMETHODCALLTYPE GetPresentStats(D3DPRESENTSTATS* pPresentationStatistics);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9SwapChain::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  ComObject<d3d8::IDirect3DSwapChain8> m_d3d8;

};
