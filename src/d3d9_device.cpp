#include "d3d9_device.h"

#include "d3d9_interface.h"
#include "d3d9_query.h"
#include "d3d9_stateblock.h"

using Logger = ThreadSafeLogger;

D3D9Device::D3D9Device(
    IDirect3D9* intf,
    ComObject<d3d8::IDirect3DDevice8>&& d3d8Device,
    D3DPRESENT_PARAMETERS presentParams,
    DWORD behaviorFlags)
  : m_isMultitheaded ( behaviorFlags & D3DCREATE_MULTITHREADED )
  , m_intf ( intf )
  , m_d3d8 ( std::move(d3d8Device) )
  , m_presentParams ( presentParams ) {
  ClearCachedD3D8Objects();
  CacheD3D8ObjectsAndRestoreState();

  if (m_isMultitheaded)
    Logger::info("D3D9Device:: Accounting for D3DCREATE_MULTITHREADED");

  if (behaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) {
    Logger::info("D3D9Device:: Using D3DCREATE_HARDWARE_VERTEXPROCESSING");
  } else if (behaviorFlags & D3DCREATE_MIXED_VERTEXPROCESSING) {
    Logger::info("D3D9Device:: Using D3DCREATE_MIXED_VERTEXPROCESSING");
    m_canSWVP = true;
  } else if (behaviorFlags & D3DCREATE_SOFTWARE_VERTEXPROCESSING) {
    Logger::info("D3D9Device:: Using D3DCREATE_SOFTWARE_VERTEXPROCESSING");
    m_canSWVP = true;
  }

  Logger::debug("D3D9Device:: Windowed: " + std::to_string(m_presentParams.Windowed));
  Logger::debug("D3D9Device:: BackBufferFormat: " + std::to_string(m_presentParams.BackBufferFormat));
  Logger::debug("D3D9Device:: BackBufferCount: " + std::to_string(m_presentParams.BackBufferCount));
  Logger::debug("D3D9Device:: EnableAutoDepthStencil: " + std::to_string(m_presentParams.EnableAutoDepthStencil));
  Logger::debug("D3D9Device:: AutoDepthStencilFormat: " + std::to_string(m_presentParams.AutoDepthStencilFormat));
}

D3D9Device::~D3D9Device() {
}

