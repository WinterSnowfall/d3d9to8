#include "d3d9_surface.h"

#include "d3d9_util.h"

D3D9Surface::D3D9Surface(
    IDirect3DDevice9* device,
    ComObject<d3d8::IDirect3DSurface8>&& d3d8Surface,
    IUnknown* container)
  : D3D9Resource (device, reinterpret_cast<d3d8::IDirect3DResource8*>(d3d8Surface.ptr()))
  , m_d3d8 ( std::move(d3d8Surface) )
  , m_container ( container ) {
}

D3D9Surface::~D3D9Surface() {
}

ULONG STDMETHODCALLTYPE D3D9Surface::AddRef() {
  // Subresources ref the parent container
  if (m_container != nullptr)
    return m_container->AddRef();

  return D3D9Resource::AddRef();
}

ULONG STDMETHODCALLTYPE D3D9Surface::Release() {
  // Subresources release the parent container
  if (m_container != nullptr)
    return m_container->Release();

  return D3D9Resource::Release();
}

HRESULT STDMETHODCALLTYPE D3D9Surface::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DResource9)
          || riid == __uuidof(IDirect3DSurface9))) {
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
  if (unlikely(pDesc == nullptr))
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC surfDesc8;
  HRESULT hr = m_d3d8->GetDesc(&surfDesc8);
  if (unlikely(FAILED(hr)))
    return hr;

  ConvertD3D8SurfaceDesc(&surfDesc8, pDesc);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::LockRect(D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
  return m_d3d8->LockRect(reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Surface::UnlockRect() {
  return m_d3d8->UnlockRect();
}

HRESULT STDMETHODCALLTYPE D3D9Surface::GetDC(HDC *phDC) {
  if (unlikely(phDC == nullptr))
    return D3DERR_INVALIDCALL;

  // GetDC calls are only supported on a limited set of surface
  // formats, so we can pretend we don't support anything at all
  Logger::err("D3D9Surface::GetDC: Unsupported call!");
  return D3DERR_INVALIDCALL;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::ReleaseDC(HDC hDC) {
  Logger::debug("D3D9Surface::ReleaseDC: Unsupported call!");
  return D3DERR_INVALIDCALL;
}

HRESULT STDMETHODCALLTYPE D3D9Surface::GetContainer(REFIID riid, void** ppContainer) {
  if (m_container != nullptr)
    return m_container->QueryInterface(riid, ppContainer);

  return m_device->QueryInterface(riid, ppContainer);
}
