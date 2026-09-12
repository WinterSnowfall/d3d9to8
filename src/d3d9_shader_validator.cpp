#include "d3d9_shader_validator.h"

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::QueryInterface(REFIID riid, void** ppvObject) {
  Logger::info("D3D9ShaderValidator::QueryInterface:");

  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = ref(this);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Begin(
    D3D9ShaderValidatorCallback pCallback,
    void*                       pUserData,
    DWORD                       Unknown) {
  Logger::warn("D3D9ShaderValidator::Begin: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Instruction(
    const char*  pFile,
          UINT   Line,
    const DWORD* pdwInst,
          DWORD  cdw) {
  Logger::warn("D3D9ShaderValidator::Instruction: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::End() {
  Logger::warn("D3D9ShaderValidator::End: Stub!");
  return D3D_OK;
}

