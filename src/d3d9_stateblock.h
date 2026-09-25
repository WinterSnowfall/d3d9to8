#pragma once

#include "d3d9_include.h"
#include "d3d9_device_child.h"
#include "d3d9_logger.h"

#include "d3d9_device.h"

using Logger = ThreadSafeLogger;

class D3D9StateBlock final : public D3D9DeviceChild<IDirect3DStateBlock9> {

public:

  D3D9StateBlock(IDirect3DDevice9* device, DWORD handle);

  ~D3D9StateBlock();

  HRESULT STDMETHODCALLTYPE QueryInterface(
      REFIID  riid,
      void** ppvObject) final;

  HRESULT STDMETHODCALLTYPE Capture() final;

  HRESULT STDMETHODCALLTYPE Apply() final;

private:

  DWORD m_handle = 0u;

};
