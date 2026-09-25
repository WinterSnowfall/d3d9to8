#pragma once

#include "d3d9_include.h"
#include "d3d9_resource.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9Surface final : public D3D9Resource<IDirect3DSurface9> {

public:

  D3D9Surface(
      IDirect3DDevice9* device,
      ComObject<d3d8::IDirect3DSurface8>&& d3d8Surface,
      IUnknown* container);

  ~D3D9Surface();

  ULONG STDMETHODCALLTYPE AddRef();

  ULONG STDMETHODCALLTYPE Release();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType() final;

  HRESULT STDMETHODCALLTYPE GetDesc(D3DSURFACE_DESC *pDesc) final;

  HRESULT STDMETHODCALLTYPE LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) final;

  HRESULT STDMETHODCALLTYPE UnlockRect() final;

  HRESULT STDMETHODCALLTYPE GetDC(HDC *phDC) final;

  HRESULT STDMETHODCALLTYPE ReleaseDC(HDC hDC) final;

  HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer) final;

  d3d8::IDirect3DSurface8* GetD3D8Surface() const {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DSurface8> m_d3d8;

  IUnknown*                          m_container = nullptr;

};
