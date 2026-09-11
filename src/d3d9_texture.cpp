#include "d3d9_texture.h"

#include "d3d9_surface.h"
#include "d3d9_util.h"

D3D9Texture2D::D3D9Texture2D(IDirect3DDevice9* device, d3d8::IDirect3DTexture8* d3d8Texture)
  : m_device ( device )
  , m_d3d8( d3d8Texture ) {
}

D3D9Texture2D::~D3D9Texture2D() {
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DResource9)
    || riid == __uuidof(IDirect3DBaseTexture9)
    || riid == __uuidof(IDirect3DTexture9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9Texture2D::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9Texture2D::GetType() {
  return D3DRTYPE_TEXTURE;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) {
  Logger::info("D3D9Texture2D::GetLevelDesc:");

  if (pDesc == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC d3d8SurfDesc;
  HRESULT hr = m_d3d8->GetLevelDesc(Level, &d3d8SurfDesc);
  if (FAILED(hr))
    return hr;

  D3DSURFACE_DESC d3d9SurfDesc = ConvertSurfaceDesc8(&d3d8SurfDesc);

  *pDesc = d3d9SurfDesc;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) {
  Logger::info("D3D9Texture2D::GetSurfaceLevel:");

  if (ppSurfaceLevel == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DSurface8* d3d8SurfaceLevel;
  HRESULT hr = m_d3d8->GetSurfaceLevel(Level, &d3d8SurfaceLevel);
  if (FAILED(hr))
    return hr;

  D3D9Surface* d3d9SurfaceLevel = new D3D9Surface(m_device, d3d8SurfaceLevel);
  *ppSurfaceLevel = d3d9SurfaceLevel->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
  Logger::info("D3D9Texture2D::LockRect:");
  return m_d3d8->LockRect(Level, reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::UnlockRect(UINT Level) {
  Logger::info("D3D9Texture2D::UnlockRect:");
  return m_d3d8->UnlockRect(Level);
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::AddDirtyRect(CONST RECT* pDirtyRect) {
  Logger::info("D3D9Texture2D::AddDirtyRect:");
  return m_d3d8->AddDirtyRect(pDirtyRect);
}

D3D9TextureCube::D3D9TextureCube(IDirect3DDevice9* device, d3d8::IDirect3DCubeTexture8* d3d8CubeTexture)
  : m_device ( device )
  , m_d3d8( d3d8CubeTexture ) {
}

D3D9TextureCube::~D3D9TextureCube() {
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DResource9)
    || riid == __uuidof(IDirect3DBaseTexture9)
    || riid == __uuidof(IDirect3DCubeTexture9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9TextureCube::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9TextureCube::GetType() {
  return D3DRTYPE_CUBETEXTURE;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) {
  Logger::info("D3D9TextureCube::GetLevelDesc:");

  if (pDesc == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC d3d8SurfDesc;
  HRESULT hr = m_d3d8->GetLevelDesc(Level, &d3d8SurfDesc);
  if (FAILED(hr))
    return hr;

  D3DSURFACE_DESC d3d9SurfDesc = ConvertSurfaceDesc8(&d3d8SurfDesc);

  *pDesc = d3d9SurfDesc;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::GetCubeMapSurface(
    D3DCUBEMAP_FACES    Face,
    UINT                Level,
    IDirect3DSurface9** ppSurfaceLevel) {
  Logger::info("D3D9TextureCube::GetCubeMapSurface:");

  if (ppSurfaceLevel == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DSurface8* d3d8SurfaceLevel;
  HRESULT hr = m_d3d8->GetCubeMapSurface(d3d8::D3DCUBEMAP_FACES(Face), Level, &d3d8SurfaceLevel);
  if (FAILED(hr))
    return hr;

  D3D9Surface* d3d9SurfaceLevel = new D3D9Surface(m_device, d3d8SurfaceLevel);
  *ppSurfaceLevel = d3d9SurfaceLevel->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::LockRect(
      D3DCUBEMAP_FACES Face,
      UINT Level,
      D3DLOCKED_RECT* pLockedRect,
      const RECT* pRect,
      DWORD Flags) {
  Logger::info("D3D9TextureCube::LockRect:");
  return m_d3d8->LockRect(d3d8::D3DCUBEMAP_FACES(Face), Level,
                          reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::UnlockRect(D3DCUBEMAP_FACES Face, UINT Level) {
  Logger::info("D3D9TextureCube::UnlockRect:");
  return m_d3d8->UnlockRect(d3d8::D3DCUBEMAP_FACES(Face), Level);
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::AddDirtyRect(D3DCUBEMAP_FACES Face, CONST RECT* pDirtyRect) {
  Logger::info("D3D9TextureCube::AddDirtyRect:");
  return m_d3d8->AddDirtyRect(d3d8::D3DCUBEMAP_FACES(Face), pDirtyRect);
}
