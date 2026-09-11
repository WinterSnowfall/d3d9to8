#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

#include "d3d9_device.h"

using Logger = ThreadSafeLogger;

class D3D9StateBlock final : public IDirect3DStateBlock9 {

public:

  D3D9StateBlock(IDirect3DDevice9* device, DWORD handle);

  ~D3D9StateBlock();

  HRESULT STDMETHODCALLTYPE QueryInterface(
      REFIID  riid,
      void** ppvObject) final;

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

  HRESULT STDMETHODCALLTYPE Capture() final;

  HRESULT STDMETHODCALLTYPE Apply() final;

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9StateBlock::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  DWORD m_handle;

};
