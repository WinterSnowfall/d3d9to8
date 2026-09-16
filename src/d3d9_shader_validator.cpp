#include "d3d9_shader_validator.h"

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  *ppvObject = ref(this);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Begin(
    D3D9ShaderValidatorCallback pCallback,
    void*                       pUserData,
    DWORD                       Unknown) {
  Logger::debug("D3D9ShaderValidator::Begin: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::Instruction(
    const char*  pFile,
          UINT   Line,
    const DWORD* pdwInst,
          DWORD  cdw) {
  Logger::debug("D3D9ShaderValidator::Instruction: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9ShaderValidator::End() {
  Logger::debug("D3D9ShaderValidator::End: Stub!");
  return D3D_OK;
}

