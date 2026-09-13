#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

template <typename ObjectType>
class D3D9Resource : public ComObjectClamp<ObjectType> {

public:

  D3D9Resource(IDirect3DDevice9* pDevice, d3d8::IDirect3DResource8* resource8)
    : m_device ( pDevice )
    , m_d3d8 ( resource8 ) {
  }

  ~D3D9Resource() {
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    return m_d3d8->SetPrivateData(refguid, pData, SizeOfData, Flags);
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    return m_d3d8->GetPrivateData(refguid, pData, pSizeOfData);
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    return m_d3d8->FreePrivateData(refguid);
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    return m_d3d8->SetPriority(PriorityNew);
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    return m_d3d8->GetPriority();
  }

  void STDMETHODCALLTYPE PreLoad() {
    m_d3d8->PreLoad();
  }

protected:

  IDirect3DDevice9* m_device = nullptr;

private:

  d3d8::IDirect3DResource8* m_d3d8 = nullptr;

};