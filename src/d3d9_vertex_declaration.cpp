#include "d3d9_vertex_declaration.h"

D3D9VertexDecl::D3D9VertexDecl(IDirect3DDevice9* device, DWORD handle, const D3DVERTEXELEMENT9* vertexElements)
  : m_device ( device )
  , m_handle ( handle ) {
  const D3DVERTEXELEMENT9* ptr = vertexElements;

  if (likely(ptr != nullptr)) {
    BYTE currentStream8 = 0xFF;
    // Process until we hit D3DDECL_END()
    while(ptr->Type != D3DDECLTYPE_UNUSED) {
      Logger::debug("D3D9VertexDecl:: Element:    " + std::to_string(m_vertexElements.size()));
      Logger::debug("D3D9VertexDecl:: Stream:     " + std::to_string(ptr->Stream));
      Logger::debug("D3D9VertexDecl:: Offset:     " + std::to_string(ptr->Offset)); // Ignored in D3D8
      Logger::debug("D3D9VertexDecl:: Type:       " + std::to_string(ptr->Type));
      Logger::debug("D3D9VertexDecl:: Method:     " + std::to_string(ptr->Method)); // Ignored in D3D8
      Logger::debug("D3D9VertexDecl:: Usage:      " + std::to_string(ptr->Usage));
      Logger::debug("D3D9VertexDecl:: UsageIndex: " + std::to_string(ptr->UsageIndex));
      m_vertexElements.push_back(*ptr);

      // Translate the D3D9 vertex declaration to a D3D8 vertex declaration
      Logger::debug("D3D9VertexDecl:: -----------------");
      if (ptr->Stream != currentStream8) {
        // Emit a D3DVSD_STREAM token if the current stream changes
        currentStream8 = ptr->Stream;
        Logger::debug("D3D9VertexDecl:: D3D8 Stream:   " + std::to_string(currentStream8));
        m_vertexElements8.push_back(D3DVSD_STREAM_D3D8(currentStream8));
      }
      BYTE Reg8 = ConvertD3D9UsageToD3D8Register(static_cast<D3DDECLUSAGE>(ptr->Usage), ptr->UsageIndex);
      Logger::debug("D3D9VertexDecl:: D3D8 Register: " + std::to_string(Reg8));
      d3d8::D3DVSDT_TYPE Type8 = d3d8::D3DVSDT_TYPE(ptr->Type);
      Logger::debug("D3D9VertexDecl:: D3D8 Type:     " + std::to_string(Type8));
      m_vertexElements8.push_back(D3DVSD_REG_D3D8(Reg8, Type8));
      Logger::debug("D3D9VertexDecl:: ----------------");

      ptr++;
    }

    m_vertexElements.push_back(D3DDECL_END());
    m_vertexElements8.push_back(D3DVSD_END());
  }
}

D3D9VertexDecl::~D3D9VertexDecl() {
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

  *pNumElements = UINT(m_vertexElements.size());

  if (pElement == nullptr)
    return D3D_OK;

  memcpy(pElement, m_vertexElements.data(), m_vertexElements.size() * sizeof(D3DVERTEXELEMENT9));

  return D3D_OK;
}
