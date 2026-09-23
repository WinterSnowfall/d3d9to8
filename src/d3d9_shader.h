#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include "d3d9_shader_util.h"

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

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

  void SetVSHandle(DWORD handle) {
    // Release any previously held shader handle
    ClearVSHandle();
    m_handle = handle;
  }

  DWORD GetVSHandle() const {
    return m_handle;
  }

  std::vector<DWORD>* GetFunction9() {
    return &m_function;
  }

  std::vector<DWORD>* GetFunction8() {
    return &m_function8;
  }

private:

  void ClearVSHandle();

  IDirect3DDevice9*  m_device = nullptr;

  DWORD              m_handle = 0u;

  std::vector<DWORD> m_function;
  std::vector<DWORD> m_function8;

};

class D3D9PixelShader final : public ComObjectClamp<IDirect3DPixelShader9> {

public:

  D3D9PixelShader(IDirect3DDevice9* device, DWORD handle, const DWORD* pFunction);

  ~D3D9PixelShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    if (unlikely(ppDevice == nullptr))
      return D3DERR_INVALIDCALL;

    *ppDevice = ref(m_device);

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData);

  DWORD GetPSHandle() const {
    return m_handle;
  }

private:

  IDirect3DDevice9*  m_device = nullptr;

  DWORD              m_handle = 0u;

  std::vector<DWORD> m_function;

};
