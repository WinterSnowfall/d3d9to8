#include "d3d9_texture.h"

#include "d3d9_util.h"

D3D9Texture2D::D3D9Texture2D(
    IDirect3DDevice9* device,
    ComObject<d3d8::IDirect3DTexture8>&& d3d8Texture)
  : D3D9BaseTexture(device, reinterpret_cast<d3d8::IDirect3DBaseTexture8*>(d3d8Texture.ptr()))
  , m_d3d8 ( std::move(d3d8Texture) ) {
  // Some games pass 0 for auto-level generation,
  // so always query to get the actual level count
  m_levels.resize(m_d3d8->GetLevelCount());
}

D3D9Texture2D::~D3D9Texture2D() {
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DResource9)
          || riid == __uuidof(IDirect3DBaseTexture9)
          || riid == __uuidof(IDirect3DTexture9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Texture2D::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9Texture2D::GetType() {
  return D3DRTYPE_TEXTURE;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) {
  if (unlikely(pDesc == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels.size()))
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC d3d8SurfDesc;
  HRESULT hr = m_d3d8->GetLevelDesc(Level, &d3d8SurfDesc);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Texture2D::GetLevelDesc: Failed to get D3D8 level desc");
    return hr;
  }

  ConvertD3D8SurfaceDesc(&d3d8SurfDesc, pDesc);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel) {
  if (unlikely(ppSurfaceLevel == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels.size()))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurfaceLevel);

  if (unlikely(m_levels[Level] == nullptr)) {
    ComObject<d3d8::IDirect3DSurface8> d3d8SurfaceLevel;
    HRESULT hr = m_d3d8->GetSurfaceLevel(Level, &d3d8SurfaceLevel);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Texture2D::GetLevelDesc: Failed to get D3D8 surface level");
      return hr;
    }

    m_levels[Level] = new D3D9Surface(m_device, std::move(d3d8SurfaceLevel), this);
  }

  *ppSurfaceLevel = m_levels[Level].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) {
  return m_d3d8->LockRect(Level, reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::UnlockRect(UINT Level) {
  return m_d3d8->UnlockRect(Level);
}

HRESULT STDMETHODCALLTYPE D3D9Texture2D::AddDirtyRect(CONST RECT* pDirtyRect) {
  return m_d3d8->AddDirtyRect(pDirtyRect);
}

D3D9TextureCube::D3D9TextureCube(
    IDirect3DDevice9* device,
    ComObject<d3d8::IDirect3DCubeTexture8>&& d3d8CubeTexture)
  : D3D9BaseTexture(device, reinterpret_cast<d3d8::IDirect3DBaseTexture8*>(d3d8CubeTexture.ptr()))
  , m_d3d8 ( std::move(d3d8CubeTexture) ) {
  // Some games pass 0 for auto-level generation,
  // so always query to get the actual level count
  const DWORD levelCount = m_d3d8->GetLevelCount();
  for (auto& faceLevel : m_levels) {
    faceLevel.resize(levelCount);
  }
}

D3D9TextureCube::~D3D9TextureCube() {
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DResource9)
          || riid == __uuidof(IDirect3DBaseTexture9)
          || riid == __uuidof(IDirect3DCubeTexture9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9TextureCube::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9TextureCube::GetType() {
  return D3DRTYPE_CUBETEXTURE;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc) {
  if (unlikely(pDesc == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels[0].size()))
    return D3DERR_INVALIDCALL;

  d3d8::D3DSURFACE_DESC d3d8SurfDesc;
  HRESULT hr = m_d3d8->GetLevelDesc(Level, &d3d8SurfDesc);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Texture2D::GetLevelDesc: Failed to get D3D8 cube level desc");
    return hr;
  }

  ConvertD3D8SurfaceDesc(&d3d8SurfDesc, pDesc);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::GetCubeMapSurface(
    D3DCUBEMAP_FACES    Face,
    UINT                Level,
    IDirect3DSurface9** ppSurfaceLevel) {
  if (unlikely(ppSurfaceLevel == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Face >= 6))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels[Face].size()))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurfaceLevel);

  if (m_levels[Face][Level] == nullptr) {
    ComObject<d3d8::IDirect3DSurface8> d3d8SurfaceLevel;
    HRESULT hr = m_d3d8->GetCubeMapSurface(d3d8::D3DCUBEMAP_FACES(Face), Level, &d3d8SurfaceLevel);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Texture2D::GetLevelDesc: Failed to get D3D8 cube map surface");
      return hr;
    }

    m_levels[Face][Level] = new D3D9Surface(m_device, std::move(d3d8SurfaceLevel), this);
  }

  *ppSurfaceLevel = m_levels[Face][Level].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::LockRect(
    D3DCUBEMAP_FACES Face,
    UINT Level,
    D3DLOCKED_RECT* pLockedRect,
    const RECT* pRect,
    DWORD Flags) {
  return m_d3d8->LockRect(d3d8::D3DCUBEMAP_FACES(Face), Level,
                          reinterpret_cast<d3d8::D3DLOCKED_RECT*>(pLockedRect), pRect, Flags);
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::UnlockRect(D3DCUBEMAP_FACES Face, UINT Level) {
  return m_d3d8->UnlockRect(d3d8::D3DCUBEMAP_FACES(Face), Level);
}

HRESULT STDMETHODCALLTYPE D3D9TextureCube::AddDirtyRect(D3DCUBEMAP_FACES Face, CONST RECT* pDirtyRect) {
  return m_d3d8->AddDirtyRect(d3d8::D3DCUBEMAP_FACES(Face), pDirtyRect);
}

D3D9Texture3D::D3D9Texture3D(
    IDirect3DDevice9* device,
    ComObject<d3d8::IDirect3DVolumeTexture8>&& d3d8VolumeTexture)
  : D3D9BaseTexture(device, reinterpret_cast<d3d8::IDirect3DBaseTexture8*>(d3d8VolumeTexture.ptr()))
  , m_d3d8 ( std::move(d3d8VolumeTexture) ) {
  // Some games pass 0 for auto-level generation,
  // so always query to get the actual level count
  m_levels.resize(m_d3d8->GetLevelCount());
}

D3D9Texture3D::~D3D9Texture3D() {
}

HRESULT STDMETHODCALLTYPE D3D9Texture3D::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DResource9)
          || riid == __uuidof(IDirect3DBaseTexture9)
          || riid == __uuidof(IDirect3DVolumeTexture9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Texture3D::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9Texture3D::GetType() {
  return D3DRTYPE_VOLUMETEXTURE;
}

// The D3D9 and D3D8 D3DVOLUME_DESC structs are identical
HRESULT STDMETHODCALLTYPE D3D9Texture3D::GetLevelDesc(UINT Level, D3DVOLUME_DESC *pDesc) {
  if (unlikely(pDesc == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels.size()))
    return D3DERR_INVALIDCALL;

  HRESULT hr = m_d3d8->GetLevelDesc(Level, reinterpret_cast<d3d8::D3DVOLUME_DESC*>(&pDesc));
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Texture3D::GetVolumeLevel: Failed to get D3D8 volume level desc");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture3D::GetVolumeLevel(UINT Level, IDirect3DVolume9** ppSurfaceLevel) {
  if (unlikely(ppSurfaceLevel == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Level >= m_levels.size()))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurfaceLevel);

  if (unlikely(m_levels[Level] == nullptr)) {
    ComObject<d3d8::IDirect3DVolume8> d3d8VolumeLevel;
    HRESULT hr = m_d3d8->GetVolumeLevel(Level, &d3d8VolumeLevel);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Texture3D::GetVolumeLevel: Failed to get D3D8 volume level");
      return hr;
    }

    m_levels[Level] = new D3D9Volume(m_device, std::move(d3d8VolumeLevel), this);
  }

  *ppSurfaceLevel = m_levels[Level].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Texture3D::LockBox(UINT Level, D3DLOCKED_BOX* pLockedBox, CONST D3DBOX* pBox, DWORD Flags) {
  return m_d3d8->LockBox(Level, reinterpret_cast<d3d8::D3DLOCKED_BOX*>(pLockedBox),
                         reinterpret_cast<CONST d3d8::D3DBOX*>(pBox), Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Texture3D::UnlockBox(UINT Level) {
  return m_d3d8->UnlockBox(Level);
}

HRESULT STDMETHODCALLTYPE D3D9Texture3D::AddDirtyBox(CONST D3DBOX* pDirtyBox) {
  return m_d3d8->AddDirtyBox(reinterpret_cast<CONST d3d8::D3DBOX*>(pDirtyBox));
}
