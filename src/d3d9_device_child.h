#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"

template <typename ObjectType>
class D3D9DeviceChild : public ComObjectClamp<ObjectType> {

public:

  D3D9DeviceChild(IDirect3DDevice9* device)
    : m_device ( device ) { }

  ULONG STDMETHODCALLTYPE AddRef() {
    uint32_t refCount = this->m_refCount++;

    if (unlikely(!refCount)) {
      this->AddRefPrivate();
      m_device->AddRef();
    }

    return refCount + 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    uint32_t oldRefCount, refCount;

    do {
      oldRefCount = this->m_refCount.load();
      if (unlikely(!oldRefCount))
        return 0;
      refCount = oldRefCount - 1;
    } while (!this->m_refCount.compare_exchange_weak(oldRefCount, refCount));

    if (unlikely(!refCount)) {
      IDirect3DDevice9* device = m_device;
      this->ReleasePrivate();
      device->Release();
    }

    return refCount;
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    ClearReturnPointer(ppDevice);

    if (unlikely(ppDevice == nullptr))
      return D3DERR_INVALIDCALL;

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

protected:

  IDirect3DDevice9* m_device = nullptr;

};

