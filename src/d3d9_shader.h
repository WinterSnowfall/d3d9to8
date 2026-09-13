#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include <vector>

using Logger = ThreadSafeLogger;

class D3D9VertexShader final : public ComObjectClamp<IDirect3DVertexShader9> {

public:

  D3D9VertexShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction);

  ~D3D9VertexShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  DWORD GetD3D8VSHandle() const {
    return m_handle;
  }

  // We may need to swap the linked D3D8 shader due to
  // an update on either the declaration or function
  void UpdateD3D8VSHandle(DWORD handle) {
    m_handle = handle;
  }

private:

  IDirect3DDevice9*  m_device = nullptr;

  DWORD              m_handle = 0;

  std::vector<DWORD> m_function;

};

class D3D9PixelShader final : public ComObjectClamp<IDirect3DPixelShader9> {

public:

  D3D9PixelShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction);

  ~D3D9PixelShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData);

  DWORD GetD3D8PSHandle() const {
    return m_handle;
  }

private:

  IDirect3DDevice9*  m_device = nullptr;

  DWORD              m_handle = 0;

  std::vector<DWORD> m_function;

};
