#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_resource.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

template <typename TextureType>
class D3D9BaseTexture : public D3D9Resource<TextureType> {

public:

  D3D9BaseTexture(IDirect3DDevice9* device, d3d8::IDirect3DBaseTexture8* baseTexture8)
    : D3D9Resource<TextureType>(device, reinterpret_cast<d3d8::IDirect3DResource8*>(baseTexture8))
    , m_d3d8 ( baseTexture8 ) {
  }

  ~D3D9BaseTexture() {
  }

  void STDMETHODCALLTYPE GenerateMipSubLevels() final {
    Logger::warn("D3D9BaseTexture::GenerateMipSubLevels: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) final {
    Logger::warn("D3D9BaseTexture::SetAutoGenFilterType: Stub!");
    return D3D_OK;
  }

  D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE GetAutoGenFilterType() final {
    Logger::warn("D3D9BaseTexture::GetAutoGenFilterType: Stub!");
    return D3DTEXTUREFILTERTYPE(0);
  }

  DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) final {
    Logger::warn("D3D9BaseTexture::SetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLOD() final {
    Logger::warn("D3D9BaseTexture::GetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLevelCount() final {
    Logger::info("D3D9Texture2D::GetLevelCount:");
    return m_d3d8->GetLevelCount();
  }

private:

  d3d8::IDirect3DBaseTexture8* m_d3d8 = nullptr;

};

class D3D9Texture2D : public D3D9BaseTexture<IDirect3DTexture9> {

public:

  D3D9Texture2D(IDirect3DDevice9* device, d3d8::IDirect3DTexture8* d3d8Texture);

  ~D3D9Texture2D();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);

  HRESULT STDMETHODCALLTYPE GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel);

  HRESULT STDMETHODCALLTYPE LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);

  HRESULT STDMETHODCALLTYPE UnlockRect(UINT Level);

  HRESULT STDMETHODCALLTYPE AddDirtyRect(CONST RECT* pDirtyRect);

  d3d8::IDirect3DTexture8* GetD3D8Texture() {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DTexture8> m_d3d8;

};

class D3D9TextureCube : public D3D9BaseTexture<IDirect3DCubeTexture9> {

public:

  D3D9TextureCube(IDirect3DDevice9* device, d3d8::IDirect3DCubeTexture8* d3d8CubeTexture);

  ~D3D9TextureCube();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);

  HRESULT STDMETHODCALLTYPE GetCubeMapSurface(
      D3DCUBEMAP_FACES    Face,
      UINT                Level,
      IDirect3DSurface9** ppSurfaceLevel);

  HRESULT STDMETHODCALLTYPE LockRect(
      D3DCUBEMAP_FACES Face,
      UINT Level,
      D3DLOCKED_RECT* pLockedRect,
      const RECT* pRect,
      DWORD Flags);

  HRESULT STDMETHODCALLTYPE UnlockRect(D3DCUBEMAP_FACES Face, UINT Level);

  HRESULT STDMETHODCALLTYPE AddDirtyRect(D3DCUBEMAP_FACES Face, CONST RECT* pDirtyRect);

  d3d8::IDirect3DCubeTexture8* GetD3D8CubeTexture() {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DCubeTexture8> m_d3d8;

};

class D3D9Texture3D : public D3D9BaseTexture<IDirect3DVolumeTexture9> {

public:

  D3D9Texture3D(IDirect3DDevice9* device, d3d8::IDirect3DVolumeTexture8* d3d8VolumeTexture);

  ~D3D9Texture3D();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DVOLUME_DESC *pDesc);

  HRESULT STDMETHODCALLTYPE GetVolumeLevel(UINT Level, IDirect3DVolume9** ppSurfaceLevel);

  HRESULT STDMETHODCALLTYPE LockBox(UINT Level, D3DLOCKED_BOX* pLockedBox, CONST D3DBOX* pBox, DWORD Flags);

  HRESULT STDMETHODCALLTYPE UnlockBox(UINT Level);

  HRESULT STDMETHODCALLTYPE AddDirtyBox(CONST D3DBOX* pDirtyBox);

  d3d8::IDirect3DVolumeTexture8* GetD3D8VolumeTexture() {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DVolumeTexture8> m_d3d8;

};