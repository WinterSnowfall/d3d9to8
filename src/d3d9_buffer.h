#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9IndexBuffer : public IDirect3DIndexBuffer9 {

public:

  D3D9IndexBuffer(d3d8::IDirect3DIndexBuffer8* d3d8IndexBuffer);

  ~D3D9IndexBuffer();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

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

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::warn("D3D9IndexBuffer::GetDevice: Stub!");
    return D3D_OK;
  }

  void STDMETHODCALLTYPE PreLoad() {
    Logger::warn("D3D9IndexBuffer::PreLoad: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9IndexBuffer::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9IndexBuffer::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9IndexBuffer::FreePrivateData: Stub!");
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    Logger::warn("D3D9IndexBuffer::SetPriority: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    Logger::warn("D3D9IndexBuffer::GetPriority: Stub!");
    return 0;
  }

  d3d8::IDirect3DIndexBuffer8* GetD3D8IndexBuffer() {
    return m_d3d8;
  }

private:

  d3d8::IDirect3DIndexBuffer8* m_d3d8 = nullptr;

};

class D3D9VertexBuffer : public IDirect3DVertexBuffer9 {

public:

  D3D9VertexBuffer(d3d8::IDirect3DVertexBuffer8* d3d8VertexBuffer);

  ~D3D9VertexBuffer();

  ULONG STDMETHODCALLTYPE AddRef() {
    return 1;
  }

  ULONG STDMETHODCALLTYPE Release() {
    return 0;
  }

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

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::warn("D3D9VertexBuffer::GetDevice: Stub!");
    return D3D_OK;
  }

  void STDMETHODCALLTYPE PreLoad() {
    Logger::warn("D3D9VertexBuffer::PreLoad: Stub!");
  }

  HRESULT STDMETHODCALLTYPE SetPrivateData(
          REFGUID     refguid,
    const void*       pData,
          DWORD       SizeOfData,
          DWORD       Flags) final {
    Logger::warn("D3D9VertexBuffer::SetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetPrivateData(
          REFGUID     refguid,
          void*       pData,
          DWORD*      pSizeOfData) final {
    Logger::warn("D3D9VertexBuffer::GetPrivateData: Stub!");
    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE FreePrivateData(REFGUID refguid) final {
    Logger::warn("D3D9VertexBuffer::FreePrivateData: Stub!");
    return D3D_OK;
  }

  DWORD STDMETHODCALLTYPE SetPriority(DWORD PriorityNew) {
    Logger::warn("D3D9VertexBuffer::SetPriority: Stub!");
    return 0;
  }

  DWORD STDMETHODCALLTYPE GetPriority() {
    Logger::warn("D3D9VertexBuffer::GetPriority: Stub!");
    return 0;
  }

  d3d8::IDirect3DVertexBuffer8* GetD3D8VertexBuffer() {
    return m_d3d8;
  }

private:

  d3d8::IDirect3DVertexBuffer8* m_d3d8 = nullptr;

};