#include "d3d9_shader.h"

D3D9VertexShader::D3D9VertexShader(IDirect3DDevice9* device, DWORD handle)
: m_device ( device )
, m_handle ( handle ) {
}

D3D9VertexShader::~D3D9VertexShader() {
}

D3D9PixelShader::D3D9PixelShader(IDirect3DDevice9* device, DWORD handle)
: m_device ( device )
, m_handle ( handle ) {
}

D3D9PixelShader::~D3D9PixelShader() {
}
