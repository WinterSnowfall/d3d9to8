#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include "d3d9_device.h"

using Logger = ThreadSafeLogger;

class D3D9StateBlock final : public ComObjectClamp<IDirect3DStateBlock9> {

public:

  D3D9StateBlock(IDirect3DDevice9* device, DWORD handle);

  ~D3D9StateBlock();

  HRESULT STDMETHODCALLTYPE QueryInterface(
      REFIID  riid,
      void** ppvObject) final;

  HRESULT STDMETHODCALLTYPE Capture() final;

  HRESULT STDMETHODCALLTYPE Apply() final;

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (unlikely(ppDevice == nullptr))
      return D3DERR_INVALIDCALL;

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  DWORD             m_handle = 0;

};
