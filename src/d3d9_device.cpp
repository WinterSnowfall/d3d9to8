#include "d3d9_device.h"

#include "d3d9_buffer.h"
#include "d3d9_interface.h"
#include "d3d9_surface.h"
#include "d3d9_swapchain.h"
#include "d3d9_shader.h"
#include "d3d9_texture.h"
#include "d3d9_query.h"
#include "d3d9_vertex_declaration.h"
#include "d3d9_stateblock.h"
#include "d3d9_util.h"

using Logger = ThreadSafeLogger;

D3D9Device::D3D9Device(d3d8::IDirect3DDevice8* d3d8Device)
  : m_d3d8             ( d3d8Device ) {
  m_textures.fill(nullptr);
}

D3D9Device::~D3D9Device() {
}

HRESULT STDMETHODCALLTYPE D3D9Device::QueryInterface(REFIID riid, void** ppvObject) {
  Logger::info("D3D9Device::QueryInterface:");

  if (ppvObject == nullptr)
    return E_POINTER;

  *ppvObject = nullptr;

  if (riid == __uuidof(IUnknown)
    || riid == __uuidof(IDirect3DDevice9)) {
    *ppvObject = this->IncrementRef();
    return S_OK;
  }

  Logger::warn("D3D9Device::QueryInterface: Unknown interface query");
  //Logger::warn(str::format(riid));
  return E_NOINTERFACE;
}

HRESULT STDMETHODCALLTYPE D3D9Device::TestCooperativeLevel() {
  Logger::info("D3D9Device::TestCooperativeLevel:");
  return m_d3d8->TestCooperativeLevel();
}

