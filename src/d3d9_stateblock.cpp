#include "d3d9_stateblock.h"

D3D9StateBlock::D3D9StateBlock(IDirect3DDevice9* device, DWORD handle)
: m_device ( device )
, m_handle ( handle ) {
}

D3D9StateBlock::~D3D9StateBlock() {
  if (m_device != nullptr) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeleteStateBlock(m_handle);
  }
}

HRESULT STDMETHODCALLTYPE D3D9StateBlock::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
   || riid == __uuidof(IDirect3DStateBlock9)) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9StateBlock::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9StateBlock::Capture() {
  Logger::info("D3D9StateBlock::Capture:");

  D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);

  d3d9Device->GetD3D8Device()->CaptureStateBlock(m_handle);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9StateBlock::Apply() {
  Logger::info("D3D9StateBlock::Apply:");

  D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);

  d3d9Device->GetD3D8Device()->ApplyStateBlock(m_handle);

  return D3D_OK;
}