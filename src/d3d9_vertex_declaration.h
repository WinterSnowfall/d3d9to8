#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include <vector>

using Logger = ThreadSafeLogger;

class D3D9VertexDecl final : public ComObjectClamp<IDirect3DVertexDeclaration9> {

public:

  D3D9VertexDecl(IDirect3DDevice9* device, const D3DVERTEXELEMENT9* vertexElements);

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

private:

  IDirect3DDevice9*              m_device = nullptr;

  std::vector<D3DVERTEXELEMENT9> m_vertexElements;

};