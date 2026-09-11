#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9VertexDecl final : public IDirect3DVertexDeclaration9 {

public:

  D3D9VertexDecl();

  ~D3D9VertexDecl();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(
          REFIID  riid,
          void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDeclaration(
          D3DVERTEXELEMENT9* pElement,
          UINT*              pNumElements);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::warn("D3D9VertexDecl::GetDevice: Stub!");
    return D3D_OK;
  }

};