#pragma once

#include "d3d9_include.h"
#include "d3d9_device_child.h"
#include "d3d9_logger.h"

#include "d3d9_shader_util.h"

#include <vector>

using Logger = ThreadSafeLogger;

class D3D9VertexShader final : public D3D9DeviceChild<IDirect3DVertexShader9> {

public:

  D3D9VertexShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction);

  ~D3D9VertexShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData);

  void SetVSHandle(DWORD handle) {
    // Release any previously held shader handle
    ClearVSHandle();
    m_handle = handle;
  }

  DWORD GetVSHandle() const {
    return m_handle;
  }

  std::vector<DWORD>* GetFunction9() {
    return &m_function;
  }

  std::vector<DWORD>* GetFunction8() {
    return &m_function8;
  }

  void SetDeclarationOrigin(IDirect3DVertexDeclaration9* vertexDeclaration) {
    m_declarationOrigin = vertexDeclaration;
  }

  bool NeedsFunctionUpdate(IDirect3DVertexDeclaration9* vertexDeclaration) const {
    return m_function8.empty() || m_declarationOrigin != vertexDeclaration;
  }

private:

  void ClearVSHandle();

  DWORD                        m_handle = 0u;

  // Stores a pointer to the declaration which was used to generate the function
  IDirect3DVertexDeclaration9* m_declarationOrigin = nullptr;

  std::vector<DWORD>           m_function;
  std::vector<DWORD>           m_function8;

};

class D3D9PixelShader final : public D3D9DeviceChild<IDirect3DPixelShader9> {

public:

  D3D9PixelShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction);

  ~D3D9PixelShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData);

  DWORD GetPSHandle() const {
    return m_handle;
  }

  std::vector<DWORD>* GetFunction9() {
    return &m_function;
  }

private:

  DWORD                       m_handle = 0u;

  std::vector<DWORD>          m_function;

};
