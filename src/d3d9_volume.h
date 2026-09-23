#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_resource.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9Volume final : public D3D9Resource<IDirect3DVolume9> {

public:

  D3D9Volume(
      IDirect3DDevice9* device,
      ComObject<d3d8::IDirect3DVolume8>&& d3d8Volume,
      IUnknown* container);

  ~D3D9Volume();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDesc(D3DVOLUME_DESC *pDesc) final;

  HRESULT STDMETHODCALLTYPE LockBox(D3DLOCKED_BOX* pLockedBox, CONST D3DBOX* pBox, DWORD Flags) final;

  HRESULT STDMETHODCALLTYPE UnlockBox() final;

  HRESULT STDMETHODCALLTYPE GetContainer(REFIID riid, void** ppContainer) final;

private:

  ComObject<d3d8::IDirect3DVolume8> m_d3d8;

  IUnknown*                         m_container = nullptr;

};
