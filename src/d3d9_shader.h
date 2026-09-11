#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

class D3D9VertexShader final : public ComObjectClamp<IDirect3DVertexShader9> {

public:

  D3D9VertexShader(IDirect3DDevice9* device, DWORD handle);

  ~D3D9VertexShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
    if (ppvObject == nullptr)
      return E_POINTER;

    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown)
      || riid == __uuidof(IDirect3DVertexShader9)) {
      //this->AddRef();
      *ppvObject = this;
      return S_OK;
    }

    Logger::warn("D3D9Shader::QueryInterface: Unknown interface query");
    //Logger::warn(str::format(riid));
    return E_NOINTERFACE;
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9VertexShader::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData) {
    Logger::warn("D3D9VertexShader::GetFunction: Stub!");
    return D3D_OK;
  }

  DWORD GetD3D8VSHandle() {
    return m_handle;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  DWORD m_handle = 0;

};

class D3D9PixelShader final : public ComObjectClamp<IDirect3DPixelShader9> {

public:

  D3D9PixelShader(IDirect3DDevice9* device, DWORD handle);

  ~D3D9PixelShader();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) {
    if (ppvObject == nullptr)
      return E_POINTER;

    *ppvObject = nullptr;

    if (riid == __uuidof(IUnknown)
      || riid == __uuidof(IDirect3DPixelShader9)) {
      //this->AddRef();
      *ppvObject = this;
      return S_OK;
    }

    Logger::warn("D3D9Shader::QueryInterface: Unknown interface query");
    //Logger::warn(str::format(riid));
    return E_NOINTERFACE;
  }

  HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice) {
    Logger::info("D3D9PixelShader::GetDevice:");

    if (ppDevice == nullptr)
      return D3DERR_INVALIDCALL;

    *ppDevice = m_device;

    return D3D_OK;
  }

  HRESULT STDMETHODCALLTYPE GetFunction(void* pOut, UINT* pSizeOfData) {
    Logger::warn("D3D9PixelShader::GetFunction: Stub!");
    return D3D_OK;
  }

  DWORD GetD3D8PSHandle() {
    return m_handle;
  }

private:

  IDirect3DDevice9* m_device = nullptr;

  DWORD m_handle = 0;

};
