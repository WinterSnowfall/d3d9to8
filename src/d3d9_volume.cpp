#include "d3d9_volume.h"

#include "d3d9_device.h"
#include "d3d9_texture.h"

D3D9Volume::D3D9Volume(IDirect3DDevice9* device, d3d8::IDirect3DVolume8* d3d8Volume)
  : D3D9Resource(device, reinterpret_cast<d3d8::IDirect3DResource8*>(d3d8Volume))
  , m_d3d8 ( d3d8Volume ) {
}

D3D9Volume::~D3D9Volume() { }

HRESULT STDMETHODCALLTYPE D3D9Volume::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
   || riid == __uuidof(IDirect3DResource9)
   || riid == __uuidof(IDirect3DVolume9)) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Volume::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9Volume::GetDesc(D3DVOLUME_DESC *pDesc) {
  return m_d3d8->GetDesc(reinterpret_cast<d3d8::D3DVOLUME_DESC*>(&pDesc));
}

HRESULT STDMETHODCALLTYPE D3D9Volume::LockBox(D3DLOCKED_BOX* pLockedBox, CONST D3DBOX* pBox, DWORD Flags) {
  return m_d3d8->LockBox(reinterpret_cast<d3d8::D3DLOCKED_BOX*>(pLockedBox),
                          reinterpret_cast<CONST d3d8::D3DBOX*>(pBox), Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Volume::UnlockBox() {
  return m_d3d8->UnlockBox();
}

HRESULT STDMETHODCALLTYPE D3D9Volume::GetContainer(REFIID riid, void** ppContainer) {
  Logger::warn("D3D9Volume::GetContainer: Stub!");
  return D3D_OK;
}

