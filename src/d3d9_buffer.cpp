#include "d3d9_buffer.h"

D3D9IndexBuffer::D3D9IndexBuffer(d3d8::IDirect3DIndexBuffer8* d3d8IndexBuffer)
: m_d3d8 ( d3d8IndexBuffer ) {
}

D3D9IndexBuffer::~D3D9IndexBuffer() {
}

HRESULT STDMETHODCALLTYPE D3D9IndexBuffer::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DResource9)
    || riid == __uuidof(IDirect3DIndexBuffer9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9IndexBuffer::QueryInterface: Unknown interface query");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9IndexBuffer::GetType() {
  return D3DRTYPE_INDEXBUFFER;
}

HRESULT STDMETHODCALLTYPE D3D9IndexBuffer::GetDesc(
        D3DINDEXBUFFER_DESC* pDesc) {
  Logger::info("D3D9IndexBuffer::GetDesc:");
  return m_d3d8->GetDesc(reinterpret_cast<d3d8::D3DINDEXBUFFER_DESC*>(pDesc));
}

HRESULT STDMETHODCALLTYPE D3D9IndexBuffer::Lock(
        UINT   OffsetToLock,
        UINT   SizeToLock,
        void** ppbData,
        DWORD  Flags) {
  Logger::info("D3D9IndexBuffer::Lock:");
  return m_d3d8->Lock(OffsetToLock, SizeToLock, reinterpret_cast<BYTE**>(ppbData), Flags);
}

HRESULT STDMETHODCALLTYPE D3D9IndexBuffer::Unlock() {
  Logger::info("D3D9IndexBuffer::Unlock:");
  return m_d3d8->Unlock();
}

D3D9VertexBuffer::D3D9VertexBuffer(d3d8::IDirect3DVertexBuffer8* d3d8VertexBuffer)
: m_d3d8 ( d3d8VertexBuffer ) {
}

D3D9VertexBuffer::~D3D9VertexBuffer() {
}

HRESULT STDMETHODCALLTYPE D3D9VertexBuffer::QueryInterface(
        REFIID  riid,
        void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DResource9)
    || riid == __uuidof(IDirect3DVertexBuffer9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9VertexBuffer::QueryInterface: Unknown interface query");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

D3DRESOURCETYPE STDMETHODCALLTYPE D3D9VertexBuffer::GetType() {
  return D3DRTYPE_VERTEXBUFFER;
}

HRESULT STDMETHODCALLTYPE D3D9VertexBuffer::GetDesc(
        D3DVERTEXBUFFER_DESC* pDesc) {
  Logger::info("D3D9VertexBuffer::GetDesc:");
  return m_d3d8->GetDesc(reinterpret_cast<d3d8::D3DVERTEXBUFFER_DESC*>(pDesc));
}

HRESULT STDMETHODCALLTYPE D3D9VertexBuffer::Lock(
        UINT   OffsetToLock,
        UINT   SizeToLock,
        void** ppbData,
        DWORD  Flags) {
  Logger::info("D3D9VertexBuffer::Lock:");
  return m_d3d8->Lock(OffsetToLock, SizeToLock, reinterpret_cast<BYTE**>(ppbData), Flags);
}

HRESULT STDMETHODCALLTYPE D3D9VertexBuffer::Unlock() {
  Logger::info("D3D9VertexBuffer::Unlock:");
  return m_d3d8->Unlock();
}

