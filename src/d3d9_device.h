#pragma once

#include "d3d9_include.h"
#include "d3d9_com_object.h"
#include "d3d9_logger.h"
#include "d3d9_caps.h"
#include "d3d9_options.h"
#include "d3d9_util.h"

#include "d3d9_buffer.h"
#include "d3d9_surface.h"
#include "d3d9_shader.h"
#include "d3d9_texture.h"
#include "d3d9_vertex_declaration.h"

#include <mutex>
#include <utility>
#include <array>
#include <vector>

class D3D9Device final : public ComObjectClamp<IDirect3DDevice9> {

public:

  D3D9Device(
      IDirect3D9* intf,
      ComObject<d3d8::IDirect3DDevice8>&& d3d8Device,
      D3DPRESENT_PARAMETERS presentParams,
      DWORD behaviorFlags);

  ~D3D9Device();

  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

  HRESULT STDMETHODCALLTYPE TestCooperativeLevel();

  UINT    STDMETHODCALLTYPE GetAvailableTextureMem();

  HRESULT STDMETHODCALLTYPE EvictManagedResources();

  HRESULT STDMETHODCALLTYPE GetDirect3D(IDirect3D9** ppD3D9);

  HRESULT STDMETHODCALLTYPE GetDeviceCaps(D3DCAPS9* pCaps);

