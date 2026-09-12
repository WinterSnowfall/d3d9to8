#include "d3d9_vertex_declaration.h"

D3D9VertexDecl::D3D9VertexDecl(const D3DVERTEXELEMENT9* vertexElements) {
  const D3DVERTEXELEMENT9* ptr = vertexElements;

  if (ptr != nullptr) {
    // Process until we hit D3DDECL_END()
    while(ptr->Type != D3DDECLTYPE_UNUSED) {
      m_vertexElements.push_back(*ptr);
      Logger::debug("D3D9VertexDecl:: Element:    " + std::to_string(m_vertexElements.size()));
      Logger::debug("D3D9VertexDecl:: Stream:     " + std::to_string(ptr->Stream));
      Logger::debug("D3D9VertexDecl:: Offset:     " + std::to_string(ptr->Offset));
      Logger::debug("D3D9VertexDecl:: Type:       " + std::to_string(ptr->Type));
      Logger::debug("D3D9VertexDecl:: Method:     " + std::to_string(ptr->Method));
      Logger::debug("D3D9VertexDecl:: Usage:      " + std::to_string(ptr->Usage));
      Logger::debug("D3D9VertexDecl:: UsageIndex: " + std::to_string(ptr->UsageIndex));
      ptr++;
    }

    m_vertexElements.push_back(D3DDECL_END());
  }
}

D3D9VertexDecl::~D3D9VertexDecl() {
}

HRESULT STDMETHODCALLTYPE D3D9VertexDecl::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  Logger::info("D3D9VertexDecl::QueryInterface:");

  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
   || riid == __uuidof(IDirect3DVertexDeclaration9)) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9VertexDecl::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9VertexDecl::GetDeclaration(
        D3DVERTEXELEMENT9* pElement,
        UINT*              pNumElements) {
  Logger::info("D3D9VertexDecl::GetDeclaration:");

  if (pNumElements == nullptr)
    return D3DERR_INVALIDCALL;

  *pNumElements = UINT(m_vertexElements.size());

  if (pElement == nullptr)
    return D3D_OK;

  memcpy(pElement, m_vertexElements.data(), m_vertexElements.size() * sizeof(D3DVERTEXELEMENT9));

  return D3D_OK;
}