#include "d3d9_vertex_declaration.h"

#include "d3d9_device.h"

D3D9VertexDecl::D3D9VertexDecl(IDirect3DDevice9* device, DWORD handle, const D3DVERTEXELEMENT9* vertexElements)
  : m_device ( device )
  , m_handle ( handle ) {
  const D3DVERTEXELEMENT9* ptr = vertexElements;

  if (likely(ptr != nullptr)) {
    // Copy vertex elements until we hit D3DDECL_END()
    while(ptr->Type != D3DDECLTYPE_UNUSED) {
      m_vertexElements.push_back(*ptr);
      ptr++;
    }
    m_vertexElements.push_back(D3DDECL_END());
  }
}

D3D9VertexDecl::~D3D9VertexDecl() {
  ClearVSHandle();
}

HRESULT STDMETHODCALLTYPE D3D9VertexDecl::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DVertexDeclaration9))) {
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
  if (unlikely(pNumElements == nullptr))
    return D3DERR_INVALIDCALL;

  *pNumElements = static_cast<UINT>(m_vertexElements.size());

  if (pElement == nullptr)
    return D3D_OK;

  memcpy(pElement, m_vertexElements.data(), m_vertexElements.size() * sizeof(D3DVERTEXELEMENT9));

  return D3D_OK;
}

void D3D9VertexDecl::ClearVSHandle() {
  if (likely(m_handle && m_device != nullptr)) {
    D3D9Device* d3d9Device = reinterpret_cast<D3D9Device*>(m_device);
    d3d9Device->GetD3D8Device()->DeleteVertexShader(m_handle);
  }
}
