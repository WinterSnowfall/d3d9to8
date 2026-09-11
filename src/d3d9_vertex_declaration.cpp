#include "d3d9_vertex_declaration.h"

D3D9VertexDecl::D3D9VertexDecl() {
}

D3D9VertexDecl::~D3D9VertexDecl() {
}

HRESULT STDMETHODCALLTYPE D3D9VertexDecl::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
   || riid == __uuidof(IDirect3DVertexDeclaration9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9VertexDecl::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9VertexDecl::GetDeclaration(
        D3DVERTEXELEMENT9* pElement,
        UINT*              pNumElements) {
  Logger::warn("D3D9VertexDecl::GetDeclaration: Stub!");
  return D3D_OK;
}