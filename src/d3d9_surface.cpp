#include "d3d9_surface.h"

#include "d3d9_util.h"

D3D9Surface::D3D9Surface(IDirect3DDevice9* device, d3d8::IDirect3DSurface8* d3d8Surface)
  : D3D9Resource (device, reinterpret_cast<d3d8::IDirect3DResource8*>(d3d8Surface))
  , m_d3d8 ( d3d8Surface ) {
}

D3D9Surface::~D3D9Surface() {
}

HRESULT STDMETHODCALLTYPE D3D9Surface::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DResource9)
    || riid == __uuidof(IDirect3DSurface9)) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Surface::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9Surface::GetType() {
  return D3DRTYPE_SURFACE;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::GetDesc(D3DSURFACE_DESC *pDesc) {
  if (pDesc == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC surfDesc8;
  HRESULT hr = m_d3d8->GetDesc(&surfDesc8);
  if (FAILED(hr))
    return hr;

  D3DSURFACE_DESC surfDesc9 = ConvertSurfaceDesc8(&surfDesc8);

  *pDesc = surfDesc9;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
  return m_d3d8->LockRect(reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Surface::UnlockRect() {
  return m_d3d8->UnlockRect();
}

HRESULT STDMETHODCALLTYPE D3D9Surface::GetDC(HDC *phDC) {
  Logger::err("D3D9Surface::GetDC: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::ReleaseDC(HDC hDC) {
  Logger::err("D3D9Surface::ReleaseDC: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::GetContainer(REFIID riid, void** ppContainer) {
  Logger::warn("D3D9Surface::GetContainer: Stub!");
  return D3D_OK;
}