  HRESULT STDMETHODCALLTYPE GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode);

  HRESULT STDMETHODCALLTYPE GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters);

  HRESULT STDMETHODCALLTYPE SetCursorProperties(
          UINT               XHotSpot,
          UINT               YHotSpot,
          IDirect3DSurface9* pCursorBitmap);

  void    STDMETHODCALLTYPE SetCursorPosition(int X, int Y, DWORD Flags);

  BOOL    STDMETHODCALLTYPE ShowCursor(BOOL bShow);

  HRESULT STDMETHODCALLTYPE CreateAdditionalSwapChain(
          D3DPRESENT_PARAMETERS* pPresentationParameters,
          IDirect3DSwapChain9**  ppSwapChain);

  HRESULT STDMETHODCALLTYPE GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** pSwapChain);

  UINT    STDMETHODCALLTYPE GetNumberOfSwapChains();

  HRESULT STDMETHODCALLTYPE Reset(D3DPRESENT_PARAMETERS* pPresentationParameters);

  HRESULT STDMETHODCALLTYPE Present(
    const RECT* pSourceRect,
    const RECT* pDestRect, HWND hDestWindowOverride,
    const RGNDATA* pDirtyRegion);

  HRESULT STDMETHODCALLTYPE GetBackBuffer(
    UINT iSwapChain,
    UINT iBackBuffer,
    D3DBACKBUFFER_TYPE Type,
    IDirect3DSurface9** ppBackBuffer);

  HRESULT STDMETHODCALLTYPE GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus);

  HRESULT STDMETHODCALLTYPE SetDialogBoxMode(BOOL bEnableDialogs);

  void    STDMETHODCALLTYPE SetGammaRamp(
    UINT iSwapChain,
    DWORD Flags,
    const D3DGAMMARAMP* pRamp);

  void    STDMETHODCALLTYPE GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp);

  HRESULT STDMETHODCALLTYPE CreateTexture(
          UINT                Width,
          UINT                Height,
          UINT                Levels,
          DWORD               Usage,
          D3DFORMAT           Format,
          D3DPOOL             Pool,
          IDirect3DTexture9** ppTexture,
          HANDLE*             pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateVolumeTexture(
          UINT                      Width,
          UINT                      Height,
          UINT                      Depth,
          UINT                      Levels,
          DWORD                     Usage,
          D3DFORMAT                 Format,
          D3DPOOL                   Pool,
          IDirect3DVolumeTexture9** ppVolumeTexture,
          HANDLE*                   pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateCubeTexture(
        UINT                      EdgeLength,
          UINT                    Levels,
          DWORD                   Usage,
          D3DFORMAT               Format,
          D3DPOOL                 Pool,
          IDirect3DCubeTexture9** ppCubeTexture,
          HANDLE*                 pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateVertexBuffer(
          UINT                     Length,
          DWORD                    Usage,
          DWORD                    FVF,
          D3DPOOL                  Pool,
          IDirect3DVertexBuffer9** ppVertexBuffer,
          HANDLE*                  pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateIndexBuffer(
          UINT                    Length,
          DWORD                   Usage,
          D3DFORMAT               Format,
          D3DPOOL                 Pool,
          IDirect3DIndexBuffer9** ppIndexBuffer,
          HANDLE*                 pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateRenderTarget(
          UINT                Width,
          UINT                Height,
          D3DFORMAT           Format,
          D3DMULTISAMPLE_TYPE MultiSample,
          DWORD               MultisampleQuality,
          BOOL                Lockable,
          IDirect3DSurface9** ppSurface,
          HANDLE*             pSharedHandle);

  HRESULT STDMETHODCALLTYPE CreateDepthStencilSurface(
          UINT                Width,
          UINT                Height,
          D3DFORMAT           Format,
          D3DMULTISAMPLE_TYPE MultiSample,
          DWORD               MultisampleQuality,
          BOOL                Discard,
          IDirect3DSurface9** ppSurface,
          HANDLE*             pSharedHandle);

  HRESULT STDMETHODCALLTYPE UpdateSurface(
          IDirect3DSurface9* pSourceSurface,
    const RECT*              pSourceRect,
          IDirect3DSurface9* pDestinationSurface,
    const POINT*             pDestPoint);

  HRESULT STDMETHODCALLTYPE UpdateTexture(
          IDirect3DBaseTexture9* pSourceTexture,
          IDirect3DBaseTexture9* pDestinationTexture);

  HRESULT STDMETHODCALLTYPE GetRenderTargetData(
          IDirect3DSurface9* pRenderTarget,
          IDirect3DSurface9* pDestSurface);

  HRESULT STDMETHODCALLTYPE GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface);

  HRESULT STDMETHODCALLTYPE StretchRect(
          IDirect3DSurface9*   pSourceSurface,
    const RECT*                pSourceRect,
          IDirect3DSurface9*   pDestSurface,
    const RECT*                pDestRect,
          D3DTEXTUREFILTERTYPE Filter);

  HRESULT STDMETHODCALLTYPE ColorFill(
          IDirect3DSurface9* pSurface,
    const RECT*              pRect,
          D3DCOLOR           Color);

  HRESULT STDMETHODCALLTYPE CreateOffscreenPlainSurface(
    UINT Width,
    UINT Height,
    D3DFORMAT Format,
    D3DPOOL Pool,
    IDirect3DSurface9** ppSurface,
    HANDLE* pSharedHandle);

  HRESULT STDMETHODCALLTYPE SetRenderTarget(
          DWORD              RenderTargetIndex,
          IDirect3DSurface9* pRenderTarget);

  HRESULT STDMETHODCALLTYPE GetRenderTarget(
          DWORD               RenderTargetIndex,
          IDirect3DSurface9** ppRenderTarget);

  HRESULT STDMETHODCALLTYPE SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);

  HRESULT STDMETHODCALLTYPE GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface);

  HRESULT STDMETHODCALLTYPE BeginScene();

  HRESULT STDMETHODCALLTYPE EndScene();

  HRESULT STDMETHODCALLTYPE Clear(
          DWORD    Count,
    const D3DRECT* pRects,
          DWORD    Flags,
          D3DCOLOR Color,
          float    Z,
          DWORD    Stencil);

  HRESULT STDMETHODCALLTYPE SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX* pMatrix);

  HRESULT STDMETHODCALLTYPE GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix);

  HRESULT STDMETHODCALLTYPE MultiplyTransform(D3DTRANSFORMSTATETYPE TransformState, const D3DMATRIX* pMatrix);

  HRESULT STDMETHODCALLTYPE SetViewport(const D3DVIEWPORT9* pViewport);

  HRESULT STDMETHODCALLTYPE GetViewport(D3DVIEWPORT9* pViewport);

  HRESULT STDMETHODCALLTYPE SetMaterial(const D3DMATERIAL9* pMaterial);

  HRESULT STDMETHODCALLTYPE GetMaterial(D3DMATERIAL9* pMaterial);

  HRESULT STDMETHODCALLTYPE SetLight(DWORD Index, const D3DLIGHT9* pLight);

  HRESULT STDMETHODCALLTYPE GetLight(DWORD Index, D3DLIGHT9* pLight);

  HRESULT STDMETHODCALLTYPE LightEnable(DWORD Index, BOOL Enable);

  HRESULT STDMETHODCALLTYPE GetLightEnable(DWORD Index, BOOL* pEnable);

  HRESULT STDMETHODCALLTYPE SetClipPlane(DWORD Index, const float* pPlane);

  HRESULT STDMETHODCALLTYPE GetClipPlane(DWORD Index, float* pPlane);

  HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);

  HRESULT STDMETHODCALLTYPE GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue);

  HRESULT STDMETHODCALLTYPE CreateStateBlock(
          D3DSTATEBLOCKTYPE      Type,
          IDirect3DStateBlock9** ppSB);

  HRESULT STDMETHODCALLTYPE BeginStateBlock();

  HRESULT STDMETHODCALLTYPE EndStateBlock(IDirect3DStateBlock9** ppSB);

  HRESULT STDMETHODCALLTYPE SetClipStatus(const D3DCLIPSTATUS9* pClipStatus);

  HRESULT STDMETHODCALLTYPE GetClipStatus(D3DCLIPSTATUS9* pClipStatus);

  HRESULT STDMETHODCALLTYPE GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture);

  HRESULT STDMETHODCALLTYPE SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture);

  HRESULT STDMETHODCALLTYPE GetTextureStageState(
          DWORD                    Stage,
          D3DTEXTURESTAGESTATETYPE Type,
          DWORD*                   pValue);

  HRESULT STDMETHODCALLTYPE SetTextureStageState(
          DWORD                    Stage,
          D3DTEXTURESTAGESTATETYPE Type,
          DWORD                    Value);

  HRESULT STDMETHODCALLTYPE GetSamplerState(
          DWORD               Sampler,
          D3DSAMPLERSTATETYPE Type,
          DWORD*              pValue);

  HRESULT STDMETHODCALLTYPE SetSamplerState(
          DWORD               Sampler,
          D3DSAMPLERSTATETYPE Type,
          DWORD               Value);

  HRESULT STDMETHODCALLTYPE ValidateDevice(DWORD* pNumPasses);

  HRESULT STDMETHODCALLTYPE SetPaletteEntries(UINT PaletteNumber, const PALETTEENTRY* pEntries);

  HRESULT STDMETHODCALLTYPE GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries);

  HRESULT STDMETHODCALLTYPE SetCurrentTexturePalette(UINT PaletteNumber);

  HRESULT STDMETHODCALLTYPE GetCurrentTexturePalette(UINT *PaletteNumber);

  HRESULT STDMETHODCALLTYPE SetScissorRect(const RECT* pRect);

  HRESULT STDMETHODCALLTYPE GetScissorRect(RECT* pRect);

  HRESULT STDMETHODCALLTYPE SetSoftwareVertexProcessing(BOOL bSoftware);

  BOOL    STDMETHODCALLTYPE GetSoftwareVertexProcessing();

  HRESULT STDMETHODCALLTYPE SetNPatchMode(float nSegments);

  float   STDMETHODCALLTYPE GetNPatchMode();

  HRESULT STDMETHODCALLTYPE DrawPrimitive(
          D3DPRIMITIVETYPE PrimitiveType,
          UINT             StartVertex,
          UINT             PrimitiveCount);

  HRESULT STDMETHODCALLTYPE DrawIndexedPrimitive(
          D3DPRIMITIVETYPE PrimitiveType,
          INT              BaseVertexIndex,
          UINT             MinVertexIndex,
          UINT             NumVertices,
          UINT             StartIndex,
          UINT             PrimitiveCount);

  HRESULT STDMETHODCALLTYPE DrawPrimitiveUP(
          D3DPRIMITIVETYPE PrimitiveType,
          UINT             PrimitiveCount,
    const void*            pVertexStreamZeroData,
          UINT             VertexStreamZeroStride);

  HRESULT STDMETHODCALLTYPE DrawIndexedPrimitiveUP(
          D3DPRIMITIVETYPE PrimitiveType,
          UINT             MinVertexIndex,
          UINT             NumVertices,
          UINT             PrimitiveCount,
    const void*            pIndexData,
          D3DFORMAT        IndexDataFormat,
    const void*            pVertexStreamZeroData,
          UINT             VertexStreamZeroStride);

  HRESULT STDMETHODCALLTYPE ProcessVertices(
          UINT                         SrcStartIndex,
          UINT                         DestIndex,
          UINT                         VertexCount,
          IDirect3DVertexBuffer9*      pDestBuffer,
          IDirect3DVertexDeclaration9* pVertexDecl,
          DWORD                        Flags);

  HRESULT STDMETHODCALLTYPE CreateVertexDeclaration(
    const D3DVERTEXELEMENT9*            pVertexElements,
          IDirect3DVertexDeclaration9** ppDecl);

  HRESULT STDMETHODCALLTYPE SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl);

  HRESULT STDMETHODCALLTYPE GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl);

  HRESULT STDMETHODCALLTYPE SetFVF(DWORD FVF);

  HRESULT STDMETHODCALLTYPE GetFVF(DWORD* pFVF);

  HRESULT STDMETHODCALLTYPE CreateVertexShader(
    const DWORD*                   pFunction,
          IDirect3DVertexShader9** ppShader);

  HRESULT STDMETHODCALLTYPE SetVertexShader(IDirect3DVertexShader9* pShader);

  HRESULT STDMETHODCALLTYPE GetVertexShader(IDirect3DVertexShader9** ppShader);

  HRESULT STDMETHODCALLTYPE SetVertexShaderConstantF(
          UINT   StartRegister,
    const float* pConstantData,
          UINT   Vector4fCount);

  HRESULT STDMETHODCALLTYPE GetVertexShaderConstantF(
          UINT   StartRegister,
          float* pConstantData,
          UINT   Vector4fCount);

  HRESULT STDMETHODCALLTYPE SetVertexShaderConstantI(
          UINT StartRegister,
    const int* pConstantData,
          UINT Vector4iCount);

  HRESULT STDMETHODCALLTYPE GetVertexShaderConstantI(
          UINT StartRegister,
          int* pConstantData,
          UINT Vector4iCount);

  HRESULT STDMETHODCALLTYPE SetVertexShaderConstantB(
          UINT  StartRegister,
    const BOOL* pConstantData,
          UINT  BoolCount);

  HRESULT STDMETHODCALLTYPE GetVertexShaderConstantB(
          UINT  StartRegister,
          BOOL* pConstantData,
          UINT  BoolCount);

  HRESULT STDMETHODCALLTYPE SetStreamSource(
          UINT                    StreamNumber,
          IDirect3DVertexBuffer9* pStreamData,
          UINT                    OffsetInBytes,
          UINT                    Stride);

  HRESULT STDMETHODCALLTYPE GetStreamSource(
          UINT                     StreamNumber,
          IDirect3DVertexBuffer9** ppStreamData,
          UINT*                    pOffsetInBytes,
          UINT*                    pStride);

  HRESULT STDMETHODCALLTYPE SetStreamSourceFreq(UINT StreamNumber, UINT Setting);

  HRESULT STDMETHODCALLTYPE GetStreamSourceFreq(UINT StreamNumber, UINT* pSetting);

  HRESULT STDMETHODCALLTYPE SetIndices(IDirect3DIndexBuffer9* pIndexData);

  HRESULT STDMETHODCALLTYPE GetIndices(IDirect3DIndexBuffer9** ppIndexData);

  HRESULT STDMETHODCALLTYPE CreatePixelShader(
    const DWORD*                  pFunction,
          IDirect3DPixelShader9** ppShader);

  HRESULT STDMETHODCALLTYPE SetPixelShader(IDirect3DPixelShader9* pShader);

  HRESULT STDMETHODCALLTYPE GetPixelShader(IDirect3DPixelShader9** ppShader);

  HRESULT STDMETHODCALLTYPE SetPixelShaderConstantF(
          UINT   StartRegister,
    const float* pConstantData,
          UINT   Vector4fCount);

  HRESULT STDMETHODCALLTYPE GetPixelShaderConstantF(
          UINT   StartRegister,
          float* pConstantData,
          UINT   Vector4fCount);

  HRESULT STDMETHODCALLTYPE SetPixelShaderConstantI(
          UINT StartRegister,
    const int* pConstantData,
          UINT Vector4iCount);

  HRESULT STDMETHODCALLTYPE GetPixelShaderConstantI(
          UINT StartRegister,
          int* pConstantData,
          UINT Vector4iCount);

  HRESULT STDMETHODCALLTYPE SetPixelShaderConstantB(
          UINT  StartRegister,
    const BOOL* pConstantData,
          UINT  BoolCount);

  HRESULT STDMETHODCALLTYPE GetPixelShaderConstantB(
          UINT  StartRegister,
          BOOL* pConstantData,
          UINT  BoolCount);

  HRESULT STDMETHODCALLTYPE DrawRectPatch(
          UINT               Handle,
    const float*             pNumSegs,
    const D3DRECTPATCH_INFO* pRectPatchInfo);

  HRESULT STDMETHODCALLTYPE DrawTriPatch(
          UINT              Handle,
    const float*            pNumSegs,
    const D3DTRIPATCH_INFO* pTriPatchInfo);

  HRESULT STDMETHODCALLTYPE DeletePatch(UINT Handle);

  HRESULT STDMETHODCALLTYPE CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery);

  d3d8::IDirect3DDevice8* GetD3D8Device() const {
    return m_d3d8.ptr();
  }

  const D3DPRESENT_PARAMETERS* GetPresentParameters() const {
    return &m_presentParams;
  }

