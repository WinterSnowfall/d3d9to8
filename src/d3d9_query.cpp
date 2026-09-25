#include "d3d9_query.h"

D3D9Query::D3D9Query(IDirect3DDevice9* device, D3DQUERYTYPE queryType)
  : D3D9DeviceChild(device)
  , m_queryType ( queryType ) {
}

D3D9Query::~D3D9Query() {
}

HRESULT STDMETHODCALLTYPE D3D9Query::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DQuery9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  Logger::warn("D3D9Query::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
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
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Query::GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags) {
  if (unlikely(pData == nullptr && dwSize != 0))
    return D3DERR_INVALIDCALL;

  if (likely(pData != nullptr)) {
    switch (m_queryType) {
      case D3DQUERYTYPE_EVENT:
        *static_cast<BOOL*>(pData) = true;
        break;
      case D3DQUERYTYPE_VCACHE: {
        // Pretend to be an Nvidia GPU in all cases
        static constexpr D3DDEVINFO_VCACHE VCACHE_DATA = { MAKEFOURCC('C', 'A', 'C', 'H'), 1, 16, 7 };
        memcpy(pData, &VCACHE_DATA, dwSize);
        break;
      }
      case D3DQUERYTYPE_TIMESTAMP: {
        const uint64_t time = static_cast<uint64_t>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        memcpy(pData, &time, dwSize);
        break;
      }
      case D3DQUERYTYPE_TIMESTAMPDISJOINT:
        *static_cast<BOOL*>(pData) = true;
        break;
      case D3DQUERYTYPE_OCCLUSION:
        Logger::warn("D3D9Query::GetData: Returning 0 for D3DQUERYTYPE_OCCLUSION");
        *static_cast<DWORD*>(pData) = 0u;
        break;
      case D3DQUERYTYPE_TIMESTAMPFREQ:
        memset(pData, 0, dwSize);
        break;
      default:
        return S_FALSE;
    }
  }

  return S_OK;
}
