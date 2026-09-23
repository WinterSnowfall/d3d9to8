#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include <chrono>

using Logger = ThreadSafeLogger;

union D3D9_QUERY_DATA {
  D3DDEVINFO_VCACHE         VCache;
  DWORD                     Occlusion;
  UINT64                    Timestamp;
  BOOL                      TimestampDisjoint;
  UINT64                    TimestampFreq;
  D3DDEVINFO_D3DVERTEXSTATS VertexStats;
};

class D3D9Query : public ComObjectClamp<IDirect3DQuery9> {

public:

  D3D9Query(IDirect3DDevice9* device, D3DQUERYTYPE queryType);

  ~D3D9Query();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  D3DQUERYTYPE STDMETHODCALLTYPE GetType() final;

  DWORD STDMETHODCALLTYPE GetDataSize() final;

  HRESULT STDMETHODCALLTYPE Issue(DWORD dwIssueFlags) final;

  HRESULT STDMETHODCALLTYPE GetData(void* pData, DWORD dwSize, DWORD dwGetDataFlags) final;

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (unlikely(ppDevice == nullptr))
      return D3DERR_INVALIDCALL;

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  D3DQUERYTYPE      m_queryType;

  D3D9_QUERY_DATA   m_queryData;

};
