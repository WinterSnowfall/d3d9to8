#include "d3d9_query.h"

D3D9Query::D3D9Query(IDirect3DDevice9* device, D3DQUERYTYPE queryType)
  : m_device ( device )
  , m_queryType ( queryType ) {
}

D3D9Query::~D3D9Query() {
}

HRESULT STDMETHODCALLTYPE D3D9Query::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DQuery9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9Query::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

D3DQUERYTYPE STDMETHODCALLTYPE D3D9Query::GetType() {
  return m_queryType;
}

DWORD STDMETHODCALLTYPE D3D9Query::GetDataSize() {
  switch (m_queryType) {
    case D3DQUERYTYPE_VCACHE:               return sizeof(D3DDEVINFO_VCACHE);
    case D3DQUERYTYPE_RESOURCEMANAGER:      return sizeof(D3DDEVINFO_RESOURCEMANAGER);
    case D3DQUERYTYPE_VERTEXSTATS:          return sizeof(D3DDEVINFO_D3DVERTEXSTATS);
    case D3DQUERYTYPE_EVENT:                return sizeof(BOOL);
    case D3DQUERYTYPE_OCCLUSION:            return sizeof(DWORD);
    case D3DQUERYTYPE_TIMESTAMP:            return sizeof(UINT64);
    case D3DQUERYTYPE_TIMESTAMPDISJOINT:    return sizeof(BOOL);
    case D3DQUERYTYPE_TIMESTAMPFREQ:        return sizeof(UINT64);
    case D3DQUERYTYPE_PIPELINETIMINGS:      return sizeof(D3DDEVINFO_D3D9PIPELINETIMINGS);
    case D3DQUERYTYPE_INTERFACETIMINGS:     return sizeof(D3DDEVINFO_D3D9INTERFACETIMINGS);
    case D3DQUERYTYPE_VERTEXTIMINGS:        return sizeof(D3DDEVINFO_D3D9STAGETIMINGS);
    case D3DQUERYTYPE_PIXELTIMINGS:         return sizeof(D3DDEVINFO_D3D9PIPELINETIMINGS);
    case D3DQUERYTYPE_BANDWIDTHTIMINGS:     return sizeof(D3DDEVINFO_D3D9BANDWIDTHTIMINGS);
    case D3DQUERYTYPE_CACHEUTILIZATION:     return sizeof(D3DDEVINFO_D3D9CACHEUTILIZATION);
    default:                                return 0;
  }
}

HRESULT STDMETHODCALLTYPE D3D9Query::Issue(DWORD dwIssueFlags) {
  Logger::info("D3D9Query::Issue: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Query::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags) {
  Logger::info("D3D9Query::GetData: Stub!");

  if (pData != nullptr)
    memset(pData, 0, dwSize);

  return D3D_OK;
}