private:

  // Shaders and state blocks aren't counted as losable resources
  // by D3D8, but reset them anyway to remain consistent with D3D9
  inline void ClearCachedD3D8Objects() {
    m_presentParams.BackBufferCount = std::max(m_presentParams.BackBufferCount, 1u);

    m_renderTarget = nullptr;
    m_depthStencil = nullptr;

    m_autoDepthStencil = nullptr;

    m_backBuffers.clear();
    m_backBuffers.resize(m_presentParams.BackBufferCount);

    m_indices = nullptr;

    m_streamSource.fill(nullptr);
    m_streamSourceStride.fill(0u);

    m_textures.fill(nullptr);

    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_vertexDecl = nullptr;
  }

  inline void CacheD3D8ObjectsAndRestoreState() {
    for (UINT i = 0; i < m_presentParams.BackBufferCount; i++) {
      ComObject<d3d8::IDirect3DSurface8> backBuffer8;
      m_d3d8->GetBackBuffer(i, d3d8::D3DBACKBUFFER_TYPE_MONO, &backBuffer8);
      m_backBuffers[i] = new D3D9Surface(this, std::move(backBuffer8), nullptr);
    }

    ComObject<d3d8::IDirect3DSurface8> autoDepthStencil8;
    // This call will fail if the D3D8 device is created without
    // the EnableAutoDepthStencil presentation parameter set to TRUE.
    HRESULT hr = m_d3d8->GetDepthStencilSurface(&autoDepthStencil8);
    m_autoDepthStencil = FAILED(hr) ? nullptr : new D3D9Surface(this, std::move(autoDepthStencil8), nullptr);

    m_renderTarget = m_backBuffers[0];
    m_depthStencil = m_autoDepthStencil;

    // D3D8 will set this to 0.0f by default
    m_d3d8->SetRenderState(d3d8::D3DRS_POINTSIZE_MIN, bitcast<DWORD>(1.0f));
  }

  IDirect3D9*                                 m_intf;

  std::mutex                                  m_deviceLock;
  bool                                        m_isMultitheaded = false;

  ComObject<d3d8::IDirect3DDevice8>           m_d3d8;

  D3DPRESENT_PARAMETERS                       m_presentParams;

  ComObject<D3D9Surface, false>               m_renderTarget;
  ComObject<D3D9Surface, false>               m_depthStencil;

  ComObject<D3D9Surface, false>               m_autoDepthStencil;

  std::vector<ComObject<D3D9Surface, false>>  m_backBuffers;

  DWORD                                       m_vertexShaderHandle = 0u;
  ComObject<D3D9VertexDecl, false>            m_vertexDecl;
  ComObject<D3D9VertexShader, false>          m_vertexShader;

  ComObject<D3D9PixelShader, false>           m_pixelShader;

  ComObject<D3D9IndexBuffer, false>           m_indices;

  std::array<UINT, D3D9TO8_MAX_STREAMS>       m_streamSourceStride;
  std::array<ComObject<D3D9VertexBuffer, false>, D3D9TO8_MAX_STREAMS> m_streamSource;

  std::array<ComObject<D3D9Texture2D, false>, D3D9TO8_MAX_TEXTURE_STAGES> m_textures;

};
