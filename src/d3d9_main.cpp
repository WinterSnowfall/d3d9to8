#include "d3d9_include.h"

#include "d3d9_logger.h"
#include "d3d9_com_object.h"

#include "d3d9_interface.h"
#include "d3d9_shader_validator.h"

class D3DFE_PROCESSVERTICES;
using PSGPERRORID = UINT;

using Logger = ThreadSafeLogger;

HMODULE GetD3D8Module() {
  static HMODULE d3d8 = nullptr;

  if (unlikely(d3d8 == nullptr)) {
    // Determine the system directory path
    char loadPath[MAX_PATH] = { };
    const uint32_t returnLength = ::GetSystemDirectoryA(loadPath, MAX_PATH);
    if (unlikely(!returnLength))
      return nullptr;

    strcat(loadPath, "\\d3d8.dll");
    d3d8 = ::LoadLibraryA(loadPath);
    if (likely(d3d8 != nullptr))
      Logger::info("GetD3D8Module:: Loaded d3d8.dll from system path");
  }

  return d3d8;
}

extern "C" {

  DLLEXPORT IDirect3D9* __stdcall Direct3DCreate9(UINT nSDKVersion) {
    typedef d3d8::IDirect3D8* (__stdcall* Direct3DCreate8_t)(UINT nSDKVersion);
    static Direct3DCreate8_t Direct3DCreate8 = nullptr;

    if (unlikely(Direct3DCreate8 == nullptr)) {
      HMODULE d3d8 = GetD3D8Module();

      if (unlikely(d3d8 == nullptr)) {
        Logger::err("Direct3DCreate9:: Failed to load d3d8.dll!");
        return nullptr;
      }

      Direct3DCreate8 = reinterpret_cast<Direct3DCreate8_t>(GetProcAddress(d3d8, "Direct3DCreate8"));

      if (unlikely(Direct3DCreate8 == nullptr)) {
        Logger::err("Direct3DCreate9:: Failed GetProcAddress");
        return nullptr;
      }
    }

    ComObject<d3d8::IDirect3D8> d3d8Intf = Direct3DCreate8(D3D_SDK_VERSION_D3D8);
    if (unlikely(d3d8Intf == nullptr)) {
      Logger::err("Direct3DCreate9:: Failed to create a D3D8 interface!");
      return nullptr;
    }

    return ref(new D3D9Interface(std::move(d3d8Intf)));
  }

  DLLEXPORT HRESULT __stdcall Direct3DCreate9Ex(UINT nSDKVersion, IDirect3D9Ex** ppDirect3D9Ex) {
    Logger::err("Direct3DCreate9Ex:: Unsupported call!");
    return D3DERR_NOTAVAILABLE;
  }

  DLLEXPORT int __stdcall D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR wszName) {
    Logger::debug("D3DPERF_BeginEvent:: Stub!");
    return -1;
  }

  DLLEXPORT int __stdcall D3DPERF_EndEvent(void) {
    Logger::debug("D3DPERF_EndEvent:: Stub!");
    return -1;
  }

  DLLEXPORT void __stdcall D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR wszName) {
    Logger::debug("D3DPERF_SetMarker:: Stub!");
  }

  DLLEXPORT void __stdcall D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR wszName) {
    Logger::debug("D3DPERF_SetRegion:: Stub!");
  }

  DLLEXPORT BOOL __stdcall D3DPERF_QueryRepeatFrame(void) {
    Logger::debug("D3DPERF_QueryRepeatFrame:: Stub!");
    return false;
  }

  DLLEXPORT void __stdcall D3DPERF_SetOptions(DWORD dwOptions) {
    Logger::debug("D3DPERF_SetOptions:: Stub!");
  }

  DLLEXPORT DWORD __stdcall D3DPERF_GetStatus(void) {
    Logger::debug("D3DPERF_GetStatus:: Stub!");
    return 0u;
  }

  DLLEXPORT void __stdcall DebugSetMute(void) {
  }

  DLLEXPORT int __stdcall DebugSetLevel(void) {
    return 0;
  }

  DLLEXPORT void __stdcall PSGPError(D3DFE_PROCESSVERTICES* a, PSGPERRORID b, UINT c) {
    Logger::warn("PSGPError:: Stub!");
  }

  DLLEXPORT void __stdcall PSGPSampleTexture(D3DFE_PROCESSVERTICES* a, UINT b, float(*const c)[4], UINT d, float(*const e)[4]) {
    Logger::warn("PSGPSampleTexture:: Stub!");
  }

  DLLEXPORT void* __stdcall Direct3DShaderValidatorCreate9(void) {
    Logger::warn("Direct3DShaderValidatorCreate9:: Stub!");
    return ref(new D3D9ShaderValidator());
  }

  DLLEXPORT int __stdcall Direct3D9EnableMaximizedWindowedModeShim(UINT a) {
    Logger::warn("Direct3D9EnableMaximizedWindowedModeShim:: Stub!");
    return 0;
  }

  DLLEXPORT void __stdcall Direct3D9ForceHybridEnumeration(UINT uHybrid) {
    Logger::warn("Direct3D9ForceHybridEnumeration:: Stub!");
  }

  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
      case DLL_THREAD_ATTACH:
        break;
      case DLL_THREAD_DETACH:
        break;
      case DLL_PROCESS_ATTACH:
        Logger::initializeFile();
        Logger::info(">>>>>>> LOADING D3D9TO8 >>>>>>>");
        break;
      case DLL_PROCESS_DETACH: {
        // Calling FreeLibrary on DLL_PROCESS_DETACH is technically discouraged,
        // however apitrace appears to do it with no ill effect, and I have no
        // other ideas on how to properly free up the proxied ddraw.dll.
        HMODULE d3d8 = GetD3D8Module();
        if (likely(d3d8 != nullptr))
          FreeLibrary(d3d8);
        Logger::info("<<<<<<< UNLOADING D3D9TO8 <<<<<<<");
        break;
      }
      default:
        break;
    }
    return TRUE;
  }

}
