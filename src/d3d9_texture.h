#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9Texture2D : public IDirect3DTexture9 {

public:

  D3D9Texture2D(IDirect3DDevice9* device, d3d8::IDirect3DTexture8* d3d8Texture);

  ~D3D9Texture2D();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetLevelDesc(UINT Level, D3DSURFACE_DESC *pDesc);

  HRESULT STDMETHODCALLTYPE GetSurfaceLevel(UINT Level, IDirect3DSurface9** ppSurfaceLevel);

  HRESULT STDMETHODCALLTYPE LockRect(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags);

  HRESULT STDMETHODCALLTYPE UnlockRect(UINT Level);

  HRESULT STDMETHODCALLTYPE AddDirtyRect(CONST RECT* pDirtyRect);

  DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) final {
    Logger::warn("D3D9Texture2D::SetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLOD() final {
    Logger::warn("D3D9Texture2D::GetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLevelCount() final {
    Logger::info("D3D9Texture2D::GetLevelCount:");
    return m_d3d8->GetLevelCount();
  }

  HRESULT STDMETHODCALLTYPE SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) final {
    Logger::warn("D3D9Texture2D::SetAutoGenFilterType: Stub!");
    return D3D_OK;
  }

  D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE GetAutoGenFilterType() final {
    Logger::warn("D3D9Texture2D::GetAutoGenFilterType: Stub!");
    return D3DTEXTUREFILTERTYPE(0);
  }

  void STDMETHODCALLTYPE GenerateMipSubLevels() final {
    Logger::warn("D3D9Texture2D::GenerateMipSubLevels: Stub!");
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9Texture2D::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  void STDMETHODCALLTYPE PreLoad() {
    Logger::warn("D3D9Texture2D::PreLoad: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9Texture2D::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9Texture2D::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9Texture2D::FreePrivateData: Stub!");
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    Logger::warn("D3D9Texture2D::SetPriority: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    Logger::warn("D3D9Texture2D::GetPriority: Stub!");
    return 0;
  }

  d3d8::IDirect3DTexture8* GetD3D8Texture() {
    return m_d3d8;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  d3d8::IDirect3DTexture8* m_d3d8 = nullptr;

};

class D3D9TextureCube : public IDirect3DCubeTexture9 {

public:

  D3D9TextureCube(IDirect3DDevice9* device, d3d8::IDirect3DCubeTexture8* d3d8Texture);

  ~D3D9TextureCube();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

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

  DWORD STDMETHODCALLTYPE SetLOD(DWORD LODNew) final {
    Logger::warn("D3D9TextureCube::SetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLOD() final {
    Logger::warn("D3D9TextureCube::GetLOD: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetLevelCount() final {
    Logger::info("D3D9TextureCube::GetLevelCount:");
    return m_d3d8->GetLevelCount();
  }

  HRESULT STDMETHODCALLTYPE SetAutoGenFilterType(D3DTEXTUREFILTERTYPE FilterType) final {
    Logger::warn("D3D9TextureCube::SetAutoGenFilterType: Stub!");
    return D3D_OK;
  }

  D3DTEXTUREFILTERTYPE STDMETHODCALLTYPE GetAutoGenFilterType() final {
    Logger::warn("D3D9TextureCube::GetAutoGenFilterType: Stub!");
    return D3DTEXTUREFILTERTYPE(0);
  }

  void STDMETHODCALLTYPE GenerateMipSubLevels() final {
    Logger::warn("D3D9TextureCube::GenerateMipSubLevels: Stub!");
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9TextureCube::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  void STDMETHODCALLTYPE PreLoad() {
    Logger::warn("D3D9TextureCube::PreLoad: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9TextureCube::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9TextureCube::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9TextureCube::FreePrivateData: Stub!");
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    Logger::warn("D3D9TextureCube::SetPriority: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    Logger::warn("D3D9TextureCube::GetPriority: Stub!");
    return 0;
  }

  d3d8::IDirect3DCubeTexture8* GetD3D8Texture() {
    return m_d3d8;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  d3d8::IDirect3DCubeTexture8* m_d3d8 = nullptr;

};