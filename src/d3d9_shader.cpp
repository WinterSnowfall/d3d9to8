#include "d3d9_shader.h"

#include "d3d9_device.h"

D3D9VertexShader::D3D9VertexShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction)
  : m_device ( device )
  , m_handle ( handle ) {
  const DWORD* ptr = pFunction;

  if (likely(ptr != nullptr)) {
    while (*ptr != D3DVS_END()) {
      m_function.push_back(*ptr);
      ptr++;
    }
    m_function.push_back(D3DVS_END());
  }
}

D3D9VertexShader::~D3D9VertexShader() {
  ClearVSHandle();
}

HRESULT STDMETHODCALLTYPE D3D9VertexShader::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DVertexShader9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Shader::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9VertexShader::GetFunction(void* pOut, UINT* pSizeOfData) {
  if (unlikely(pSizeOfData == nullptr))
    return D3DERR_INVALIDCALL;

  *pSizeOfData = m_function.size() * sizeof(DWORD);

  if (pOut == nullptr)
    return D3D_OK;

  memcpy(pOut, m_function.data(), m_function.size() * sizeof(DWORD));

  return D3D_OK;
}

void D3D9VertexShader::ClearVSHandle() {
  if (likely(m_handle && m_device != nullptr)) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeleteVertexShader(m_handle);
  }
}

D3D9PixelShader::D3D9PixelShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction)
  : m_device ( device )
  , m_handle ( handle ) {
  const DWORD* ptr = pFunction;

  if (likely(ptr != nullptr)) {
    while (*ptr != D3DPS_END()) {
      m_function.push_back(*ptr);
      ptr++;
    }
    m_function.push_back(D3DPS_END());
  }
}

D3D9PixelShader::~D3D9PixelShader() {
  if (likely(m_handle && m_device != nullptr)) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeletePixelShader(m_handle);
  }
}

HRESULT STDMETHODCALLTYPE D3D9PixelShader::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DPixelShader9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Shader::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9PixelShader::GetFunction(void* pOut, UINT* pSizeOfData) {
  if (unlikely(pSizeOfData == nullptr))
    return D3DERR_INVALIDCALL;

  *pSizeOfData = m_function.size() * sizeof(DWORD);

  if (pOut == nullptr)
    return D3D_OK;

  memcpy(pOut, m_function.data(), m_function.size() * sizeof(DWORD));

  return D3D_OK;
}