UINT STDMETHODCALLTYPE D3D9Device::GetAvailableTextureMem() {
  Logger::info("D3D9Device::GetAvailableTextureMem:");
  return m_d3d8->GetAvailableTextureMem();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EvictManagedResources() {
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDirect3D(IDirect3D9** ppD3D9) {
  Logger::info("D3D9Device::GetDirect3D:");

  if (ppD3D9 == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3D8* d3d8Intf;
  HRESULT hr = m_d3d8->GetDirect3D(&d3d8Intf);
  if (FAILED(hr))
    return hr;

  D3D9Interface* d3d9Intf = new D3D9Interface(d3d8Intf);
  *ppD3D9 = d3d9Intf->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDeviceCaps(D3DCAPS9* pCaps) {
  Logger::info("D3D9Interface::GetDeviceCaps:");

  if (pCaps == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::D3DCAPS8 caps8;
  HRESULT hr = m_d3d8->GetDeviceCaps(&caps8);
  if (FAILED(hr))
    return hr;

  ConvertCaps9(caps8, pCaps);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) {
  Logger::info("D3D9Device::GetDisplayMode:");
  return m_d3d8->GetDisplayMode(reinterpret_cast<d3d8::D3DDISPLAYMODE*>(pMode));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) {
  Logger::info("D3D9Device::GetCreationParameters:");
  return m_d3d8->GetCreationParameters(reinterpret_cast<d3d8::D3DDEVICE_CREATION_PARAMETERS*>(pParameters));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetCursorProperties(
        UINT               XHotSpot,
        UINT               YHotSpot,
        IDirect3DSurface9* pCursorBitmap) {
  Logger::info("D3D9Device::SetCursorProperties:");

  D3D9Surface* d3d9Surface = reinterpret_cast<D3D9Surface*>(pCursorBitmap);

  return m_d3d8->SetCursorProperties(XHotSpot, YHotSpot, d3d9Surface->GetD3D8Surface());
}

void STDMETHODCALLTYPE D3D9Device::SetCursorPosition(int X, int Y, DWORD Flags) {
  Logger::info("D3D9Device::SetCursorPosition:");
  m_d3d8->SetCursorPosition(X, Y, Flags);
}

BOOL STDMETHODCALLTYPE D3D9Device::ShowCursor(BOOL bShow) {
  Logger::info("D3D9Device::ShowCursor:");
  return m_d3d8->ShowCursor(bShow);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateAdditionalSwapChain(
        D3DPRESENT_PARAMETERS* pPresentationParameters,
        IDirect3DSwapChain9**  ppSwapChain) {
  Logger::warn("D3D9Device::CreateAdditionalSwapChain: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain) {
  Logger::warn("D3D9Device::GetSwapChain: Stub!");

  if (pSwapChain == nullptr)
    return D3DERR_INVALIDCALL;

  // TODO: Emulate the implicit swapchain by forwarding (some?) calls to the device
  if (iSwapChain == 0)
    Logger::warn("D3D9Device::GetSwapChain: Use of zero iSwapChain");

  D3D9SwapChain* d3d9SwapChain = new D3D9SwapChain(this, nullptr);
  *pSwapChain = d3d9SwapChain->IncrementRef();

  return D3D_OK;
}

UINT STDMETHODCALLTYPE D3D9Device::GetNumberOfSwapChains() {
  return 1;
}

HRESULT STDMETHODCALLTYPE D3D9Device::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) {
  Logger::info("D3D9Device::Reset:");

  d3d8::D3DPRESENT_PARAMETERS params8 = ConvertPresentParameters8(pPresentationParameters);

  HRESULT hr = m_d3d8->Reset(&params8);
  if (FAILED(hr))
    return hr;

  m_textures.fill(nullptr);

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::Present(
  const RECT*    pSourceRect,
  const RECT*    pDestRect,
        HWND     hDestWindowOverride,
  const RGNDATA* pDirtyRegion) {
  Logger::info("D3D9Device::Present:");
  return m_d3d8->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetBackBuffer(
        UINT                iSwapChain,
        UINT                iBackBuffer,
        D3DBACKBUFFER_TYPE  Type,
        IDirect3DSurface9** ppBackBuffer) {
  Logger::info("D3D9Device::GetBackBuffer:");

  if (ppBackBuffer == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DSurface8* d3d8BackBuffer;
  HRESULT hr = m_d3d8->GetBackBuffer(iBackBuffer, d3d8::D3DBACKBUFFER_TYPE(Type),
                                     &d3d8BackBuffer);
  if (FAILED(hr))
    return hr;

  D3D9Surface* d3d9BackBuffer = new D3D9Surface(this, d3d8BackBuffer);
  *ppBackBuffer = d3d9BackBuffer->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) {
  Logger::warn("D3D9Device::GetRasterStatus: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetDialogBoxMode(BOOL bEnableDialogs) {
  Logger::warn("D3D9Device::SetDialogBoxMode: Stub!");
  return D3D_OK;
}

void STDMETHODCALLTYPE D3D9Device::SetGammaRamp(
        UINT          iSwapChain,
        DWORD         Flags,
  const D3DGAMMARAMP* pRamp) {
  Logger::warn("D3D9Device::SetGammaRamp: Stub!");
}

void STDMETHODCALLTYPE D3D9Device::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) {
  Logger::warn("D3D9Device::GetGammaRamp: Stub!");
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
  Logger::info("D3D9Device::CreateTexture:");

  if (ppTexture == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DTexture8* d3d8Texture;
  HRESULT hr = m_d3d8->CreateTexture(Width, Height, Levels, Usage,
                                     d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                     &d3d8Texture);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateTexture: Failed to create D3D8 texture");
    return hr;
  }

  D3D9Texture2D* d3d9Texture = new D3D9Texture2D(this, d3d8Texture);
  *ppTexture = d3d9Texture->IncrementRef();

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
  Logger::warn("D3D9Device::CreateVolumeTexture: Stub!");
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
  Logger::info("D3D9Device::CreateCubeTexture:");

  if (ppCubeTexture == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DCubeTexture8* d3d8CubeTexture;
  HRESULT hr = m_d3d8->CreateCubeTexture(EdgeLength, Levels, Usage,
                                         d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                         &d3d8CubeTexture);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateCubeTexture: Failed to create D3D8 cube texture");
    return hr;
  }

  D3D9TextureCube* d3d9CubeTexture = new D3D9TextureCube(this, d3d8CubeTexture);
  *ppCubeTexture = d3d9CubeTexture->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexBuffer(
        UINT                     Length,
        DWORD                    Usage,
        DWORD                    FVF,
        D3DPOOL                  Pool,
        IDirect3DVertexBuffer9** ppVertexBuffer,
        HANDLE*                  pSharedHandle) {
  Logger::info("D3D9Device::CreateVertexBuffer:");

  if (ppVertexBuffer == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DVertexBuffer8* d3d8VertexBuffer;
  HRESULT hr = m_d3d8->CreateVertexBuffer(Length, Usage, FVF,
                                          d3d8::D3DPOOL(Pool),
                                          &d3d8VertexBuffer);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateVertexBuffer: Failed to create D3D8 vertex buffer");
    return hr;
  }

  D3D9VertexBuffer* d3d9VertexBuffer = new D3D9VertexBuffer(d3d8VertexBuffer);
  *ppVertexBuffer = d3d9VertexBuffer->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateIndexBuffer(
        UINT                    Length,
        DWORD                   Usage,
        D3DFORMAT               Format,
        D3DPOOL                 Pool,
        IDirect3DIndexBuffer9** ppIndexBuffer,
        HANDLE*                 pSharedHandle) {
  Logger::info("D3D9Device::CreateIndexBuffer:");

  if (ppIndexBuffer == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DIndexBuffer8* d3d8IndexBuffer;
  HRESULT hr = m_d3d8->CreateIndexBuffer(Length, Usage,
                                         d3d8::D3DFORMAT(Format), d3d8::D3DPOOL(Pool),
                                         &d3d8IndexBuffer);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateIndexBuffer: Failed to create D3D8 index buffer");
    return hr;
  }

  D3D9IndexBuffer* d3d9IndexBuffer = new D3D9IndexBuffer(d3d8IndexBuffer);
  *ppIndexBuffer = d3d9IndexBuffer->IncrementRef();

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
  Logger::warn("D3D9Device::CreateRenderTarget: Stub!");
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
  Logger::info("D3D9Device::CreateDepthStencilSurface:");

  if (ppSurface == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DSurface8* d3d8DepthStencil;
  HRESULT hr = m_d3d8->CreateDepthStencilSurface(Width, Height,
                                                 d3d8::D3DFORMAT(Format),
                                                 d3d8::D3DMULTISAMPLE_TYPE(MultiSample),
                                                 &d3d8DepthStencil);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateDepthStencilSurface: Failed to create D3D8 depth stencil surface");
    return hr;
  }

  D3D9Surface* d3d9DepthStencil = new D3D9Surface(this, d3d8DepthStencil);
  *ppSurface = d3d9DepthStencil->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::UpdateSurface(
        IDirect3DSurface9* pSourceSurface,
  const RECT*              pSourceRect,
        IDirect3DSurface9* pDestinationSurface,
  const POINT*             pDestPoint) {
  Logger::info("D3D9Device::UpdateSurface:");

  D3D9Surface* sourceSurface9 = reinterpret_cast<D3D9Surface*>(pSourceSurface);
  D3D9Surface* destinationSurface9 = reinterpret_cast<D3D9Surface*>(pDestinationSurface);

  return m_d3d8->CopyRects(sourceSurface9->GetD3D8Surface(), pSourceRect, 1,
                           destinationSurface9->GetD3D8Surface(), pDestPoint);
}

HRESULT STDMETHODCALLTYPE D3D9Device::UpdateTexture(
        IDirect3DBaseTexture9* pSourceTexture,
        IDirect3DBaseTexture9* pDestinationTexture) {
  Logger::info("D3D9Device::UpdateTexture:");

  d3d8::IDirect3DBaseTexture8* sourceTexture8 = nullptr;
  d3d8::IDirect3DBaseTexture8* destinationTexture8 = nullptr;

  if (pSourceTexture != nullptr) {
    const D3DRESOURCETYPE sourceTextureType = pSourceTexture->GetType();
    switch (sourceTextureType) {
      default:
      case D3DRTYPE_TEXTURE: {
        D3D9Texture2D* sourceTexture9 = reinterpret_cast<D3D9Texture2D*>(pSourceTexture);
        sourceTexture8 = sourceTexture9->GetD3D8Texture();
        break;
      }
      case D3DRTYPE_CUBETEXTURE: {
        D3D9TextureCube* sourceCubeTexture9 = reinterpret_cast<D3D9TextureCube*>(pSourceTexture);
        sourceTexture8 = sourceCubeTexture9->GetD3D8CubeTexture();
        break;
      }
    }
  }

  if (pDestinationTexture != nullptr) {
    const D3DRESOURCETYPE destinationTextureType = pDestinationTexture->GetType();
    switch (destinationTextureType) {
      default:
      case D3DRTYPE_TEXTURE: {
        D3D9Texture2D* destinationTexture9 = reinterpret_cast<D3D9Texture2D*>(pDestinationTexture);
        destinationTexture8 = destinationTexture9->GetD3D8Texture();
        break;
      }
      case D3DRTYPE_CUBETEXTURE: {
        D3D9TextureCube* destinationCubeTexture9 = reinterpret_cast<D3D9TextureCube*>(pDestinationTexture);
        destinationTexture8 = destinationCubeTexture9->GetD3D8CubeTexture();
        break;
      }
    }
  }

  return m_d3d8->UpdateTexture(sourceTexture8, destinationTexture8);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderTargetData(
        IDirect3DSurface9* pRenderTarget,
        IDirect3DSurface9* pDestSurface) {
  // TODO: Copy the content with locks, if the surface is lockable
  Logger::warn("D3D9Device::GetRenderTargetData: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) {
  Logger::warn("D3D9Device::GetFrontBufferData: Stub!");
  // TODO: GetFrontBuffer exists in D3D8, but it doesn't copy to pDestSurface
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::StretchRect(
        IDirect3DSurface9*   pSourceSurface,
  const RECT*                pSourceRect,
        IDirect3DSurface9*   pDestSurface,
  const RECT*                pDestRect,
        D3DTEXTUREFILTERTYPE Filter) {
  Logger::err("D3D9Device::StretchRect: Unsupported call!");
  return D3DERR_NOTAVAILABLE;
}

HRESULT STDMETHODCALLTYPE D3D9Device::ColorFill(
        IDirect3DSurface9* pSurface,
  const RECT*              pRect,
        D3DCOLOR           Color) {
  // TODO: Implement with viewport (color) clears
  Logger::warn("D3D9Device::ColorFill: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateOffscreenPlainSurface(
  UINT Width,
  UINT Height,
  D3DFORMAT Format,
  D3DPOOL Pool,
  IDirect3DSurface9** ppSurface,
  HANDLE* pSharedHandle) {
  Logger::info("D3D9Device::CreateOffscreenPlainSurface:");

  if (ppSurface == nullptr)
    return D3DERR_INVALIDCALL;

  d3d8::IDirect3DSurface8* d3d8OffscreenSurface;
  HRESULT hr = m_d3d8->CreateImageSurface(Width, Height,
                                          d3d8::D3DFORMAT(Format),
                                          &d3d8OffscreenSurface);
  if (FAILED(hr)) {
    Logger::warn("D3D9Device::CreateOffscreenPlainSurface: Failed to create D3D8 offscreen surface");
    return hr;
  }

  D3D9Surface* d3d9OffscreenSurface = new D3D9Surface(this, d3d8OffscreenSurface);
  *ppSurface = d3d9OffscreenSurface->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetRenderTarget(
        DWORD              RenderTargetIndex,
        IDirect3DSurface9* pRenderTarget) {
  Logger::info("D3D9Device::SetRenderTarget:");

  if (m_rt != nullptr && m_rt == pRenderTarget)
    return D3D_OK;

  D3D9Surface* d3d9RenderTarget = reinterpret_cast<D3D9Surface*>(pRenderTarget);

  HRESULT hr = m_d3d8->SetRenderTarget(d3d9RenderTarget != nullptr ? d3d9RenderTarget->GetD3D8Surface() : nullptr, nullptr);
  if (FAILED(hr))
    return hr;

  m_rt = pRenderTarget;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderTarget(
        DWORD               RenderTargetIndex,
        IDirect3DSurface9** ppRenderTarget) {
  Logger::info("D3D9Device::GetRenderTarget: ");

  if (ppRenderTarget == nullptr)
    return D3DERR_INVALIDCALL;

  if (m_rt != nullptr) {
    *ppRenderTarget = m_rt;
    return D3D_OK;
  }

  d3d8::IDirect3DSurface8* d3d8RenderTarget;
  HRESULT hr = m_d3d8->GetRenderTarget(&d3d8RenderTarget);
  if (FAILED(hr))
    return hr;

  D3D9Surface* d3d9RenderTarget = new D3D9Surface(this, d3d8RenderTarget);
  *ppRenderTarget = d3d9RenderTarget->IncrementRef();

  return D3D_OK;
}


HRESULT STDMETHODCALLTYPE D3D9Device::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) {
  Logger::info("D3D9Device::SetDepthStencilSurface:");

  D3D9Surface* d3d9DepthStencil = reinterpret_cast<D3D9Surface*>(pNewZStencil);

  if (m_ds != nullptr && m_ds == pNewZStencil)
    return D3D_OK;

  HRESULT hr = m_d3d8->SetRenderTarget(nullptr, d3d9DepthStencil != nullptr ? d3d9DepthStencil->GetD3D8Surface() : nullptr);
  if (FAILED(hr))
    return hr;

  m_ds = pNewZStencil;

  return D3D_OK;
}


HRESULT STDMETHODCALLTYPE D3D9Device::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface) {
  Logger::info("D3D9Device::GetDepthStencilSurface:");

  if (ppZStencilSurface == nullptr)
    return D3DERR_INVALIDCALL;

  if (m_ds != nullptr) {
    *ppZStencilSurface = m_ds;
    return D3D_OK;
  }

  d3d8::IDirect3DSurface8* d3d8ZStencilSurface;
  HRESULT hr = m_d3d8->GetDepthStencilSurface(&d3d8ZStencilSurface);
  if (FAILED(hr))
    return hr;

  D3D9Surface* d3d9ZStencilSurface = new D3D9Surface(this, d3d8ZStencilSurface);
  *ppZStencilSurface = d3d9ZStencilSurface->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::BeginScene() {
  Logger::info("D3D9Device::BeginScene:");
  return m_d3d8->BeginScene();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EndScene() {
  Logger::info("D3D9Device::EndScene:");
  return m_d3d8->EndScene();
}

HRESULT STDMETHODCALLTYPE D3D9Device::Clear(
        DWORD    Count,
  const D3DRECT* pRects,
        DWORD    Flags,
        D3DCOLOR Color,
        float    Z,
        DWORD    Stencil) {
  Logger::info("D3D9Device::Clear:");
  return m_d3d8->Clear(Count, reinterpret_cast<const d3d8::D3DRECT*>(pRects), Flags, Color, Z, Stencil);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix) {
  Logger::info("D3D9Device::SetTransform:");
  return m_d3d8->SetTransform(d3d8::D3DTRANSFORMSTATETYPE(State), reinterpret_cast<const d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) {
  Logger::info("D3D9Device::GetTransform:");
  return m_d3d8->GetTransform(d3d8::D3DTRANSFORMSTATETYPE(State), reinterpret_cast<d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, const D3DMATRIX* pMatrix) {
  Logger::info("D3D9Device::MultiplyTransform:");
  return m_d3d8->MultiplyTransform(d3d8::D3DTRANSFORMSTATETYPE(TransformState), reinterpret_cast<const d3d8::D3DMATRIX*>(pMatrix));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetViewport(const D3DVIEWPORT9* pViewport) {
  Logger::info("D3D9Device::SetViewport:");
  return m_d3d8->SetViewport(reinterpret_cast<const d3d8::D3DVIEWPORT8*>(pViewport));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetViewport(D3DVIEWPORT9* pViewport) {
  Logger::info("D3D9Device::GetViewport:");
  return m_d3d8->GetViewport(reinterpret_cast<d3d8::D3DVIEWPORT8*>(pViewport));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetMaterial(const D3DMATERIAL9* pMaterial) {
  Logger::info("D3D9Device::SetMaterial:");
  return m_d3d8->SetMaterial(reinterpret_cast<const d3d8::D3DMATERIAL8*>(pMaterial));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetMaterial(D3DMATERIAL9* pMaterial) {
  Logger::info("D3D9Device::GetMaterial:");
  return m_d3d8->GetMaterial(reinterpret_cast<d3d8::D3DMATERIAL8*>(pMaterial));
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetLight(DWORD Index, const D3DLIGHT9* pLight) {
  Logger::info("D3D9Device::SetLight:");
  return m_d3d8->SetLight(Index, reinterpret_cast<const d3d8::D3DLIGHT8*>(pLight));
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetLight(DWORD Index, D3DLIGHT9* pLight) {
  Logger::info("D3D9Device::GetLight:");
  return m_d3d8->GetLight(Index, reinterpret_cast<d3d8::D3DLIGHT8*>(pLight));
}

HRESULT STDMETHODCALLTYPE D3D9Device::LightEnable(DWORD Index, BOOL Enable) {
  Logger::info("D3D9Device::LightEnable:");
  return m_d3d8->LightEnable(Index, Enable);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetLightEnable(DWORD Index, BOOL* pEnable) {
  Logger::info("D3D9Device::GetLightEnable:");
  return m_d3d8->GetLightEnable(Index, pEnable);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetClipPlane(DWORD Index, const float* pPlane) {
  Logger::warn("D3D9Device::SetClipPlane: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetClipPlane(DWORD Index, float* pPlane) {
  Logger::warn("D3D9Device::GetClipPlane: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) {
  Logger::info("D3D9Device::SetRenderState:");
  return m_d3d8->SetRenderState(d3d8::D3DRENDERSTATETYPE(State), Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) {
  Logger::info("D3D9Device::GetRenderState:");
  return m_d3d8->GetRenderState(d3d8::D3DRENDERSTATETYPE(State), pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateStateBlock(
        D3DSTATEBLOCKTYPE      Type,
        IDirect3DStateBlock9** ppSB) {
  Logger::info("D3D9Device::CreateStateBlock:");

  if (ppSB == nullptr)
    return D3DERR_INVALIDCALL;

  DWORD handle;
  HRESULT hr = m_d3d8->CreateStateBlock(d3d8::D3DSTATEBLOCKTYPE(Type), &handle);
  if (FAILED(hr))
    return hr;

  D3D9StateBlock* d3d9StateBlock = new D3D9StateBlock(this, handle);
  *ppSB = d3d9StateBlock->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::BeginStateBlock() {
  Logger::info("D3D9Device::BeginStateBlock:");
  return m_d3d8->BeginStateBlock();
}

HRESULT STDMETHODCALLTYPE D3D9Device::EndStateBlock(IDirect3DStateBlock9** ppSB) {
  Logger::info("D3D9Device::EndStateBlock:");

  if (ppSB == nullptr)
    return D3DERR_INVALIDCALL;

  DWORD handle;
  HRESULT hr = m_d3d8->EndStateBlock(&handle);
  if (FAILED(hr))
    return hr;

  D3D9StateBlock* d3d9StateBlock = new D3D9StateBlock(this, handle);
  *ppSB = d3d9StateBlock->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetClipStatus(const D3DCLIPSTATUS9* pClipStatus) {
  Logger::warn("D3D9Device::SetClipStatus: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) {
  Logger::warn("D3D9Device::GetClipStatus: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) {
  Logger::info("D3D9Device::GetTexture:");

  if (ppTexture == nullptr)
    return D3DERR_INVALIDCALL;

  if (m_textures[Stage] != nullptr) {
    *ppTexture = m_textures[Stage];
    return D3D_OK;
  }

  d3d8::IDirect3DBaseTexture8* texture8;
  HRESULT hr = m_d3d8->GetTexture(Stage, &texture8);
  if (FAILED(hr))
    return hr;

  if (texture8 != nullptr) {
    const d3d8::D3DRESOURCETYPE textureType = texture8->GetType();

    switch (textureType) {
      case D3DRTYPE_TEXTURE: {
        D3D9Texture2D* texture9 = new D3D9Texture2D(this, reinterpret_cast<d3d8::IDirect3DTexture8*>(texture8));
        *ppTexture = texture9->IncrementRef();
        break;
      }
      case D3DRTYPE_CUBETEXTURE: {
        D3D9TextureCube* cubeTexture9 = new D3D9TextureCube(this, reinterpret_cast<d3d8::IDirect3DCubeTexture8*>(texture8));
        *ppTexture = cubeTexture9->IncrementRef();
        break;
      }
      default:
        return D3DERR_INVALIDCALL;
    }
  } else {
    *ppTexture = nullptr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture) {
  Logger::info("D3D9Device::SetTexture:");

  if (m_textures[Stage] != nullptr && m_textures[Stage] == pTexture)
    return D3D_OK;

  if (pTexture != nullptr) {
    const D3DRESOURCETYPE textureType = pTexture->GetType();

    switch (textureType) {
      case D3DRTYPE_TEXTURE: {
        D3D9Texture2D* texture9 = reinterpret_cast<D3D9Texture2D*>(pTexture);

        HRESULT hr = m_d3d8->SetTexture(Stage, texture9->GetD3D8Texture());
        if (FAILED(hr))
          return hr;

        break;
      }
      case D3DRTYPE_CUBETEXTURE: {
        D3D9TextureCube* textureCube9 = reinterpret_cast<D3D9TextureCube*>(pTexture);

        HRESULT hr = m_d3d8->SetTexture(Stage, textureCube9->GetD3D8CubeTexture());
        if (FAILED(hr))
          return hr;

        break;
      }
      default:
        return D3DERR_INVALIDCALL;
    }
  } else {
    HRESULT hr = m_d3d8->SetTexture(Stage, nullptr);
    if (FAILED(hr))
      return hr;
  }

  m_textures[Stage] = pTexture;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetTextureStageState(
        DWORD                    Stage,
        D3DTEXTURESTAGESTATETYPE Type,
        DWORD*                   pValue) {
  Logger::info("D3D9Device::GetTextureStageState:");
  return m_d3d8->GetTextureStageState(Stage, d3d8::D3DTEXTURESTAGESTATETYPE(Type), pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetTextureStageState(
        DWORD                    Stage,
        D3DTEXTURESTAGESTATETYPE Type,
        DWORD                    Value) {
  return m_d3d8->SetTextureStageState(Stage, d3d8::D3DTEXTURESTAGESTATETYPE(Type), Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetSamplerState(
        DWORD               Sampler,
        D3DSAMPLERSTATETYPE Type,
        DWORD*              pValue) {
  Logger::info("D3D9Device::GetSamplerState:");

  const d3d8::D3DTEXTURESTAGESTATETYPE d3d8Type = GetTextureStateType8(Type);

  return m_d3d8->GetTextureStageState(Sampler, d3d8Type, pValue);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetSamplerState(
        DWORD               Sampler,
        D3DSAMPLERSTATETYPE Type,
        DWORD               Value) {
  Logger::info("D3D9Device::SetSamplerState:");

  const d3d8::D3DTEXTURESTAGESTATETYPE d3d8Type = GetTextureStateType8(Type);

  return m_d3d8->SetTextureStageState(Sampler, d3d8Type, Value);
}

HRESULT STDMETHODCALLTYPE D3D9Device::ValidateDevice(DWORD* pNumPasses) {
  Logger::info("D3D9Device::ValidateDevice:");
  return m_d3d8->ValidateDevice(pNumPasses);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries) {
  Logger::warn("D3D9Device::SetPaletteEntries: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) {
  Logger::warn("D3D9Device::GetPaletteEntries: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetCurrentTexturePalette(UINT PaletteNumber) {
  Logger::warn("D3D9Device::SetCurrentTexturePalette: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetCurrentTexturePalette(UINT *PaletteNumber) {
  Logger::warn("D3D9Device::GetCurrentTexturePalette: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetScissorRect(const RECT* pRect) {
  Logger::err("D3D9Device::SetScissorRect: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetScissorRect(RECT* pRect) {
  Logger::err("D3D9Device::GetScissorRect: Unsupported call!");

  if (pRect == nullptr)
    return D3DERR_INVALIDCALL;

  RECT rect = { };
  *pRect = rect;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetSoftwareVertexProcessing(BOOL bSoftware) {
  Logger::info("D3D9Device::SetSoftwareVertexProcessing:");
  return m_d3d8->SetRenderState(d3d8::D3DRS_SOFTWAREVERTEXPROCESSING, static_cast<DWORD>(bSoftware));
}

BOOL STDMETHODCALLTYPE D3D9Device::GetSoftwareVertexProcessing() {
  Logger::info("D3D9Device::GetSoftwareVertexProcessing:");

  DWORD swvpD3D8 = 0;
  m_d3d8->GetRenderState(d3d8::D3DRS_SOFTWAREVERTEXPROCESSING, &swvpD3D8);

  return static_cast<BOOL>(swvpD3D8);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetNPatchMode(float nSegments) {
  Logger::warn("D3D9Device::SetNPatchMode: Stub!");
  return D3D_OK;
}

float STDMETHODCALLTYPE D3D9Device::GetNPatchMode() {
  Logger::warn("D3D9Device::GetNPatchMode: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawPrimitive(
        D3DPRIMITIVETYPE PrimitiveType,
        UINT             StartVertex,
        UINT             PrimitiveCount) {
  Logger::info("D3D9Device::DrawPrimitive:");
  return m_d3d8->DrawPrimitive(d3d8::D3DPRIMITIVETYPE(PrimitiveType), StartVertex, PrimitiveCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawIndexedPrimitive(
        D3DPRIMITIVETYPE PrimitiveType,
        INT              BaseVertexIndex,
        UINT             MinVertexIndex,
        UINT             NumVertices,
        UINT             StartIndex,
        UINT             PrimitiveCount) {
  Logger::info("D3D9Device::DrawIndexedPrimitive:");

  UINT baseVertexIndex;
  d3d8::IDirect3DIndexBuffer8* indexBuffer8;
  HRESULT hr = m_d3d8->GetIndices(&indexBuffer8, &baseVertexIndex);
  if (FAILED(hr))
    return hr;

  if (BaseVertexIndex < 0)
    Logger::warn("D3D9Device::DrawIndexedPrimitive: Use of negative BaseVertexIndex");

  // BaseVertexIndex is set through SetIndices in D3D8
  if (baseVertexIndex != static_cast<UINT>(BaseVertexIndex))
    m_d3d8->SetIndices(indexBuffer8, BaseVertexIndex);

  return m_d3d8->DrawIndexedPrimitive(d3d8::D3DPRIMITIVETYPE(PrimitiveType), MinVertexIndex,
                                      NumVertices, StartIndex, PrimitiveCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawPrimitiveUP(
        D3DPRIMITIVETYPE PrimitiveType,
        UINT             PrimitiveCount,
  const void*            pVertexStreamZeroData,
        UINT             VertexStreamZeroStride) {
  Logger::info("D3D9Device::DrawPrimitiveUP:");
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
  Logger::info("D3D9Device::DrawIndexedPrimitiveUP:");
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
  Logger::warn("D3D9Device::ProcessVertices: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexDeclaration(
  const D3DVERTEXELEMENT9*            pVertexElements,
        IDirect3DVertexDeclaration9** ppDecl) {
  Logger::warn("D3D9Device::CreateVertexDeclaration: Stub!");

  if (ppDecl == nullptr)
    return D3DERR_INVALIDCALL;

  D3D9VertexDecl* d3d9VertexDecl = new D3D9VertexDecl();
  *ppDecl = d3d9VertexDecl->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) {
  Logger::warn("D3D9Device::SetVertexDeclaration: Stub!");
  // TODO: Use SetVertexShader along with the current existing VS?
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) {
  Logger::warn("D3D9Device::GetVertexDeclaration: Stub!");
  // TODO: Get it from the currently set VS
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetFVF(DWORD FVF) {
  Logger::info("D3D9Device::SetFVF:");
  return m_d3d8->SetVertexShader(FVF);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetFVF(DWORD* pFVF) {
  Logger::info("D3D9Device::GetFVF:");
  return m_d3d8->GetVertexShader(pFVF);
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateVertexShader(
  const DWORD*                   pFunction,
        IDirect3DVertexShader9** ppShader) {
  Logger::info("D3D9Device::CreateVertexShader:");

  if (pFunction == nullptr || ppShader == nullptr)
    return D3DERR_INVALIDCALL;

  const uint32_t majorVersion = D3DSHADER_VERSION_MAJOR(pFunction[0]);
  const uint32_t minorVersion = D3DSHADER_VERSION_MINOR(pFunction[0]);

  if (majorVersion > 1 || (majorVersion == 1 && minorVersion > 1)) {
    Logger::err("D3D9Device::CreateVertexShader: Unsupported VS version " + std::to_string(majorVersion)
                                                                    + "." + std::to_string(minorVersion));
    return D3DERR_INVALIDCALL;
  }

  DWORD handle;
  // TODO: Handle setting/updating the programmable VS declaration too
  HRESULT hr = m_d3d8->CreateVertexShader(nullptr, pFunction, &handle, 0);
  if (FAILED(hr))
    return hr;

  D3D9VertexShader* d3d9VSShader = new D3D9VertexShader(this, handle);
  *ppShader = d3d9VSShader->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShader(IDirect3DVertexShader9* pShader) {
  Logger::info("D3D9Device::SetVertexShader:");

  if (pShader == nullptr) {
    m_d3d8->SetVertexShader(0);
  } else {
    D3D9VertexShader* vertexShader9 = reinterpret_cast<D3D9VertexShader*>(pShader);

    HRESULT hr = m_d3d8->SetVertexShader(vertexShader9->GetD3D8VSHandle());
    if (FAILED(hr))
      return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShader(IDirect3DVertexShader9** ppShader) {
  Logger::warn("D3D9Device::GetVertexShader: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantF(
        UINT   StartRegister,
  const float* pConstantData,
        UINT   Vector4fCount) {
  Logger::info("D3D9Device::SetVertexShaderConstantF:");
  return m_d3d8->SetVertexShaderConstant(StartRegister, reinterpret_cast<const void*>(pConstantData), Vector4fCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantF(
        UINT   StartRegister,
        float* pConstantData,
        UINT   Vector4fCount) {
  Logger::info("D3D9Device::GetVertexShaderConstantF:");
  return m_d3d8->GetVertexShaderConstant(StartRegister, reinterpret_cast<void*>(pConstantData), Vector4fCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantI(
        UINT StartRegister,
  const int* pConstantData,
        UINT Vector4iCount) {
  Logger::err("D3D9Device::SetVertexShaderConstantI: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantI(
        UINT StartRegister,
        int* pConstantData,
        UINT Vector4iCount) {
  Logger::err("D3D9Device::GetVertexShaderConstantI: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetVertexShaderConstantB(
        UINT  StartRegister,
  const BOOL* pConstantData,
        UINT  BoolCount) {
  Logger::err("D3D9Device::SetVertexShaderConstantB: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetVertexShaderConstantB(
        UINT  StartRegister,
        BOOL* pConstantData,
        UINT  BoolCount) {
  Logger::warn("D3D9Device::GetVertexShaderConstantB: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetStreamSource(
        UINT                    StreamNumber,
        IDirect3DVertexBuffer9* pStreamData,
        UINT                    OffsetInBytes,
        UINT                    Stride) {
  Logger::info("D3D9Device::SetStreamSource:");

  D3D9VertexBuffer* vertexBuffer9 = reinterpret_cast<D3D9VertexBuffer*>(pStreamData);

  if (OffsetInBytes != 0)
    Logger::warn("D3D9Device::SetStreamSource: Non-zero OffsetInBytes");

  return m_d3d8->SetStreamSource(StreamNumber, vertexBuffer9 != nullptr ? vertexBuffer9->GetD3D8VertexBuffer() : nullptr, Stride);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetStreamSource(
        UINT                     StreamNumber,
        IDirect3DVertexBuffer9** ppStreamData,
        UINT*                    pOffsetInBytes,
        UINT*                    pStride) {
  Logger::warn("D3D9Device::GetStreamSource: Stub!");

  if (pOffsetInBytes != nullptr)
    *pOffsetInBytes = 0;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetStreamSourceFreq(UINT StreamNumber, UINT Setting) {
  Logger::err("D3D9Device::SetStreamSourceFreq: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting) {
  Logger::err("D3D9Device::SetStreamSourceFreq: Unsupported call!");

  if (pSetting == nullptr)
    return D3DERR_INVALIDCALL;

  *pSetting = 0;

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetIndices(IDirect3DIndexBuffer9* pIndexData) {
  Logger::info("D3D9Device::SetIndices");

  D3D9IndexBuffer* d3d9IndexBuffer = reinterpret_cast<D3D9IndexBuffer*>(pIndexData);

  return m_d3d8->SetIndices(d3d9IndexBuffer != nullptr ? d3d9IndexBuffer->GetD3D8IndexBuffer() : nullptr, 0);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetIndices(IDirect3DIndexBuffer9** ppIndexData) {
  Logger::warn("D3D9Device::GetIndices: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreatePixelShader(
  const DWORD*                  pFunction,
        IDirect3DPixelShader9** ppShader) {
  Logger::info("D3D9Device::CreatePixelShader:");

  if (pFunction == nullptr || ppShader == nullptr)
    return D3DERR_INVALIDCALL;

  const uint32_t majorVersion = D3DSHADER_VERSION_MAJOR(pFunction[0]);
  const uint32_t minorVersion = D3DSHADER_VERSION_MINOR(pFunction[0]);

  if (majorVersion > 1 || (majorVersion == 1 && minorVersion > 4)) {
    Logger::err("D3D9Device::CreatePixelShader: Unsupported PS version " + std::to_string(majorVersion)
                                                                   + "." + std::to_string(minorVersion));
    return D3DERR_INVALIDCALL;
  }

  DWORD handle;
  HRESULT hr = m_d3d8->CreatePixelShader(pFunction, &handle);
  if (FAILED(hr))
    return hr;

  D3D9PixelShader* d3d9PSShader = new D3D9PixelShader(this, handle);
  *ppShader = d3d9PSShader->IncrementRef();

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShader(IDirect3DPixelShader9* pShader) {
  Logger::info("D3D9Device::SetPixelShader:");

  if (pShader == nullptr) {
    m_d3d8->SetPixelShader(0);
  } else {
    D3D9PixelShader* pixelShader9 = reinterpret_cast<D3D9PixelShader*>(pShader);

    HRESULT hr = m_d3d8->SetPixelShader(pixelShader9->GetD3D8PSHandle());
    if (FAILED(hr))
      return hr;
  }

  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShader(IDirect3DPixelShader9** ppShader) {
  Logger::warn("D3D9Device::GetPixelShader: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantF(
  UINT   StartRegister,
  const float* pConstantData,
  UINT   Vector4fCount) {
  Logger::info("D3D9Device::SetPixelShaderConstantF:");
  return m_d3d8->SetPixelShaderConstant(StartRegister, reinterpret_cast<const void*>(pConstantData), Vector4fCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantF(
  UINT   StartRegister,
  float* pConstantData,
  UINT   Vector4fCount) {
  Logger::info("D3D9Device::GetPixelShaderConstantF:");
  return m_d3d8->GetPixelShaderConstant(StartRegister, reinterpret_cast<void*>(pConstantData), Vector4fCount);
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantI(
  UINT StartRegister,
  const int* pConstantData,
  UINT Vector4iCount) {
  Logger::err("D3D9Device::SetPixelShaderConstantI: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantI(
  UINT StartRegister,
  int* pConstantData,
  UINT Vector4iCount) {
  Logger::err("D3D9Device::GetPixelShaderConstantI: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::SetPixelShaderConstantB(
  UINT  StartRegister,
  const BOOL* pConstantData,
  UINT  BoolCount) {
  Logger::err("D3D9Device::SetPixelShaderConstantB: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::GetPixelShaderConstantB(
  UINT  StartRegister,
  BOOL* pConstantData,
  UINT  BoolCount) {
  Logger::err("D3D9Device::GetPixelShaderConstantB: Unsupported call!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawRectPatch(
        UINT               Handle,
  const float*             pNumSegs,
  const D3DRECTPATCH_INFO* pRectPatchInfo) {
  Logger::warn("D3D9Device::DrawRectPatch: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::DrawTriPatch(
        UINT              Handle,
  const float*            pNumSegs,
  const D3DTRIPATCH_INFO* pTriPatchInfo) {
  Logger::warn("D3D9Device::DrawTriPatch: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::DeletePatch(UINT Handle) {
  Logger::warn("D3D9Device::DeletePatch: Stub!");
  return D3D_OK;
}

HRESULT STDMETHODCALLTYPE D3D9Device::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery) {
  Logger::info("D3D9Device::CreateQuery:");

  if (ppQuery == nullptr)
    return D3DERR_INVALIDCALL;

  D3D9Query* d3d9Query = new D3D9Query(this, Type);
  *ppQuery = d3d9Query->IncrementRef();

  return D3D_OK;
}

