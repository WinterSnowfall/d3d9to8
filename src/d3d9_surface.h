#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

#include "d3d9_device.h"

using Logger = ThreadSafeLogger;

class D3D9Surface final : public IDirect3DSurface9 {

public:

  D3D9Surface(IDirect3DDevice9* device, d3d8::IDirect3DSurface8* d3d8Surface);

  ~D3D9Surface();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final;

  HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC *pDesc) final;

  HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) final;

  HRESULT STDMETHODCALLTYPE UnlockRect() final;

  HRESULT STDMETHODCALLTYPE GetDC(HDC *phDC) final;

  HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hDC) final;

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9Surface::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer) final {
    Logger::warn("D3D9Surface::GetContainer: Stub!");
    return D3D_OK;
  }

  void STDMETHODCALLTYPE PreLoad() {
    Logger::warn("D3D9Surface::PreLoad: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9Surface::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9Surface::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9Surface::FreePrivateData: Stub!");
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    Logger::warn("D3D9Surface::SetPriority: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    Logger::warn("D3D9Surface::GetPriority: Stub!");
    return 0;
  }

  d3d8::IDirect3DSurface8* GetD3D8Surface() {
    return m_d3d8;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  d3d8::IDirect3DSurface8* m_d3d8 = nullptr;

};
