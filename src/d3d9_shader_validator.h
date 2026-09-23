#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

using Logger = ThreadSafeLogger;

using D3D9ShaderValidatorCallback = HRESULT(STDMETHODCALLTYPE *)(
  const char*                      pFile,
        UINT                       Line,
        DWORD                      Unknown,
        UINT                       MessageID,
  const char*                      pMessage,
        void*                      pUserData);

class IDirect3DShaderValidator9 : public IUnknown {

public:

  virtual HRESULT STDMETHODCALLTYPE Begin(
          D3D9ShaderValidatorCallback pCallback,
          void*                       pUserParam,
          DWORD                       Unknown) = 0;

  virtual HRESULT STDMETHODCALLTYPE Instruction(
    const char*  pFile,
          UINT   Line,
    const DWORD* pdwInst,
          DWORD  cdw) = 0;

  virtual HRESULT STDMETHODCALLTYPE End() = 0;

};

class D3D9ShaderValidator final : public ComObjectClamp<IDirect3DShaderValidator9> {

public:

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE Begin(
      D3D9ShaderValidatorCallback pCallback,
      void*                       pUserData,
      DWORD                       Unknown);

  HRESULT STDMETHODCALLTYPE Instruction(
      const char*  pFile,
            UINT   Line,
      const DWORD* pdwInst,
            DWORD  cdw);

  HRESULT STDMETHODCALLTYPE End();
};
