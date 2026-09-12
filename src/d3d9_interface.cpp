#include "d3d9_interface.h"

#include "d3d9_device.h"
#include "d3d9_util.h"

using Logger = ThreadSafeLogger;

D3D9Interface::D3D9Interface(d3d8::IDirect3D8* d3d8Intf)
  : m_d3d8 (d3d8Intf) {
  const UINT adapterCount = m_d3d8->GetAdapterCount();

  m_adapterModeCounts.resize(adapterCount);
  m_adapterModes.reserve(adapterCount);

  for (UINT adapter = 0; adapter < adapterCount; adapter++) {
    m_adapterModes.emplace_back();

    // cache adapter modes and mode counts for each of the two supported
    // D3D8 adapter formats: D3DFMT_X8R8G8B8 and D3DFMT_R5G6B5
    const UINT modeCount = m_d3d8->GetAdapterModeCount(adapter);

    for (UINT mode = 0; mode < modeCount; mode++) {
      D3DDISPLAYMODE displayMode;
      m_d3d8->EnumAdapterModes(adapter, mode, reinterpret_cast<d3d8::D3DDISPLAYMODE*>(&displayMode));

      switch (D3DFORMAT(displayMode.Format)) {
        case D3DFMT_X8R8G8B8:
          m_adapterModes[adapter][0].emplace_back(displayMode);
          m_adapterModeCounts[adapter][0]++;
          break;
        case D3DFMT_R5G6B5:
          m_adapterModes[adapter][1].emplace_back(displayMode);
          m_adapterModeCounts[adapter][1]++;
          break;
        default:
          break;
      }
    }
  }
}

D3D9Interface::~D3D9Interface() {
}

HRESULT STDMETHODCALLTYPE D3D9Interface::QueryInterface(REFIID riid, void** ppvObject) {
  Logger::info("D3D9Interface::QueryInterface:");

  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
   || riid == __uuidof(IDirect3D9)) {
    *ppvObject = ref(this);
    return S_OK;
  }

  if (riid == __uuidof(IDirect3D9Ex))
    return E_NOINTERFACE;

  Logger::warn("D3D9Interface::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9Interface::RegisterSoftwareDevice(void* pInitializeFunction) {
  Logger::warn("D3D9Interface::RegisterSoftwareDevice: Unsupported call!");
  return D3D_OK;
}

UINT STDMETHODCALLTYPE D3D9Interface::GetAdapterCount() {
  Logger::info("D3D9Interface::GetAdapterCount:");
  return m_d3d8->GetAdapterCount();
}

HRESULT STDMETHODCALLTYPE D3D9Interface::GetAdapterIdentifier(
        UINT                    Adapter,
        DWORD                   Flags,
        D3DADAPTER_IDENTIFIER9* pIdentifier) {
  Logger::info("D3D9Interface::GetAdapterCount:");

  d3d8::D3DADAPTER_IDENTIFIER8 identifier8;
  HRESULT hr = m_d3d8->GetAdapterIdentifier(Adapter, Flags, &identifier8);
  if (FAILED(hr))
    return hr;

  strncpy(pIdentifier->Driver, identifier8.Driver, MAX_DEVICE_IDENTIFIER_STRING);
  strncpy(pIdentifier->Description, identifier8.Description, MAX_DEVICE_IDENTIFIER_STRING);

  pIdentifier->DriverVersion    = identifier8.DriverVersion;
  pIdentifier->VendorId         = identifier8.VendorId;
  pIdentifier->DeviceId         = identifier8.DeviceId;
  pIdentifier->SubSysId         = identifier8.SubSysId;
  pIdentifier->Revision         = identifier8.Revision;
  pIdentifier->DeviceIdentifier = identifier8.DeviceIdentifier;

  pIdentifier->WHQLLevel = identifier8.WHQLLevel;

  return D3D_OK;
}

