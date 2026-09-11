#include "d3d9_shader_validator.h"

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::QueryInterface(REFIID riid, void** ppvObject) {
  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = this->IncrementRef();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Begin(
    D3D9ShaderValidatorCallback pCallback,
    void*                       pUserData,
    DWORD                       Unknown) {
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Instruction(
    const char*  pFile,
          UINT   Line,
    const DWORD* pdwInst,
          DWORD  cdw) {
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::End() {
  return D3D_OK;
}

