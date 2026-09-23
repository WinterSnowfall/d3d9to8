#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include "d3d9_shader_util.h"

#include <vector>

using Logger = ThreadSafeLogger;

class D3D9VertexDecl final : public ComObjectClamp<IDirect3DVertexDeclaration9> {

public:

  D3D9VertexDecl(IDirect3DDevice9* device, DWORD handle, const D3DVERTEXELEMENT9* vertexElements);

  ~D3D9VertexDecl();

  HRESULT STDMETHODCALLTYPE QueryInterface(
          REFIID  riid,
          void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDeclaration(
          D3DVERTEXELEMENT9* pElement,
          UINT*              pNumElements);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (unlikely(ppDevice == nullptr))
      return D3DERR_INVALIDCALL;

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

  void SetVSHandle(DWORD handle) {
    // Release any previously held shader handle
    ClearVSHandle();
    m_handle = handle;
  }

  DWORD GetVSHandle() const {
    return m_handle;
  }

  std::vector<D3DVERTEXELEMENT9>* GetDeclaration9() {
    return &m_vertexElements;
  }

  std::vector<DWORD>* GetDeclaration8() {
    return &m_vertexElements8;
  }

  void SetFunctionOrigin(IDirect3DVertexShader9* vertexShader) {
    m_functionOrigin = vertexShader;
  }

  bool NeedsDefinitionUpdate(IDirect3DVertexShader9* vertexShader) const {
    return m_vertexElements8.empty() || m_functionOrigin != vertexShader;
  }

private:

  void ClearVSHandle();

  IDirect3DDevice9*              m_device = nullptr;

  DWORD                          m_handle = 0u;

  // Stores a pointer to the VS which was used to generate the definition
  IDirect3DVertexShader9*        m_functionOrigin = nullptr;

  std::vector<D3DVERTEXELEMENT9> m_vertexElements;
  std::vector<DWORD>             m_vertexElements8;

};
