#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_resource.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9IndexBuffer : public D3D9Resource<IDirect3DIndexBuffer9> {

public:

  D3D9IndexBuffer(IDirect3DDevice9* device, ComObject<d3d8::IDirect3DIndexBuffer8>&& d3d8IndexBuffer);

  ~D3D9IndexBuffer();

  HRESULT STDMETHODCALLTYPE QueryInterface(
    REFIID  riid,
    void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetDesc(D3DINDEXBUFFER_DESC* pDesc);

  HRESULT STDMETHODCALLTYPE Lock(
          UINT   OffsetToLock,
          UINT   SizeToLock,
          void** ppbData,
          DWORD  Flags);

  HRESULT STDMETHODCALLTYPE Unlock();

  d3d8::IDirect3DIndexBuffer8* GetD3D8IndexBuffer() const {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DIndexBuffer8> m_d3d8;

};

class D3D9VertexBuffer : public D3D9Resource<IDirect3DVertexBuffer9> {

public:

  D3D9VertexBuffer(IDirect3DDevice9* device, ComObject<d3d8::IDirect3DVertexBuffer8>&& d3d8VertexBuffer);

  ~D3D9VertexBuffer();

  HRESULT STDMETHODCALLTYPE QueryInterface(
    REFIID  riid,
    void** ppvObject);

  D3DRESOURCETYPE STDMETHODCALLTYPE GetType();

  HRESULT STDMETHODCALLTYPE GetDesc(D3DVERTEXBUFFER_DESC* pDesc);

  HRESULT STDMETHODCALLTYPE Lock(
          UINT   OffsetToLock,
          UINT   SizeToLock,
          void** ppbData,
          DWORD  Flags);

  HRESULT STDMETHODCALLTYPE Unlock();

  d3d8::IDirect3DVertexBuffer8* GetD3D8VertexBuffer() const {
    return m_d3d8.ptr();
  }

private:

  ComObject<d3d8::IDirect3DVertexBuffer8> m_d3d8;

};