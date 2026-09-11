#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9Volume final : public ComObjectClamp<IDirect3DVolume9> {

public:

  D3D9Volume(IDirect3DDevice9* device, d3d8::IDirect3DVolume8* d3d8Volume);

  ~D3D9Volume();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDesc(D3DVOLUME_DESC *pDesc) final;

  HRESULT STDMETHODCALLTYPE LockBox(D3DLOCKED_BOX* pLockedBox, CONST D3DBOX* pBox, DWORD Flags) final;

  HRESULT STDMETHODCALLTYPE UnlockBox() final;

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9Volume::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer) final {
    Logger::warn("D3D9Volume::GetContainer: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9Volume::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9Volume::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9Volume::FreePrivateData: Stub!");
    return D3D_OK;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  ComObject<d3d8::IDirect3DVolume8> m_d3d8;

};