UINT STDMETHODCALLTYPE D3D9Interface::GetAdapterModeCount(UINT Adapter, D3DFORMAT Format) {
  Logger::info("D3D9Interface::GetAdapterModeCount:");

  Logger::debug("D3D9Interface::GetAdapterModeCount: Format: " + std::to_string(Format));

  // D3D8 can only use two D3DFMT_X8R8G8B8 (22) and D3DFMT_R5G6B5 (23) as adapter formats
  switch (Format) {
    case D3DFMT_X8R8G8B8:
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_X8R8G8B8: " + std::to_string(m_adapterModeCounts[Adapter][0]));
      return m_adapterModeCounts[Adapter][0];
    case D3DFMT_R5G6B5:
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_R5G6B5: " + std::to_string(m_adapterModeCounts[Adapter][1]));
      return m_adapterModeCounts[Adapter][1];
    default:
      return 0;
  }

  return 0;
}

HRESULT STDMETHODCALLTYPE D3D9Interface::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE* pMode) {
  Logger::info("D3D9Interface::GetAdapterDisplayMode:");
  return m_d3d8->GetAdapterDisplayMode(Adapter, reinterpret_cast<d3d8::D3DDISPLAYMODE*>(pMode));
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CheckDeviceType(
        UINT       Adapter,
        D3DDEVTYPE DevType,
        D3DFORMAT  AdapterFormat,
        D3DFORMAT  BackBufferFormat,
        BOOL       bWindowed) {
  Logger::info("D3D9Interface::CheckDeviceType:");
  return m_d3d8->CheckDeviceType(Adapter, d3d8::D3DDEVTYPE(DevType),
                                 d3d8::D3DFORMAT(AdapterFormat),
                                 d3d8::D3DFORMAT(BackBufferFormat), bWindowed);
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CheckDeviceFormat(
        UINT            Adapter,
        D3DDEVTYPE      DeviceType,
        D3DFORMAT       AdapterFormat,
        DWORD           Usage,
        D3DRESOURCETYPE RType,
        D3DFORMAT       CheckFormat) {
  Logger::info("D3D9Interface::CheckDeviceFormat:");

  if (IsUnsupportedD3D9Format(CheckFormat))
    Logger::warn("D3D9Interface::CheckDeviceFormat: Query for unsupported format: " + std::to_string(CheckFormat));

  return m_d3d8->CheckDeviceFormat(
    Adapter,
    (d3d8::D3DDEVTYPE)DeviceType,
    (d3d8::D3DFORMAT)AdapterFormat,
    Usage,
    (d3d8::D3DRESOURCETYPE)RType,
    (d3d8::D3DFORMAT)CheckFormat
  );
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CheckDeviceMultiSampleType(
        UINT                Adapter,
        D3DDEVTYPE          DeviceType,
        D3DFORMAT           SurfaceFormat,
        BOOL                Windowed,
        D3DMULTISAMPLE_TYPE MultiSampleType,
        DWORD*              pQualityLevels) {
  Logger::info("D3D9Interface::CheckDeviceMultiSampleType:");

  if (pQualityLevels != nullptr)
    *pQualityLevels = 0;

  return m_d3d8->CheckDeviceMultiSampleType(
    Adapter,
    (d3d8::D3DDEVTYPE)DeviceType,
    (d3d8::D3DFORMAT)SurfaceFormat,
    Windowed,
    (d3d8::D3DMULTISAMPLE_TYPE)MultiSampleType
  );
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CheckDepthStencilMatch(
        UINT       Adapter,
        D3DDEVTYPE DeviceType,
        D3DFORMAT  AdapterFormat,
        D3DFORMAT  RenderTargetFormat,
        D3DFORMAT  DepthStencilFormat) {
  Logger::info("D3D9Interface::CheckDepthStencilMatch:");
  return m_d3d8->CheckDepthStencilMatch(Adapter, d3d8::D3DDEVTYPE(DeviceType),
                                        d3d8::D3DFORMAT(AdapterFormat),
                                        d3d8::D3DFORMAT(RenderTargetFormat),
                                        d3d8::D3DFORMAT(DepthStencilFormat));
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CheckDeviceFormatConversion(
        UINT       Adapter,
        D3DDEVTYPE DeviceType,
        D3DFORMAT  SourceFormat,
        D3DFORMAT  TargetFormat) {
  Logger::warn("D3D9Interface::CheckDeviceFormatConversion: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Interface::GetDeviceCaps(
        UINT       Adapter,
        D3DDEVTYPE DeviceType,
        D3DCAPS9*  pCaps) {
  Logger::info("D3D9Interface::GetDeviceCaps:");

  if (pCaps == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DCAPS8 caps8;
  HRESULT hr = m_d3d8->GetDeviceCaps(Adapter, d3d8::D3DDEVTYPE(DeviceType), &caps8);
  if (FAILED(hr))
    return hr;

  ConvertCaps9(caps8, pCaps);

  return D3D_OK;
}

HMONITOR STDMETHODCALLTYPE D3D9Interface::GetAdapterMonitor(UINT Adapter) {
  Logger::info("D3D9Interface::GetAdapterMonitor:");
  return m_d3d8->GetAdapterMonitor(Adapter);
}

HRESULT STDMETHODCALLTYPE D3D9Interface::CreateDevice(
        UINT                   Adapter,
        D3DDEVTYPE             DeviceType,
        HWND                   hFocusWindow,
        DWORD                  BehaviorFlags,
        D3DPRESENT_PARAMETERS* pPresentationParameters,
        IDirect3DDevice9**     ppReturnedDeviceInterface) {
  Logger::info("D3D9Interface::CreateDevice:");

  if (ppReturnedDeviceInterface == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DPRESENT_PARAMETERS params8 = ConvertPresentParameters8(pPresentationParameters);

  d3d8::IDirect3DDevice8* d3d8Device;
  HRESULT hr = m_d3d8->CreateDevice(Adapter, static_cast<d3d8::D3DDEVTYPE>(DeviceType),
                                    hFocusWindow, BehaviorFlags, &params8,
                                    &d3d8Device);
  if (FAILED(hr)) {
    Logger::warn("D3D9Interface::CreateDevice: Failed to create D3D8 device");
    return hr;
  }

  *ppReturnedDeviceInterface = ref(new D3D9Device(this, d3d8Device));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Interface::EnumAdapterModes(
        UINT            Adapter,
        D3DFORMAT       Format,
        UINT            Mode,
        D3DDISPLAYMODE* pMode) {
  Logger::info("D3D9Interface::EnumAdapterModes:");

  Logger::debug("D3D9Interface::EnumAdapterModes: Mode:   " + std::to_string(Mode));
  Logger::debug("D3D9Interface::EnumAdapterModes: Format: " + std::to_string(Format));

  // D3D8 can only use two D3DFMT_X8R8G8B8 (22) and D3DFMT_R5G6B5 (23) as adapter formats
  switch (Format) {
    case D3DFMT_X8R8G8B8:
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_X8R8G8B8: Width:  " + std::to_string(m_adapterModes[Adapter][0][Mode].Width));
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_X8R8G8B8: Height: " + std::to_string(m_adapterModes[Adapter][0][Mode].Height));
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_X8R8G8B8: Hz:     " + std::to_string(m_adapterModes[Adapter][0][Mode].RefreshRate));
      *pMode = m_adapterModes[Adapter][0][Mode];
      break;
    case D3DFMT_R5G6B5:
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_R5G6B5: Width:  " + std::to_string(m_adapterModes[Adapter][1][Mode].Width));
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_R5G6B5: Height: " + std::to_string(m_adapterModes[Adapter][1][Mode].Height));
      Logger::debug("D3D9Interface::GetAdapterModeCount: D3DFMT_R5G6B5: Hz:     " + std::to_string(m_adapterModes[Adapter][1][Mode].RefreshRate));
      *pMode = m_adapterModes[Adapter][1][Mode];
      break;
    default:
      return D3D_OK;
  }

  return D3D_OK;
}