HRESULT STDMETHODCALLTYPE D3D9Device::QueryInterface(REFIID riid, void** ppvObject) {
  if (unlikely(ppvObject == nullptr))
    return E_POINTER;

  ClearReturnPointer(ppvObject);

  if (likely(riid == __uuidof(IUnknown)
          || riid == __uuidof(IDirect3DDevice9))) {
    *ppvObject = ref(this);
    return S_OK;
  }

  if (riid == __uuidof(IDirect3DDevice9Ex))
    return E_NOINTERFACE;

  Logger::warn("D3D9Device::QueryInterface: Unknown interface query:");
  Logger::warn(riid);
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9Device::TestCooperativeLevel() {
  return m_d3d8->TestCooperativeLevel();
}

UINT STDMETHODCALLTYPE D3D9Device::GetAvailableTextureMem() {
  return m_d3d8->GetAvailableTextureMem();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EvictManagedResources() {
  return m_d3d8->ResourceManagerDiscardBytes(0);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDirect3D(IDirect3D9** ppD3D9) {
  if (unlikely(ppD3D9 == nullptr))
    return D3DERR_INVALIDCALL;

  *ppD3D9 = m_intf.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDeviceCaps(D3DCAPS9* pCaps) {
  if (unlikely(pCaps == nullptr))
    return D3DERR_INVALIDCALL;

  d3d8::D3DCAPS8 caps8;
  HRESULT hr = m_d3d8->GetDeviceCaps(&caps8);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::GetDeviceCaps: Failed to get D3D8 caps");
    return hr;
  }

  ConvertCaps9(caps8, pCaps);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::GetDisplayMode: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  return m_d3d8->GetDisplayMode(reinterpret_cast<d3d8::D3DDISPLAYMODE*>(pMode));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
  return m_d3d8->GetCreationParameters(reinterpret_cast<d3d8::D3DDEVICE_CREATION_PARAMETERS*>(pParameters));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetCursorProperties(
        UINT               XHotSpot,
        UINT               YHotSpot,
        IDirect3DSurface9* pCursorBitmap) {
  D3D9Surface* d3d9Surface = reinterpret_cast<D3D9Surface*>(pCursorBitmap);
  return m_d3d8->SetCursorProperties(XHotSpot, YHotSpot, d3d9Surface->GetD3D8Surface());
}

void STDMETHODCALLTYPE D3D9Device::SetCursorPosition(int X, int Y, DWORD Flags) {
  m_d3d8->SetCursorPosition(X, Y, Flags);
}

BOOL STDMETHODCALLTYPE D3D9Device::ShowCursor(BOOL bShow) {
  return m_d3d8->ShowCursor(bShow);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateAdditionalSwapChain(
        D3DPRESENT_PARAMETERS* pPresentationParameters,
        IDirect3DSwapChain9**  ppSwapChain) {
  if (unlikely(ppSwapChain == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSwapChain);

  d3d8::D3DPRESENT_PARAMETERS params8 = ConvertPresentParameters8(pPresentationParameters);

  ComObject<d3d8::IDirect3DSwapChain8> d3d8SwapChain;
  HRESULT hr = m_d3d8->CreateAdditionalSwapChain(&params8, &d3d8SwapChain);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateAdditionalSwapChain: Failed to create D3D8 swapchain");
    return hr;
  }

  *ppSwapChain = ref(new D3D9SwapChain(this, std::move(d3d8SwapChain), pPresentationParameters));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
  if (unlikely(pSwapChain == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(pSwapChain);

  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::GetSwapChain: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  // Return a dummy swapchain if we get queries for the implicit swapchain
  if (unlikely(m_implicitSwapchain == nullptr))
    m_implicitSwapchain = new D3D9SwapChain(this, nullptr, nullptr);

  *pSwapChain = m_implicitSwapchain.ref();

  return D3D_OK;
}

UINT STDMETHODCALLTYPE D3D9Device::GetNumberOfSwapChains() {
  return 1;
}

HRESULT STDMETHODCALLTYPE D3D9Device::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  ClearCachedD3D8Objects();

  d3d8::D3DPRESENT_PARAMETERS params8 = ConvertPresentParameters8(pPresentationParameters);
  // Cache only after conversion, because some corrections may be applied
  m_presentParams = *pPresentationParameters;

  HRESULT hr = m_d3d8->Reset(&params8);
  // Failed calls can be legitimate cues that make calling apps release
  // any still held resources and then retry the Reset() call
  if (unlikely(FAILED(hr))) {
    Logger::debug("D3D9Device::Reset: Failed to reset the D3D8 device");
    return hr;
  }

  CacheD3D8ObjectsAndRestoreState();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::Present(
  const RECT*    pSourceRect,
  const RECT*    pDestRect,
        HWND     hDestWindowOverride,
  const RGNDATA* pDirtyRegion) {
  return m_d3d8->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetBackBuffer(
        UINT                iSwapChain,
        UINT                iBackBuffer,
        D3DBACKBUFFER_TYPE  Type,
        IDirect3DSurface9** ppBackBuffer) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppBackBuffer == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppBackBuffer);

  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::GetBackBuffer: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  if (unlikely(m_backBuffers[iBackBuffer] == nullptr || iBackBuffer >= m_backBuffers.size())) {
    ComObject<d3d8::IDirect3DSurface8> d3d8BackBuffer;
    HRESULT hr = m_d3d8->GetBackBuffer(iBackBuffer, d3d8::D3DBACKBUFFER_TYPE(Type),
                                       &d3d8BackBuffer);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Device::GetBackBuffer: Failed to get D3D8 back buffer");
      return hr;
    }

    m_backBuffers[iBackBuffer] = new D3D9Surface(this, std::move(d3d8BackBuffer), nullptr);
  }

  *ppBackBuffer = m_backBuffers[iBackBuffer].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::GetRasterStatus: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  return m_d3d8->GetRasterStatus(reinterpret_cast<d3d8::D3DRASTER_STATUS*>(pRasterStatus));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetDialogBoxMode(BOOL bEnableDialogs) {
  Logger::warn("D3D9Device::SetDialogBoxMode: Unsupported call!");
  return D3D_OK;
}

void STDMETHODCALLTYPE D3D9Device::SetGammaRamp(
        UINT          iSwapChain,
        DWORD         Flags,
  const D3DGAMMARAMP* pRamp) {
  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::SetGammaRamp: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  m_d3d8->SetGammaRamp(Flags, reinterpret_cast<const d3d8::D3DGAMMARAMP*>(pRamp));
}

void STDMETHODCALLTYPE D3D9Device::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
  if (unlikely(iSwapChain != 0))
    Logger::warn("D3D9Device::GetGammaRamp: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  m_d3d8->GetGammaRamp(reinterpret_cast<d3d8::D3DGAMMARAMP*>(pRamp));
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateTexture(
        UINT                Width,
        UINT                Height,
        UINT                Levels,
        DWORD               Usage,
        D3DFORMAT           Format,
        D3DPOOL             Pool,
        IDirect3DTexture9** ppTexture,
        HANDLE*             pSharedHandle) {
  if (unlikely(ppTexture == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppTexture);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateTexture: Use of unsupported format: " + std::to_string(Format));

  ConvertD3D9Usage(&Usage, &Levels);

  ComObject<d3d8::IDirect3DTexture8> d3d8Texture;
  HRESULT hr = m_d3d8->CreateTexture(Width, Height, Levels, Usage,
                                     d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                     &d3d8Texture);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateTexture: Failed to create D3D8 texture");

  //Logger::debug("D3D9Device::CreateTexture: Width:  " + std::to_string(Width));
  //Logger::debug("D3D9Device::CreateTexture: Height: " + std::to_string(Height));
  //Logger::debug("D3D9Device::CreateTexture: Levels: " + std::to_string(Levels));
  //Logger::debug("D3D9Device::CreateTexture: Usage:  " + std::to_string(Usage));
  //Logger::debug("D3D9Device::CreateTexture: Format: " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateTexture: Pool:   " + std::to_string(Pool));

    return hr;
  }

  *ppTexture = ref(new D3D9Texture2D(this, std::move(d3d8Texture), Levels));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVolumeTexture(
        UINT                      Width,
        UINT                      Height,
        UINT                      Depth,
        UINT                      Levels,
        DWORD                     Usage,
        D3DFORMAT                 Format,
        D3DPOOL                   Pool,
        IDirect3DVolumeTexture9** ppVolumeTexture,
        HANDLE*                   pSharedHandle) {
  if (unlikely(ppVolumeTexture == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppVolumeTexture);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateVolumeTexture: Use of unsupported format: " + std::to_string(Format));

  ConvertD3D9Usage(&Usage, &Levels);

  ComObject<d3d8::IDirect3DVolumeTexture8> d3d8VolumeTexture;
  HRESULT hr = m_d3d8->CreateVolumeTexture(Width, Height, Depth, Levels, Usage,
                                           d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                           &d3d8VolumeTexture);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateVolumeTexture: Failed to create D3D8 volume texture");

  //Logger::debug("D3D9Device::CreateVolumeTexture: Width:  " + std::to_string(Width));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Height: " + std::to_string(Height));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Depth:  " + std::to_string(Depth));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Levels: " + std::to_string(Levels));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Usage:  " + std::to_string(Usage));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Format: " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateVolumeTexture: Pool:   " + std::to_string(Pool));

    return hr;
  }

  *ppVolumeTexture = ref(new D3D9Texture3D(this, std::move(d3d8VolumeTexture), Levels));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateCubeTexture(
        UINT                    EdgeLength,
        UINT                    Levels,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        IDirect3DCubeTexture9** ppCubeTexture,
        HANDLE*                 pSharedHandle) {
  if (unlikely(ppCubeTexture == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppCubeTexture);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateCubeTexture: Use of unsupported format: " + std::to_string(Format));

  ConvertD3D9Usage(&Usage, &Levels);

  ComObject<d3d8::IDirect3DCubeTexture8> d3d8CubeTexture = nullptr;
  HRESULT hr = m_d3d8->CreateCubeTexture(EdgeLength, Levels, Usage,
                                         d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                         &d3d8CubeTexture);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateCubeTexture: Failed to create D3D8 cube texture");

  //Logger::debug("D3D9Device::CreateCubeTexture: EdgeLength:  " + std::to_string(EdgeLength));
  //Logger::debug("D3D9Device::CreateCubeTexture: Levels:      " + std::to_string(Levels));
  //Logger::debug("D3D9Device::CreateCubeTexture: Usage:       " + std::to_string(Usage));
  //Logger::debug("D3D9Device::CreateCubeTexture: Format:      " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateCubeTexture: Pool:        " + std::to_string(Pool));

    return hr;
  }

  *ppCubeTexture = ref(new D3D9TextureCube(this, std::move(d3d8CubeTexture), Levels));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexBuffer(
        UINT                     Length,
        DWORD                    Usage,
        DWORD                    FVF,
        D3DPOOL                  Pool,
        IDirect3DVertexBuffer9** ppVertexBuffer,
        HANDLE*                  pSharedHandle) {
  if (unlikely(ppVertexBuffer == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppVertexBuffer);

  ComObject<d3d8::IDirect3DVertexBuffer8> d3d8VertexBuffer;
  HRESULT hr = m_d3d8->CreateVertexBuffer(Length, Usage, FVF,
                                          d3d8::D3DPOOL(Pool), &d3d8VertexBuffer);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateVertexBuffer: Failed to create D3D8 vertex buffer");
    return hr;
  }

  *ppVertexBuffer = ref(new D3D9VertexBuffer(this, std::move(d3d8VertexBuffer)));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateIndexBuffer(
        UINT                    Length,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        IDirect3DIndexBuffer9** ppIndexBuffer,
        HANDLE*                 pSharedHandle) {
  if (unlikely(ppIndexBuffer == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppIndexBuffer);

  ComObject<d3d8::IDirect3DIndexBuffer8> d3d8IndexBuffer;
  HRESULT hr = m_d3d8->CreateIndexBuffer(Length, Usage, d3d8::D3DFORMAT(Format),
                                         d3d8::D3DPOOL(Pool), &d3d8IndexBuffer);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateIndexBuffer: Failed to create D3D8 index buffer");
    return hr;
  }

  *ppIndexBuffer = ref(new D3D9IndexBuffer(this, std::move(d3d8IndexBuffer)));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateRenderTarget(
        UINT                Width,
        UINT                Height,
        D3DFORMAT           Format,
        D3DMULTISAMPLE_TYPE MultiSample,
        DWORD               MultisampleQuality,
        BOOL                Lockable,
        IDirect3DSurface9** ppSurface,
        HANDLE*             pSharedHandle) {
  if (unlikely(ppSurface == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurface);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateRenderTarget: Use of unsupported format: " + std::to_string(Format));

  ComObject<d3d8::IDirect3DSurface8> d3d8RenderTarget;
  HRESULT hr = m_d3d8->CreateRenderTarget(Width, Height,
                                          d3d8::D3DFORMAT(Format),
                                          d3d8::D3DMULTISAMPLE_TYPE(MultiSample),
                                          Lockable, &d3d8RenderTarget);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateRenderTarget: Failed to create D3D8 render target surface");

  //Logger::debug("D3D9Device::CreateRenderTarget: Width:        " + std::to_string(Width));
  //Logger::debug("D3D9Device::CreateRenderTarget: Height:       " + std::to_string(Height));
  //Logger::debug("D3D9Device::CreateRenderTarget: Format:       " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateRenderTarget: MultiSample:  " + std::to_string(MultiSample));
  //Logger::debug("D3D9Device::CreateRenderTarget: MultisampleQ: " + std::to_string(MultisampleQuality));
  //Logger::debug("D3D9Device::CreateRenderTarget: Lockable:     " + std::to_string(Lockable));

    return hr;
  }

  *ppSurface = ref(new D3D9Surface(this, std::move(d3d8RenderTarget), nullptr));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateDepthStencilSurface(
        UINT                Width,
        UINT                Height,
        D3DFORMAT           Format,
        D3DMULTISAMPLE_TYPE MultiSample,
        DWORD               MultisampleQuality,
        BOOL                Discard,
        IDirect3DSurface9** ppSurface,
        HANDLE*             pSharedHandle) {
  if (unlikely(ppSurface == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurface);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateDepthStencilSurface: Use of unsupported format: " + std::to_string(Format));

  ComObject<d3d8::IDirect3DSurface8> d3d8DepthStencil;
  HRESULT hr = m_d3d8->CreateDepthStencilSurface(Width, Height,
                                                 d3d8::D3DFORMAT(Format),
                                                 d3d8::D3DMULTISAMPLE_TYPE(MultiSample),
                                                 &d3d8DepthStencil);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateDepthStencilSurface: Failed to create D3D8 depth stencil surface");

  //Logger::debug("D3D9Device::CreateDepthStencilSurface: Width:        " + std::to_string(Width));
  //Logger::debug("D3D9Device::CreateDepthStencilSurface: Height:       " + std::to_string(Height));
  //Logger::debug("D3D9Device::CreateDepthStencilSurface: Format:       " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateDepthStencilSurface: MultiSample:  " + std::to_string(MultiSample));
  //Logger::debug("D3D9Device::CreateDepthStencilSurface: MultisampleQ: " + std::to_string(MultisampleQuality));
  //Logger::debug("D3D9Device::CreateDepthStencilSurface: Discard:      " + std::to_string(Discard));

    return hr;
  }

  *ppSurface = ref(new D3D9Surface(this, std::move(d3d8DepthStencil), nullptr));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::UpdateSurface(
        IDirect3DSurface9* pSourceSurface,
  const RECT*              pSourceRect,
        IDirect3DSurface9* pDestinationSurface,
  const POINT*             pDestPoint) {
  D3D9Surface* sourceSurface9 = reinterpret_cast<D3D9Surface*>(pSourceSurface);
  D3D9Surface* destinationSurface9 = reinterpret_cast<D3D9Surface*>(pDestinationSurface);

  HRESULT hr = m_d3d8->CopyRects(sourceSurface9->GetD3D8Surface(), pSourceRect, 1,
                                 destinationSurface9->GetD3D8Surface(), pDestPoint);
  if (unlikely(FAILED(hr))) {
    Logger::debug("D3D9Device::UpdateSurface: Failed D3D8 call to CopyRects");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::UpdateTexture(
        IDirect3DBaseTexture9* pSourceTexture,
        IDirect3DBaseTexture9* pDestinationTexture) {
  if (unlikely(pSourceTexture == nullptr || pDestinationTexture == nullptr))
    return D3DERR_INVALIDCALL;

  D3D9Texture2D* sourceTexture      = reinterpret_cast<D3D9Texture2D*>(pSourceTexture);
  D3D9Texture2D* destinationTexture = reinterpret_cast<D3D9Texture2D*>(pDestinationTexture);

  return m_d3d8->UpdateTexture(sourceTexture->GetD3D8BaseTexture(),
                               destinationTexture->GetD3D8BaseTexture());
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderTargetData(
        IDirect3DSurface9* pRenderTarget,
        IDirect3DSurface9* pDestSurface) {
  if (unlikely(pRenderTarget == nullptr || pDestSurface == nullptr))
    return D3DERR_INVALIDCALL;

  D3D9Surface* sourceSurface9 = reinterpret_cast<D3D9Surface*>(pRenderTarget);
  D3D9Surface* destinationSurface9 = reinterpret_cast<D3D9Surface*>(pDestSurface);

  // CopyRects will fail if the surfaces have different dimensions,
  // which is actually the expected behavior here
  HRESULT hr = m_d3d8->CopyRects(sourceSurface9->GetD3D8Surface(), NULL, 0,
                                 destinationSurface9->GetD3D8Surface(), NULL);
  if (unlikely(FAILED(hr))) {
    Logger::debug("D3D9Device::GetRenderTargetData: Failed D3D8 call to CopyRects");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
  if (unlikely(pDestSurface == nullptr))
    return D3DERR_INVALIDCALL;

  if (iSwapChain != 0)
    Logger::warn("D3D9Device::GetFrontBufferData: Unsupported use of iSwapChain: " + std::to_string(iSwapChain));

  D3D9Surface* destinationSurface9 = reinterpret_cast<D3D9Surface*>(pDestSurface);

  return m_d3d8->GetFrontBuffer(destinationSurface9->GetD3D8Surface());
}

HRESULT STDMETHODCALLTYPE D3D9Device::StretchRect(
        IDirect3DSurface9*   pSourceSurface,
  const RECT*                pSourceRect,
        IDirect3DSurface9*   pDestSurface,
  const RECT*                pDestRect,
        D3DTEXTUREFILTERTYPE Filter) {
  if (unlikely(pSourceSurface == nullptr || pDestSurface == nullptr))
    return D3DERR_INVALIDCALL;

  D3DSURFACE_DESC sourceSurfaceDesc;
  pSourceSurface->GetDesc(&sourceSurfaceDesc);
  D3DSURFACE_DESC destSurfaceDesc;
  pDestSurface->GetDesc(&destSurfaceDesc);

  // CopyRects doesn't support format conversion...
  if (sourceSurfaceDesc.Format != destSurfaceDesc.Format) {
    Logger::warn("D3D9Device::StretchRect: Unsupported use of format conversion");
    return D3D_OK; // Don't error out, as some games explode because of it
  }

  const uint32_t sourceSurfaceWidth  = pSourceRect == nullptr  ? sourceSurfaceDesc.Width  :
                                                                 pSourceRect->right - pSourceRect->left;
  const uint32_t sourceSurfaceHeight = pSourceRect == nullptr  ? sourceSurfaceDesc.Height :
                                                                 pSourceRect->bottom - pSourceRect->top;
  const uint32_t destSurfaceWidth    = pDestRect == nullptr ? destSurfaceDesc.Width :
                                                              pDestRect->right - pDestRect->left;
  const uint32_t destSurfaceHeight   = pDestRect == nullptr ? destSurfaceDesc.Height :
                                                              pDestRect->bottom - pDestRect->top;

  const bool doesStretching = sourceSurfaceWidth != destSurfaceWidth || sourceSurfaceHeight != destSurfaceHeight;

  // Unfortunately, this will be hit in the vast majority of cases
  if (doesStretching) {
    Logger::warn("D3D9Device::StretchRect: Unsupported use of stretching");
    return D3D_OK; // Don't error out, as some games explode because of it
  }

  // In case of a non-stretched call, the destination point will always be the top left corner
  const POINT destPoint = { 0u, 0u };

  D3D9Surface* sourceSurface9 = reinterpret_cast<D3D9Surface*>(pSourceSurface);
  D3D9Surface* destSurface9 = reinterpret_cast<D3D9Surface*>(pDestSurface);

  HRESULT hr = m_d3d8->CopyRects(sourceSurface9->GetD3D8Surface(), pSourceRect, 1,
                                 destSurface9->GetD3D8Surface(), &destPoint);
  if (unlikely(FAILED(hr))) {
    Logger::debug("D3D9Device::StretchRect: Failed D3D8 call to CopyRects");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::ColorFill(
        IDirect3DSurface9* pSurface,
  const RECT*              pRect,
        D3DCOLOR           Color) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(pSurface == nullptr))
    return D3DERR_INVALIDCALL;

  D3D9Surface* destSurface9 = reinterpret_cast<D3D9Surface*>(pSurface);

  // Use the viewport to apply color clears on the current render target,
  // or the current depth stencil, as some mad games use it for depth clears...
  if (m_renderTarget == destSurface9 || m_depthStencil == destSurface9) {
    d3d8::D3DVIEWPORT8 currentViewport;
    m_d3d8->GetViewport(&currentViewport);

    D3DSURFACE_DESC destSurfaceDesc;
    pSurface->GetDesc(&destSurfaceDesc);

    // We need to make sure the viewport is set to
    // the full surface dimensions before we clear it
    d3d8::D3DVIEWPORT8 clearViewport;
    clearViewport.X = 0u;
    clearViewport.Y = 0u;
    clearViewport.Width  = destSurfaceDesc.Width;
    clearViewport.Height = destSurfaceDesc.Height;
    clearViewport.MinZ = 0.0f;
    clearViewport.MaxZ = 1.0f;

    const DWORD clearFlags = m_renderTarget == destSurface9 ? D3DCLEAR_TARGET : D3DCLEAR_ZBUFFER;

    if (clearFlags & D3DCLEAR_TARGET)
      Logger::debug("D3D9Device::ColorFill: Clearing the current render target");
    else if (clearFlags & D3DCLEAR_ZBUFFER)
      Logger::debug("D3D9Device::ColorFill: Clearing the current depth stencil");

    m_d3d8->SetViewport(&clearViewport);

    HRESULT hr = m_d3d8->Clear(pRect == nullptr ? 0 : 1, reinterpret_cast<const d3d8::D3DRECT*>(pRect),
                               clearFlags, Color, 1.0f, 0);
    if (unlikely(FAILED(hr)))
      Logger::warn("D3D9Device::ColorFill: Failed D3D8 viewport color clear");

    m_d3d8->SetViewport(&currentViewport);
  // TODO: Potentially handle other renderable surfaces
  } else {
    Logger::warn("D3D9Device::ColorFill: Unsupported surface use!");
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateOffscreenPlainSurface(
        UINT Width,
        UINT Height,
        D3DFORMAT Format,
        D3DPOOL Pool,
        IDirect3DSurface9** ppSurface,
        HANDLE* pSharedHandle) {
  if (unlikely(ppSurface == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSurface);

  if (unlikely(IsUnsupportedD3D9Format(Format)))
    Logger::err("D3D9Device::CreateOffscreenPlainSurface: Use of unsupported format: " + std::to_string(Format));

  ComObject<d3d8::IDirect3DSurface8> d3d8OffscreenSurface;
  HRESULT hr = m_d3d8->CreateImageSurface(Width, Height,
                                          d3d8::D3DFORMAT(Format),
                                          &d3d8OffscreenSurface);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateOffscreenPlainSurface: Failed to create D3D8 offscreen surface");

  //Logger::debug("D3D9Device::CreateOffscreenPlainSurface: Width:  " + std::to_string(Width));
  //Logger::debug("D3D9Device::CreateOffscreenPlainSurface: Height: " + std::to_string(Height));
  //Logger::debug("D3D9Device::CreateOffscreenPlainSurface: Format: " + std::to_string(Format));
  //Logger::debug("D3D9Device::CreateOffscreenPlainSurface: Pool:   " + std::to_string(Pool));

    return hr;
  }

  *ppSurface = ref(new D3D9Surface(this, std::move(d3d8OffscreenSurface), nullptr));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetRenderTarget(
        DWORD              RenderTargetIndex,
        IDirect3DSurface9* pRenderTarget) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(pRenderTarget == nullptr && RenderTargetIndex == 0))
    return D3DERR_INVALIDCALL;

  if (unlikely(RenderTargetIndex != 0)) {
    if (pRenderTarget != nullptr) {
      Logger::err("D3D9Device::SetRenderTarget: Unsupported use of RenderTargetIndex: " + std::to_string(RenderTargetIndex));
      return D3DERR_INVALIDCALL;
    }
    return D3D_OK;
  }

  D3D9Surface* d3d9RenderTarget = reinterpret_cast<D3D9Surface*>(pRenderTarget);

  HRESULT hr = m_d3d8->SetRenderTarget(d3d9RenderTarget != nullptr ? d3d9RenderTarget->GetD3D8Surface() : nullptr, nullptr);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetRenderTarget: Failed to set D3D8 render target");
    return hr;
  }

  m_renderTarget = d3d9RenderTarget;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderTarget(
        DWORD               RenderTargetIndex,
        IDirect3DSurface9** ppRenderTarget) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppRenderTarget == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppRenderTarget);

  if (unlikely(RenderTargetIndex != 0)) {
    Logger::err("D3D9Device::GetRenderTarget: Unsupported use of RenderTargetIndex: " + std::to_string(RenderTargetIndex));
    return D3DERR_NOTFOUND;
  }

  if (unlikely(m_renderTarget == nullptr)) {
    ComObject<d3d8::IDirect3DSurface8> d3d8RenderTarget;
    HRESULT hr = m_d3d8->GetRenderTarget(&d3d8RenderTarget);
    if (unlikely(FAILED(hr))) {
      Logger::err("D3D9Device::GetRenderTarget: Failed to get D3D8 render target");
      return hr;
    }

    m_renderTarget = new D3D9Surface(this, std::move(d3d8RenderTarget), nullptr);
  }

  *ppRenderTarget = m_renderTarget.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  D3D9Surface* d3d9DepthStencil = reinterpret_cast<D3D9Surface*>(pNewZStencil);

  // TODO: Consider checking the DS size against the RT and creating a new one if
  // needed, because D3D8 will validate dimensions, and apparently Battle Engine
  // Aquila fails to set a depth stencil repeateadly for some reason...
  HRESULT hr = m_d3d8->SetRenderTarget(nullptr, d3d9DepthStencil != nullptr ? d3d9DepthStencil->GetD3D8Surface() : nullptr);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetDepthStencilSurface: Failed to set D3D8 depth stencil");
    return hr;
  }

  m_depthStencil = d3d9DepthStencil;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppZStencilSurface == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppZStencilSurface);

  if (unlikely(m_depthStencil == nullptr)) {
    ComObject<d3d8::IDirect3DSurface8> d3d8ZStencilSurface;
    HRESULT hr = m_d3d8->GetDepthStencilSurface(&d3d8ZStencilSurface);
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Device::GetDepthStencilSurface: Failed to get D3D8 depth stencil");
      return hr;
    }

    m_depthStencil = new D3D9Surface(this, std::move(d3d8ZStencilSurface), nullptr);
  }

  *ppZStencilSurface = m_depthStencil.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::BeginScene() {
  return m_d3d8->BeginScene();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EndScene() {
  return m_d3d8->EndScene();
}

HRESULT STDMETHODCALLTYPE D3D9Device::Clear(
        DWORD    Count,
  const D3DRECT* pRects,
        DWORD    Flags,
        D3DCOLOR Color,
        float    Z,
        DWORD    Stencil) {
  return m_d3d8->Clear(Count, reinterpret_cast<const d3d8::D3DRECT*>(pRects), Flags, Color, Z, Stencil);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) {
  return m_d3d8->SetTransform(d3d8::D3DTRANSFORMSTATETYPE(State), reinterpret_cast<const d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) {
  return m_d3d8->GetTransform(d3d8::D3DTRANSFORMSTATETYPE(State), reinterpret_cast<d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, const D3DMATRIX* pMatrix) {
  return m_d3d8->MultiplyTransform(d3d8::D3DTRANSFORMSTATETYPE(TransformState), reinterpret_cast<const d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetViewport(const D3DVIEWPORT9* pViewport) {
  return m_d3d8->SetViewport(reinterpret_cast<const d3d8::D3DVIEWPORT8*>(pViewport));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetViewport(D3DVIEWPORT9* pViewport) {
  return m_d3d8->GetViewport(reinterpret_cast<d3d8::D3DVIEWPORT8*>(pViewport));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetMaterial(const D3DMATERIAL9* pMaterial) {
  return m_d3d8->SetMaterial(reinterpret_cast<const d3d8::D3DMATERIAL8*>(pMaterial));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetMaterial(D3DMATERIAL9* pMaterial) {
  return m_d3d8->GetMaterial(reinterpret_cast<d3d8::D3DMATERIAL8*>(pMaterial));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetLight(DWORD Index, const D3DLIGHT9* pLight) {
  return m_d3d8->SetLight(Index, reinterpret_cast<const d3d8::D3DLIGHT8*>(pLight));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetLight(DWORD Index, D3DLIGHT9* pLight) {
  return m_d3d8->GetLight(Index, reinterpret_cast<d3d8::D3DLIGHT8*>(pLight));
}

HRESULT STDMETHODCALLTYPE D3D9Device::LightEnable(DWORD Index, BOOL Enable) {
  return m_d3d8->LightEnable(Index, Enable);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetLightEnable(DWORD Index, BOOL* pEnable) {
  return m_d3d8->GetLightEnable(Index, pEnable);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetClipPlane(DWORD Index, const float* pPlane) {
  return m_d3d8->SetClipPlane(Index, pPlane);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetClipPlane(DWORD Index, float* pPlane) {
  return m_d3d8->GetClipPlane(Index, pPlane);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
  d3d8::D3DRENDERSTATETYPE State8 = d3d8::D3DRENDERSTATETYPE(State);

  switch (State) {
    // For render states above 173, check against the actual defaults in case they're not 0
    case D3DRS_MINTESSELLATIONLEVEL:
      if (Value != bitcast<DWORD>(1.0f))
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_MINTESSELLATIONLEVEL");
      break;

    case D3DRS_MAXTESSELLATIONLEVEL:
      if (Value != bitcast<DWORD>(1.0f))
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_MINTESSELLATIONLEVEL");
      break;

    case D3DRS_ADAPTIVETESS_Z:
      if (Value != bitcast<DWORD>(1.0f))
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_ADAPTIVETESS_Z");
      break;

    case D3DRS_CCW_STENCILFAIL:
      if (Value != D3DSTENCILOP_KEEP)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_CCW_STENCILFAIL");
      break;

    case D3DRS_CCW_STENCILZFAIL:
      if (Value != D3DSTENCILOP_KEEP)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_CCW_STENCILZFAIL");
      break;

    case D3DRS_CCW_STENCILPASS:
      if (Value != D3DSTENCILOP_KEEP)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_CCW_STENCILPASS");
      break;

    case D3DRS_CCW_STENCILFUNC:
      if (Value != D3DCMP_ALWAYS)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_CCW_STENCILFUNC");
      break;

    case D3DRS_COLORWRITEENABLE1:
    case D3DRS_COLORWRITEENABLE2:
    case D3DRS_COLORWRITEENABLE3:
      if (Value != 0x0000000F)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: " + std::to_string(State));
      break;

    case D3DRS_BLENDFACTOR:
      if (Value != 0xFFFFFFFF)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_D3DRS_BLENDFACTOR");
      break;

    case D3DRS_SRCBLENDALPHA:
      if (Value != D3DBLEND_ONE)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_SRCBLENDALPHA");
      break;

    case D3DRS_DESTBLENDALPHA:
      if (Value != D3DBLEND_ZERO)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_DESTBLENDALPHA");
      break;

    // "If the D3DPMISCCAPS_BLENDOP device capability is not supported, then D3DBLENDOP_ADD is performed."
    case D3DRS_BLENDOPALPHA:
      if (Value != D3DBLENDOP_ADD) {
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: D3DRS_BLENDOPALPHA");
        return D3D_OK;
      }
      break;

    case D3DRS_DEPTHBIAS: {
      if (Value != 0) {
        static bool s_depthBiasInfoShown = false;

        if (!std::exchange(s_depthBiasInfoShown, true))
          Logger::info("D3D9Device::SetRenderState: Converting D3DRS_DEPTHBIAS to D3DRS_ZBIAS");
      }

      State8 = d3d8::D3DRS_ZBIAS;
      Value  = static_cast<DWORD>(bitcast<float>(Value) * D3D9TO8_ZBIAS_SCALE_INV);
      break;
    }

    case D3DRS_ANTIALIASEDLINEENABLE:
      State8 = d3d8::D3DRS_EDGEANTIALIAS;
      break;

    default:
      // Render states above D3DRS_NORMALORDER/D3DRS_NORMALDEGREE (173) don't exist in D3D8
      if (State > D3DRS_NORMALDEGREE && Value != 0)
        Logger::warn("D3D9Device::SetRenderState: Use of unsupported render state: " + std::to_string(State));
      break;
  }

  return m_d3d8->SetRenderState(State8, Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
  if (pValue == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DRENDERSTATETYPE State8 = d3d8::D3DRENDERSTATETYPE(State);

  switch (State) {
    case D3DRS_DEPTHBIAS: {
      static bool s_depthBiasInfoShown = false;

      if (!std::exchange(s_depthBiasInfoShown, true))
        Logger::info("D3D9Device::GetRenderState: Converting D3DRS_ZBIAS to D3DRS_DEPTHBIAS");

      DWORD zBias = 0u;
      m_d3d8->GetRenderState(d3d8::D3DRS_ZBIAS, &zBias);
      *pValue = bitcast<DWORD>(static_cast<float>(zBias) * D3D9TO8_ZBIAS_SCALE);
      return D3D_OK;
    }

    case D3DRS_ANTIALIASEDLINEENABLE:
      State8 = d3d8::D3DRS_EDGEANTIALIAS;
      break;

    default:
      // Render states above D3DRS_NORMALORDER/D3DRS_NORMALDEGREE (173) don't exist in D3D8
      if (State > D3DRS_NORMALDEGREE)
        Logger::debug("D3D9Device::GetRenderState: Use of unsupported render state: " + std::to_string(State));
      break;
  }

  return m_d3d8->GetRenderState(State8, pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateStateBlock(
        D3DSTATEBLOCKTYPE      Type,
        IDirect3DStateBlock9** ppSB) {
  if (unlikely(ppSB == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSB);

  DWORD handle;
  HRESULT hr = m_d3d8->CreateStateBlock(d3d8::D3DSTATEBLOCKTYPE(Type), &handle);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreateStateBlock: Failed to create D3D8 state block");
    return hr;
  }

  *ppSB = ref(new D3D9StateBlock(this, handle));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::BeginStateBlock() {
  return m_d3d8->BeginStateBlock();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EndStateBlock(IDirect3DStateBlock9** ppSB) {
  if (unlikely(ppSB == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppSB);

  DWORD handle;
  HRESULT hr = m_d3d8->EndStateBlock(&handle);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::EndStateBlock: Failed to end D3D8 state block");
    return hr;
  }

  *ppSB = ref(new D3D9StateBlock(this, handle));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) {
  return m_d3d8->SetClipStatus(reinterpret_cast<const d3d8::D3DCLIPSTATUS8*>(pClipStatus));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
  return m_d3d8->GetClipStatus(reinterpret_cast<d3d8::D3DCLIPSTATUS8*>(pClipStatus));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppTexture == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(Stage >= D3D9TO8_MAX_TEXTURE_STAGES))
    return D3DERR_INVALIDCALL;

  *ppTexture = m_textures[Stage].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(Stage >= D3D9TO8_MAX_TEXTURE_STAGES))
    return D3DERR_INVALIDCALL;

  D3D9Texture2D* baseTexture9 = reinterpret_cast<D3D9Texture2D*>(pTexture);

  HRESULT hr = m_d3d8->SetTexture(Stage, baseTexture9 != nullptr ? baseTexture9->GetD3D8BaseTexture() : nullptr);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetTexture: Failed to set D3D8 texture");
    return hr;
  }

  m_textures[Stage] = baseTexture9;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTextureStageState(
        DWORD                    Stage,
        D3DTEXTURESTAGESTATETYPE Type,
        DWORD*                   pValue) {
  // D3D8 doesn't support D3DTSS_CONSTANT (32)
  if (unlikely(Type == D3DTSS_CONSTANT)) {
    Logger::debug("D3D9Device::GetTextureStageState: Unsupported D3DTEXTURESTAGESTATETYPE: D3DTSS_CONSTANT");
    *pValue = 0u;
    return D3D_OK;
  }

  return m_d3d8->GetTextureStageState(Stage, d3d8::D3DTEXTURESTAGESTATETYPE(Type), pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTextureStageState(
        DWORD                    Stage,
        D3DTEXTURESTAGESTATETYPE Type,
        DWORD                    Value) {
  // D3D8 doesn't support D3DTSS_CONSTANT (32)
  if (unlikely(Type == D3DTSS_CONSTANT)) {
    if (Value != 0)
      Logger::warn("D3D9Device::SetTextureStageState: Unsupported D3DTEXTURESTAGESTATETYPE D3DTSS_CONSTANT value: "
                   + std::to_string(Value));
    return D3D_OK;
  }

  return m_d3d8->SetTextureStageState(Stage, d3d8::D3DTEXTURESTAGESTATETYPE(Type), Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetSamplerState(
        DWORD               Sampler,
        D3DSAMPLERSTATETYPE Type,
        DWORD*              pValue) {
  const d3d8::D3DTEXTURESTAGESTATETYPE d3d8Type = GetTextureStateType8(Type);

  // D3DSAMP_SRGBTEXTURE (11), D3DSAMP_ELEMENTINDEX (12) and D3DSAMP_DMAPOFFSET (13) don't exist in D3D8
  if (unlikely(d3d8Type == d3d8::D3DTEXTURESTAGESTATETYPE(-1))) {
    Logger::debug("D3D9Device::GetSamplerState: Unsupported D3DSAMPLERSTATETYPE: " + std::to_string(Type));
    *pValue = 0u;
    return D3D_OK;
  }

  return m_d3d8->GetTextureStageState(Sampler, d3d8Type, pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetSamplerState(
        DWORD               Sampler,
        D3DSAMPLERSTATETYPE Type,
        DWORD               Value) {
  const d3d8::D3DTEXTURESTAGESTATETYPE d3d8Type = GetTextureStateType8(Type);

  // D3DSAMP_SRGBTEXTURE (11), D3DSAMP_ELEMENTINDEX (12) and D3DSAMP_DMAPOFFSET (13) don't exist in D3D8
  if (unlikely(d3d8Type == d3d8::D3DTEXTURESTAGESTATETYPE(-1))) {
    if (Value != 0)
      Logger::warn("D3D9Device::SetSamplerState: Unsupported D3DSAMPLERSTATETYPE "
                  + std::to_string(Type) + " value: " + std::to_string(Value));
    return D3D_OK;
  }

  return m_d3d8->SetTextureStageState(Sampler, d3d8Type, Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::ValidateDevice(DWORD* pNumPasses) {
  return m_d3d8->ValidateDevice(pNumPasses);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) {
  return m_d3d8->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
  return m_d3d8->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetCurrentTexturePalette(UINT PaletteNumber) {
  return m_d3d8->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetCurrentTexturePalette(UINT *PaletteNumber) {
  return m_d3d8->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetScissorRect(const RECT* pRect) {
  if (unlikely(pRect == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::SetScissorRect: Unsupported call!");

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetScissorRect(RECT* pRect) {
  if (unlikely(pRect == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::GetScissorRect: Unsupported call!");

  RECT rect = { };
  *pRect = rect;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetSoftwareVertexProcessing(BOOL bSoftware) {
  if (unlikely(!m_canSWVP))
    return D3DERR_INVALIDCALL;

  return m_d3d8->SetRenderState(d3d8::D3DRS_SOFTWAREVERTEXPROCESSING, static_cast<DWORD>(bSoftware));
}

BOOL STDMETHODCALLTYPE D3D9Device::GetSoftwareVertexProcessing() {
  DWORD swvpD3D8 = 0u;
  m_d3d8->GetRenderState(d3d8::D3DRS_SOFTWAREVERTEXPROCESSING, &swvpD3D8);

  return static_cast<BOOL>(swvpD3D8);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetNPatchMode(float nSegments) {
  return m_d3d8->SetRenderState(d3d8::D3DRS_PATCHSEGMENTS, bitcast<DWORD>(nSegments));
}

float STDMETHODCALLTYPE D3D9Device::GetNPatchMode() {
  DWORD nPatchMode = 0u;
  m_d3d8->GetRenderState(d3d8::D3DRS_PATCHSEGMENTS, &nPatchMode);

  return bitcast<float>(nPatchMode);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawPrimitive(
        D3DPRIMITIVETYPE PrimitiveType,
        UINT             StartVertex,
        UINT             PrimitiveCount) {
  if (unlikely(!PrimitiveCount))
    return D3D_OK;

  return m_d3d8->DrawPrimitive(d3d8::D3DPRIMITIVETYPE(PrimitiveType), StartVertex, PrimitiveCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawIndexedPrimitive(
        D3DPRIMITIVETYPE PrimitiveType,
        INT              BaseVertexIndex,
        UINT             MinVertexIndex,
        UINT             NumVertices,
        UINT             StartIndex,
        UINT             PrimitiveCount) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(!PrimitiveCount || !NumVertices))
    return D3D_OK;

  if (unlikely(BaseVertexIndex < 0)) {
    Logger::err("D3D9Device::DrawIndexedPrimitive: Use of negative BaseVertexIndex");
    if (likely(!D3D9TO8_LENIENT_BVI_DRAWS))
      return D3DERR_INVALIDCALL;
  }

  if (likely(m_indices != nullptr)) {
    // We need to update the D3D8 BaseVertexIndex on each call, since the bound index buffer may change
    HRESULT hr = m_d3d8->SetIndices(m_indices->GetD3D8IndexBuffer(), static_cast<UINT>(BaseVertexIndex));
    if (unlikely(FAILED(hr))) {
      Logger::err("D3D9Device::DrawIndexedPrimitive: Failed to set new D3D8 BaseVertexIndex");
      return hr;
    }
  }

  return m_d3d8->DrawIndexedPrimitive(d3d8::D3DPRIMITIVETYPE(PrimitiveType), MinVertexIndex,
                                      NumVertices, StartIndex, PrimitiveCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawPrimitiveUP(
        D3DPRIMITIVETYPE PrimitiveType,
        UINT             PrimitiveCount,
  const void*            pVertexStreamZeroData,
        UINT             VertexStreamZeroStride) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(!PrimitiveCount))
    return D3D_OK;

  m_streamSource[0] = nullptr;
  m_streamSourceStride[0] = 0u;

  return m_d3d8->DrawPrimitiveUP(d3d8::D3DPRIMITIVETYPE(PrimitiveType), PrimitiveCount,
                                 pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawIndexedPrimitiveUP(
        D3DPRIMITIVETYPE PrimitiveType,
        UINT             MinVertexIndex,
        UINT             NumVertices,
        UINT             PrimitiveCount,
  const void*            pIndexData,
        D3DFORMAT        IndexDataFormat,
  const void*            pVertexStreamZeroData,
        UINT             VertexStreamZeroStride) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(!PrimitiveCount || !NumVertices))
    return D3D_OK;

  m_streamSource[0] = nullptr;
  m_streamSourceStride[0] = 0u;
  m_indices = nullptr;

  return m_d3d8->DrawIndexedPrimitiveUP(d3d8::D3DPRIMITIVETYPE(PrimitiveType), MinVertexIndex, NumVertices,
                                        PrimitiveCount, pIndexData, d3d8::D3DFORMAT(IndexDataFormat),
                                        pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT STDMETHODCALLTYPE D3D9Device::ProcessVertices(
        UINT                         SrcStartIndex,
        UINT                         DestIndex,
        UINT                         VertexCount,
        IDirect3DVertexBuffer9*      pDestBuffer,
        IDirect3DVertexDeclaration9* pVertexDecl,
        DWORD                        Flags) {
  // TODO: Temporarily switch to the pVertexDecl and back once done
  if (unlikely(pVertexDecl != nullptr))
    Logger::warn("D3D9Device::ProcessVertices: Use of non-null pVertexDecl");

  D3D9VertexBuffer* vertexBuffer9 = reinterpret_cast<D3D9VertexBuffer*>(pDestBuffer);

  return m_d3d8->ProcessVertices(SrcStartIndex, DestIndex, VertexCount,
                                 vertexBuffer9->GetD3D8VertexBuffer(), Flags);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexDeclaration(
  const D3DVERTEXELEMENT9*            pVertexElements,
        IDirect3DVertexDeclaration9** ppDecl) {
  if (unlikely(ppDecl == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppDecl);

  // We don't generate a D3D8 vertex shader here, as we need to consolidate
  // things on D3D8 side during SetVertexShader calls, while also using the
  // currently set vertex shader function (if it's not a FF shader)
  *ppDecl = ref(new D3D9VertexDecl(this, 0u, pVertexElements));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  D3D9VertexDecl* vertexDecl = reinterpret_cast<D3D9VertexDecl*>(pDecl);

  if (pDecl != nullptr) {
    // Fixed function vertex shader declaration
    if (m_vertexShader == nullptr) {
      if (vertexDecl->NeedsDefinitionUpdate(nullptr)) {
        ConvertD3D9Shader(vertexDecl->GetDeclaration8(), vertexDecl->GetDeclaration9(),
                          nullptr, nullptr);
        vertexDecl->SetFunctionOrigin(nullptr);
        vertexDecl->SetVSHandle(0u);
      }

      DWORD handle = vertexDecl->GetVSHandle();
      if (!handle) {
        HRESULT hr = m_d3d8->CreateVertexShader(vertexDecl->GetDeclaration8()->data(),
                                                nullptr,
                                                &handle, 0);
        if (unlikely(FAILED(hr))) {
          Logger::warn("D3D9Device::CreateVertexDeclaration: Failed to create D3D8 vertex shader");
          if (likely(!D3D9TO8_LENIENT_SHADERS))
            return hr;
        }
        vertexDecl->SetVSHandle(handle);
      }

      HRESULT hr = m_d3d8->SetVertexShader(handle);
      if (unlikely(FAILED(hr))) {
        Logger::warn("D3D9Device::SetVertexDeclaration: Failed to set D3D8 vertex shader");
        if (likely(!D3D9TO8_LENIENT_SHADERS))
          return hr;
      }
    } else {
      if (vertexDecl->NeedsDefinitionUpdate(m_vertexShader.ptr())) {
        ConvertD3D9Shader(vertexDecl->GetDeclaration8(), vertexDecl->GetDeclaration9(),
                          m_vertexShader->GetFunction8(), m_vertexShader->GetFunction9());
        vertexDecl->SetFunctionOrigin(m_vertexShader.ptr());
        m_vertexShader->SetDeclarationOrigin(vertexDecl);
        m_vertexShader->SetVSHandle(0u);
      }

      DWORD handle = m_vertexShader->GetVSHandle();
      if (!handle) {
        HRESULT hr = m_d3d8->CreateVertexShader(vertexDecl->GetDeclaration8()->data(),
                                                m_vertexShader->GetFunction8()->data(),
                                                &handle, 0);
        if (unlikely(FAILED(hr))) {
          Logger::warn("D3D9Device::SetVertexDeclaration: Failed to create D3D8 vertex shader");
          if (likely(!D3D9TO8_LENIENT_SHADERS))
            return hr;
        }
        m_vertexShader->SetVSHandle(handle);
      }

      HRESULT hr = m_d3d8->SetVertexShader(handle);
      if (unlikely(FAILED(hr))) {
        Logger::warn("D3D9Device::SetVertexDeclaration: Failed to set D3D8 vertex shader");
        if (likely(!D3D9TO8_LENIENT_SHADERS))
          return hr;
      }
    }
  } else {
    // Restore either the current FVF (even if 0)
    m_d3d8->SetVertexShader(m_fvf);
  }

  m_vertexDecl = vertexDecl;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppDecl == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppDecl);

  *ppDecl = m_vertexDecl.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetFVF(DWORD FVF) {
  HRESULT hr = m_d3d8->SetVertexShader(FVF);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetFVF: Failed to set D3D8 FVF");
    return hr;
  }

  m_fvf = FVF;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetFVF(DWORD* pFVF) {
  if (unlikely(pFVF == nullptr))
    return D3DERR_INVALIDCALL;

  // The D3D8 side FVF may be overwritten by declaration/shader handles
  *pFVF = m_fvf;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexShader(
  const DWORD*                   pFunction,
        IDirect3DVertexShader9** ppShader) {
  if (unlikely(pFunction == nullptr || ppShader == nullptr))
    return D3DERR_INVALIDCALL;

  const uint32_t majorVersion = D3DSHADER_VERSION_MAJOR(pFunction[0]);
  const uint32_t minorVersion = D3DSHADER_VERSION_MINOR(pFunction[0]);

  if (majorVersion > 1 || (majorVersion == 1 && minorVersion > 1)) {
    Logger::err("D3D9Device::CreateVertexShader: Unsupported VS version " + std::to_string(majorVersion)
                                                                    + "." + std::to_string(minorVersion));
    if (likely(!D3D9TO8_LENIENT_SHADERS))
      return D3DERR_INVALIDCALL;
  }

  // We don't generate a D3D8 vertex shader here, as we need to consolidate
  // things on D3D8 side during SetVertexShader calls, while also using the
  // currently set vertex shader definition
  *ppShader = ref(new D3D9VertexShader(this, 0u, pFunction));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShader(IDirect3DVertexShader9* pShader) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  D3D9VertexShader* vertexShader9 = reinterpret_cast<D3D9VertexShader*>(pShader);

  if (pShader != nullptr) {
    // Defer any operation until we have a valid m_vertexDecl set,
    // otherwise a non-FF vertex shader creation will fail in D3D8
    if (likely(m_vertexDecl != nullptr)) {
      if (vertexShader9->NeedsFunctionUpdate(m_vertexDecl.ptr())) {
        ConvertD3D9Shader(m_vertexDecl->GetDeclaration8(), m_vertexDecl->GetDeclaration9(),
                          vertexShader9->GetFunction8(), vertexShader9->GetFunction9());
        m_vertexDecl->SetFunctionOrigin(vertexShader9);
        vertexShader9->SetDeclarationOrigin(m_vertexDecl.ptr());
        vertexShader9->SetVSHandle(0u);
      }

      DWORD handle = vertexShader9->GetVSHandle();
      if (!handle) {
        HRESULT hr = m_d3d8->CreateVertexShader(m_vertexDecl->GetDeclaration8()->data(),
                                                vertexShader9->GetFunction8()->data(),
                                                &handle, 0);
        if (unlikely(FAILED(hr))) {
          Logger::warn("D3D9Device::SetVertexShader: Failed to create D3D8 vertex shader");
          if (likely(!D3D9TO8_LENIENT_SHADERS))
            return hr;
        }
        vertexShader9->SetVSHandle(handle);
      }

      HRESULT hr = m_d3d8->SetVertexShader(handle);
      if (unlikely(FAILED(hr))) {
        Logger::warn("D3D9Device::SetVertexShader: Failed to set D3D8 vertex shader");
        if (likely(!D3D9TO8_LENIENT_SHADERS))
          return hr;
      }
    }
  } else {
    // Revert to the fixed function declaration otherwise
    if (likely(m_fvf == 0u && m_vertexDecl != nullptr)) {
      if (m_vertexDecl->NeedsDefinitionUpdate(nullptr)) {
        ConvertD3D9Shader(m_vertexDecl->GetDeclaration8(), m_vertexDecl->GetDeclaration9(),
                          nullptr, nullptr);
        m_vertexDecl->SetFunctionOrigin(nullptr);
        m_vertexDecl->SetVSHandle(0u);
      }

      DWORD handle = m_vertexDecl->GetVSHandle();
      if (!handle) {
        HRESULT hr = m_d3d8->CreateVertexShader(m_vertexDecl->GetDeclaration8()->data(),
                                                nullptr,
                                                &handle, 0);
        if (unlikely(FAILED(hr))) {
          Logger::warn("D3D9Device::SetVertexShader: Failed to create D3D8 vertex shader");
          if (likely(!D3D9TO8_LENIENT_SHADERS))
            return hr;
        }
        m_vertexDecl->SetVSHandle(handle);
      }

      HRESULT hr = m_d3d8->SetVertexShader(handle);
      if (unlikely(FAILED(hr))) {
        Logger::warn("D3D9Device::SetVertexShader: Failed to set D3D8 vertex shader");
        if (likely(!D3D9TO8_LENIENT_SHADERS))
          return hr;
      }
    } else {
      // Restore either the current FVF (even if 0)
      m_d3d8->SetVertexShader(m_fvf);
    }
  }

  m_vertexShader = vertexShader9;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShader(IDirect3DVertexShader9** ppShader) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppShader == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppShader);

  *ppShader = m_vertexShader.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantF(
        UINT   StartRegister,
  const float* pConstantData,
        UINT   Vector4fCount) {
  if (unlikely(!Vector4fCount))
    return D3D_OK;

  HRESULT hr = m_d3d8->SetVertexShaderConstant(StartRegister, reinterpret_cast<const void*>(pConstantData), Vector4fCount);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetVertexShaderConstantF: Failed to set D3D8 constant");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantF(
        UINT   StartRegister,
        float* pConstantData,
        UINT   Vector4fCount) {
  if (unlikely(!Vector4fCount))
    return D3D_OK;

  HRESULT hr = m_d3d8->GetVertexShaderConstant(StartRegister, reinterpret_cast<void*>(pConstantData), Vector4fCount);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::GetVertexShaderConstantF: Failed to get D3D8 constant");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantI(
        UINT StartRegister,
  const int* pConstantData,
        UINT Vector4iCount) {
  if (unlikely(!Vector4iCount))
    return D3D_OK;

  Logger::err("D3D9Device::SetVertexShaderConstantI: Unsupported call!");
  if (likely(!D3D9TO8_LENIENT_SHADERS))
    return D3DERR_INVALIDCALL;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantI(
        UINT StartRegister,
        int* pConstantData,
        UINT Vector4iCount) {
  if (unlikely(!Vector4iCount))
    return D3D_OK;

  if (unlikely(pConstantData == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::GetVertexShaderConstantI: Unsupported call!");

  memset(pConstantData, 0, Vector4iCount * sizeof(int));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantB(
        UINT  StartRegister,
  const BOOL* pConstantData,
        UINT  BoolCount) {
  if (unlikely(!BoolCount))
    return D3D_OK;

  Logger::err("D3D9Device::SetVertexShaderConstantB: Unsupported call!");
  if (likely(!D3D9TO8_LENIENT_SHADERS))
    return D3DERR_INVALIDCALL;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantB(
        UINT  StartRegister,
        BOOL* pConstantData,
        UINT  BoolCount) {
  if (unlikely(!BoolCount))
    return D3D_OK;

  if (unlikely(pConstantData == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::GetVertexShaderConstantB: Unsupported call!");

  memset(pConstantData, 0, BoolCount * sizeof(BOOL));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetStreamSource(
        UINT                    StreamNumber,
        IDirect3DVertexBuffer9* pStreamData,
        UINT                    OffsetInBytes,
        UINT                    Stride) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(StreamNumber >= D3D9TO8_MAX_STREAMS))
    return D3DERR_INVALIDCALL;

  D3D9VertexBuffer* vertexBuffer9 = reinterpret_cast<D3D9VertexBuffer*>(pStreamData);

  if (unlikely(OffsetInBytes != 0))
    Logger::err("D3D9Device::SetStreamSource: Unsupported use of non-zero OffsetInBytes");

  HRESULT hr = m_d3d8->SetStreamSource(StreamNumber,
                                       vertexBuffer9 != nullptr ? vertexBuffer9->GetD3D8VertexBuffer() : nullptr,
                                       Stride);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetStreamSource: Failed to set the D3D8 stream source");
    return hr;
  }

  m_streamSource[StreamNumber] = vertexBuffer9;
  m_streamSourceStride[StreamNumber] = Stride;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetStreamSource(
        UINT                     StreamNumber,
        IDirect3DVertexBuffer9** ppStreamData,
        UINT*                    pOffsetInBytes,
        UINT*                    pStride) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppStreamData == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppStreamData);

  if (unlikely(StreamNumber >= D3D9TO8_MAX_STREAMS))
    return D3DERR_INVALIDCALL;

  if (pOffsetInBytes != nullptr)
    *pOffsetInBytes = 0u;

  if (pStride != nullptr)
    *pStride = m_streamSourceStride[StreamNumber];

  *ppStreamData = m_streamSource[StreamNumber].ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) {
  Logger::err("D3D9Device::SetStreamSourceFreq: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) {
  if (unlikely(pSetting == nullptr))
    return D3DERR_INVALIDCALL;

  if (unlikely(StreamNumber >= D3D9TO8_MAX_STREAMS))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::SetStreamSourceFreq: Unsupported call!");

  *pSetting = 0u;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  D3D9IndexBuffer* d3d9IndexBuffer = reinterpret_cast<D3D9IndexBuffer*>(pIndexData);

  HRESULT hr = m_d3d8->SetIndices(d3d9IndexBuffer != nullptr ? d3d9IndexBuffer->GetD3D8IndexBuffer() : nullptr, 0u);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetIndices: Failed to set D3D8 indices");
    return hr;
  }

  m_indices = d3d9IndexBuffer;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppIndexData == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppIndexData);

  *ppIndexData = m_indices.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreatePixelShader(
  const DWORD*                  pFunction,
        IDirect3DPixelShader9** ppShader) {
  if (unlikely(pFunction == nullptr || ppShader == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppShader);

  const uint32_t majorVersion = D3DSHADER_VERSION_MAJOR(pFunction[0]);
  const uint32_t minorVersion = D3DSHADER_VERSION_MINOR(pFunction[0]);

  if (majorVersion > 1 || (majorVersion == 1 && minorVersion > 4)) {
    Logger::err("D3D9Device::CreatePixelShader: Unsupported PS version " + std::to_string(majorVersion)
                                                                   + "." + std::to_string(minorVersion));
    if (likely(!D3D9TO8_LENIENT_SHADERS))
      return D3DERR_INVALIDCALL;
  }

  DWORD handle = 0u;
  HRESULT hr = m_d3d8->CreatePixelShader(pFunction, &handle);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::CreatePixelShader: Failed to create D3D8 pixel shader");
    if (likely(!D3D9TO8_LENIENT_SHADERS))
      return hr;
  }

  *ppShader = ref(new D3D9PixelShader(this, handle, pFunction));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShader(IDirect3DPixelShader9* pShader) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  D3D9PixelShader* pixelShader9 = reinterpret_cast<D3D9PixelShader*>(pShader);

  if (pShader != nullptr) {
    HRESULT hr = m_d3d8->SetPixelShader(pixelShader9->GetPSHandle());
    if (unlikely(FAILED(hr))) {
      Logger::warn("D3D9Device::SetPixelShader: Failed to set D3D8 pixel shader");
      if (likely(!D3D9TO8_LENIENT_SHADERS))
        return hr;
    }
  } else {
    m_d3d8->SetPixelShader(0);
  }

  m_pixelShader = pixelShader9;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShader(IDirect3DPixelShader9** ppShader) {
  std::unique_lock<std::mutex> deviceLock(m_deviceLock, std::defer_lock);
  if (m_isMultitheaded)
    deviceLock.lock();

  if (unlikely(ppShader == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppShader);

  *ppShader = m_pixelShader.ref();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantF(
  UINT   StartRegister,
  const float* pConstantData,
  UINT   Vector4fCount) {
  if (unlikely(!Vector4fCount))
    return D3D_OK;

  HRESULT hr = m_d3d8->SetPixelShaderConstant(StartRegister, reinterpret_cast<const void*>(pConstantData), Vector4fCount);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::SetPixelShaderConstantF: Failed to set D3D8 constant");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantF(
  UINT   StartRegister,
  float* pConstantData,
  UINT   Vector4fCount) {
  if (unlikely(!Vector4fCount))
    return D3D_OK;

  HRESULT hr = m_d3d8->GetPixelShaderConstant(StartRegister, reinterpret_cast<void*>(pConstantData), Vector4fCount);
  if (unlikely(FAILED(hr))) {
    Logger::warn("D3D9Device::GetPixelShaderConstantF: Failed to set D3D8 constant");
    return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantI(
  UINT StartRegister,
  const int* pConstantData,
  UINT Vector4iCount) {
  if (unlikely(!Vector4iCount))
    return D3D_OK;

  Logger::err("D3D9Device::SetPixelShaderConstantI: Unsupported call!");
  if (likely(!D3D9TO8_LENIENT_SHADERS))
    return D3DERR_INVALIDCALL;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantI(
  UINT StartRegister,
  int* pConstantData,
  UINT Vector4iCount) {
  if (unlikely(!Vector4iCount))
    return D3D_OK;

  if (unlikely(pConstantData == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::GetPixelShaderConstantI: Unsupported call!");

  memset(pConstantData, 0, Vector4iCount * sizeof(int));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantB(
  UINT  StartRegister,
  const BOOL* pConstantData,
  UINT  BoolCount) {
  if (unlikely(!BoolCount))
    return D3D_OK;

  Logger::err("D3D9Device::SetPixelShaderConstantB: Unsupported call!");
  if (likely(!D3D9TO8_LENIENT_SHADERS))
    return D3DERR_INVALIDCALL;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantB(
  UINT  StartRegister,
  BOOL* pConstantData,
  UINT  BoolCount) {
  if (unlikely(!BoolCount))
    return D3D_OK;

  if (unlikely(pConstantData == nullptr))
    return D3DERR_INVALIDCALL;

  Logger::debug("D3D9Device::GetPixelShaderConstantB: Unsupported call!");

  memset(pConstantData, 0, BoolCount * sizeof(BOOL));

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawRectPatch(
        UINT               Handle,
  const float*             pNumSegs,
  const D3DRECTPATCH_INFO* pRectPatchInfo) {
  return m_d3d8->DrawRectPatch(Handle, pNumSegs, reinterpret_cast<const d3d8::D3DRECTPATCH_INFO*>(pRectPatchInfo));
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawTriPatch(
        UINT              Handle,
  const float*            pNumSegs,
  const D3DTRIPATCH_INFO* pTriPatchInfo) {
  return m_d3d8->DrawTriPatch(Handle, pNumSegs, reinterpret_cast<const d3d8::D3DTRIPATCH_INFO*>(pTriPatchInfo));
}

HRESULT STDMETHODCALLTYPE D3D9Device::DeletePatch(UINT Handle) {
  return m_d3d8->DeletePatch(Handle);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
  if (unlikely(ppQuery == nullptr))
    return D3DERR_INVALIDCALL;

  ClearReturnPointer(ppQuery);

  switch (Type) {
    // Supported, with a very crude implementation
    case D3DQUERYTYPE_VCACHE:
    case D3DQUERYTYPE_EVENT:
    case D3DQUERYTYPE_TIMESTAMP:
    case D3DQUERYTYPE_TIMESTAMPDISJOINT:
    case D3DQUERYTYPE_TIMESTAMPFREQ:
    case D3DQUERYTYPE_OCCLUSION:
      break;
    default:
      return D3DERR_NOTAVAILABLE;
  }

  *ppQuery = ref(new D3D9Query(this, Type));

  return D3D_OK;
}
