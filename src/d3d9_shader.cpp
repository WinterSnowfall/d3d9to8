#include "d3d9_shader.h"

#include "d3d9_device.h"

D3D9VertexShader::D3D9VertexShader(IDirect3DDevice9* device, DWORD handle)
: m_device ( device )
, m_handle ( handle ) {
}

D3D9VertexShader::~D3D9VertexShader() {
  if (m_device != nullptr) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeleteVertexShader(m_handle);
  }
}

D3D9PixelShader::D3D9PixelShader(IDirect3DDevice9* device, DWORD handle)
: m_device ( device )
, m_handle ( handle ) {
}

D3D9PixelShader::~D3D9PixelShader() {
  if (m_device != nullptr) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeletePixelShader(m_handle);
  }
}
