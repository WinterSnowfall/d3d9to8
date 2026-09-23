#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"

#include <vector>
#include <array>

class D3D9Interface final : public ComObjectClamp<IDirect3D9> {

public:

  D3D9Interface(ComObject<d3d8::IDirect3D8>&& d3d8Intf);

  ~D3D9Interface();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE RegisterSoftwareDevice(void* pInitializeFunction);

  UINT STDMETHODCALLTYPE GetAdapterCount();

  HRESULT STDMETHODCALLTYPE GetAdapterIdentifier(
          UINT                    Adapter,
          DWORD                   Flags,
          D3DADAPTER_IDENTIFIER9* pIdentifier);

  UINT STDMETHODCALLTYPE GetAdapterModeCount(UINT Adapter, D3DFORMAT Format);

  HRESULT STDMETHODCALLTYPE GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode);

  HRESULT STDMETHODCALLTYPE CheckDeviceType(
          UINT       Adapter,
          D3DDEVTYPE DevType,
          D3DFORMAT  AdapterFormat,
          D3DFORMAT  BackBufferFormat,
          BOOL       bWindowed);

  HRESULT STDMETHODCALLTYPE CheckDeviceFormat(
          UINT            Adapter,
          D3DDEVTYPE      DeviceType,
          D3DFORMAT       AdapterFormat,
          DWORD           Usage,
          D3DRESOURCETYPE RType,
          D3DFORMAT       CheckFormat);

  HRESULT STDMETHODCALLTYPE CheckDeviceMultiSampleType(
          UINT                Adapter,
          D3DDEVTYPE          DeviceType,
          D3DFORMAT           SurfaceFormat,
          BOOL                Windowed,
          D3DMULTISAMPLE_TYPE MultiSampleType,
          DWORD*              pQualityLevels);

  HRESULT STDMETHODCALLTYPE CheckDepthStencilMatch(
          UINT       Adapter,
          D3DDEVTYPE DeviceType,
          D3DFORMAT  AdapterFormat,
          D3DFORMAT  RenderTargetFormat,
          D3DFORMAT  DepthStencilFormat);

  HRESULT STDMETHODCALLTYPE CheckDeviceFormatConversion(
          UINT       Adapter,
          D3DDEVTYPE DeviceType,
          D3DFORMAT  SourceFormat,
          D3DFORMAT  TargetFormat);

  HRESULT STDMETHODCALLTYPE GetDeviceCaps(
          UINT       Adapter,
          D3DDEVTYPE DeviceType,
          D3DCAPS9*  pCaps);

  HMONITOR STDMETHODCALLTYPE GetAdapterMonitor(UINT Adapter);

  HRESULT STDMETHODCALLTYPE CreateDevice(
          UINT                   Adapter,
          D3DDEVTYPE             DeviceType,
          HWND                   hFocusWindow,
          DWORD                  BehaviorFlags,
          D3DPRESENT_PARAMETERS* pPresentationParameters,
          IDirect3DDevice9**     ppReturnedDeviceInterface);

  HRESULT STDMETHODCALLTYPE EnumAdapterModes(
          UINT            Adapter,
          D3DFORMAT       Format,
          UINT            Mode,
          D3DDISPLAYMODE* pMode);

private:

  ComObject<d3d8::IDirect3D8>      m_d3d8;

  UINT                             m_adapterCount = 0u;
  std::vector<std::array<UINT, 2>> m_adapterModeCounts;
  std::vector<std::array<std::vector<D3DDISPLAYMODE>, 2>> m_adapterModes;

};
