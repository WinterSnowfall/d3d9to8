#pragma once

#include <d3d9_options.h>

using Logger = ThreadSafeLogger;

// (9<-8) D3DCAPSX: Writes to D3DCAPS9 from D3DCAPS8
inline void ConvertCaps9(const d3d8::D3DCAPS8& caps8, D3DCAPS9* pCaps9) {
  memset(pCaps9, 0, sizeof(D3DCAPS9));
  memcpy(pCaps9, &caps8, sizeof(d3d8::D3DCAPS8));

  if (!D3D9TO8_LENIENT_SHADERS) {
    // ensure we report only D3D8-level VS/PS caps at best
    if (pCaps9->VertexShaderVersion > D3DVS_VERSION(1, 1))
      pCaps9->VertexShaderVersion = D3DVS_VERSION(1, 1);
    if (pCaps9->PixelShaderVersion > D3DPS_VERSION(1, 4))
      pCaps9->PixelShaderVersion  = D3DPS_VERSION(1, 4);
  } else {
    // fake report full SM3 support (NOT recommended)
    pCaps9->VertexShaderVersion = D3DVS_VERSION(3, 0);
    pCaps9->PixelShaderVersion  = D3DPS_VERSION(3, 0);

    // These caps are reported only by SM2+ capable GPUs
    pCaps9->VS20Caps.Caps                    = D3DVS20CAPS_PREDICATION;
    pCaps9->VS20Caps.DynamicFlowControlDepth = D3DVS20_MAX_DYNAMICFLOWCONTROLDEPTH;
    pCaps9->VS20Caps.NumTemps                = D3DVS20_MAX_NUMTEMPS;
    pCaps9->VS20Caps.StaticFlowControlDepth  = D3DVS20_MAX_STATICFLOWCONTROLDEPTH;

    pCaps9->PS20Caps.Caps                    = D3DPS20CAPS_ARBITRARYSWIZZLE
                                             | D3DPS20CAPS_GRADIENTINSTRUCTIONS
                                             | D3DPS20CAPS_PREDICATION
                                             | D3DPS20CAPS_NODEPENDENTREADLIMIT
                                             | D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT;
    pCaps9->PS20Caps.DynamicFlowControlDepth = D3DPS20_MAX_DYNAMICFLOWCONTROLDEPTH;
    pCaps9->PS20Caps.NumTemps                = D3DPS20_MAX_NUMTEMPS;
    pCaps9->PS20Caps.StaticFlowControlDepth  = D3DPS20_MAX_STATICFLOWCONTROLDEPTH;
    pCaps9->PS20Caps.NumInstructionSlots     = D3DPS20_MAX_NUMINSTRUCTIONSLOTS;

    // These caps are reported only by SM3 capable GPUs
    pCaps9->VertexTextureFilterCaps          = D3DPTFILTERCAPS_MINFPOINT
                                             | D3DPTFILTERCAPS_MINFLINEAR
                                             | D3DPTFILTERCAPS_MAGFPOINT
                                             | D3DPTFILTERCAPS_MAGFLINEAR;

    pCaps9->MaxVShaderInstructionsExecuted    = 4294967295;
    pCaps9->MaxPShaderInstructionsExecuted    = 4294967295;

    pCaps9->MaxVertexShader30InstructionSlots = 32768;
    pCaps9->MaxPixelShader30InstructionSlots  = 32768;
  }

  //
  // add a few D3D9 caps which are missing in D3D8
  //
  pCaps9->Caps2                 |= D3DCAPS2_CANAUTOGENMIPMAP;

  pCaps9->Caps3                 |= D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION
                                 | D3DCAPS3_COPY_TO_VIDMEM
                                 | D3DCAPS3_COPY_TO_SYSTEMMEM;

  pCaps9->PrimitiveMiscCaps     |= D3DPMISCCAPS_INDEPENDENTWRITEMASKS
                              // | D3DPMISCCAPS_PERSTAGECONSTANT // Doesn't actually exist in D3D8
                                 | D3DPMISCCAPS_FOGANDSPECULARALPHA
                                 | D3DPMISCCAPS_SEPARATEALPHABLEND
                                 | D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS
                                 | D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING
                                 | D3DPMISCCAPS_FOGVERTEXCLAMPED
                                 | D3DPMISCCAPS_POSTBLENDSRGBCONVERT;

  pCaps9->RasterCaps            |= D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS
                                 | D3DPRASTERCAPS_DEPTHBIAS
                              // | D3DPRASTERCAPS_SCISSORTEST // There's no D3D8 equivalent
                                 | D3DPRASTERCAPS_MULTISAMPLE_TOGGLE;

  pCaps9->SrcBlendCaps          |= D3DPBLENDCAPS_BLENDFACTOR;

  pCaps9->DestBlendCaps         |= D3DPBLENDCAPS_BLENDFACTOR;

  pCaps9->LineCaps              |= D3DLINECAPS_ANTIALIAS;

  pCaps9->StencilCaps           |= D3DSTENCILCAPS_TWOSIDED;

  pCaps9->VertexProcessingCaps  |= D3DVTXPCAPS_TEXGEN_SPHEREMAP;
  //
  //
  //

  //
  // remove D3D8 caps which are no longer present in D3D9
  //
  pCaps9->Caps2                 &= ~D3DCAPS2_CANRENDERWINDOWED;

  pCaps9->RasterCaps            &= ~D3DPRASTERCAPS_ZBIAS;
  //
  //
  //

  //
  // add all the remaining D3D9 caps, which are not present at all in D3D8
  //
  // none of the D3D9 DevCaps2 are supported/possible in D3D8
  pCaps9->DevCaps2                 = 0; //   D3DDEVCAPS2_STREAMOFFSET
                                        // | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES
                                        // | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET;

  pCaps9->MaxNpatchTessellationLevel = 0.0f;
  pCaps9->Reserved5                  = 0;
  pCaps9->MasterAdapterOrdinal       = 0;
  pCaps9->AdapterOrdinalInGroup      = 0;
  pCaps9->NumberOfAdaptersInGroup    = 1;

  pCaps9->DeclTypes                  = D3DDTCAPS_UBYTE4
                                     | D3DDTCAPS_UBYTE4N
                                     | D3DDTCAPS_SHORT2N
                                     | D3DDTCAPS_SHORT4N
                                     | D3DDTCAPS_USHORT2N
                                     | D3DDTCAPS_USHORT4N
                                     | D3DDTCAPS_UDEC3
                                     | D3DDTCAPS_DEC3N
                                     | D3DDTCAPS_FLOAT16_2
                                     | D3DDTCAPS_FLOAT16_4;

  // D3D8 doesn't support multiple simultaneous render targets
  pCaps9->NumSimultaneousRTs         = 1;

  // D3D8 doesn't support StretchRect, but report these anyway
  pCaps9->StretchRectFilterCaps      = D3DPTFILTERCAPS_MINFPOINT
                                     | D3DPTFILTERCAPS_MINFLINEAR
                                     | D3DPTFILTERCAPS_MAGFPOINT
                                     | D3DPTFILTERCAPS_MAGFLINEAR;
  //
  //
  //
}

// (8<-9) D3DD3DPRESENT_PARAMETERS: Returns D3D8's params given an input for D3D9
inline d3d8::D3DPRESENT_PARAMETERS ConvertPresentParameters8(D3DPRESENT_PARAMETERS* pParams) {
  // A 0 back buffer count needs to be corrected and made visible to the D3D9 application as well
  pParams->BackBufferCount = std::max(pParams->BackBufferCount, 1u);

  // In D3D8 D3DSWAPEFFECT_COPY(_VSYNC) can't be used with more than one back buffer
  if (pParams->SwapEffect == D3DSWAPEFFECT_COPY && pParams->BackBufferCount > 1)
    pParams->BackBufferCount = 1;

  d3d8::D3DPRESENT_PARAMETERS params;
  //Logger::debug("pParams->BackBufferWidth: " + std::to_string(pParams->BackBufferWidth));
  params.BackBufferWidth = pParams->BackBufferWidth;
  //Logger::debug("pParams->BackBufferHeight: " + std::to_string(pParams->BackBufferHeight));
  params.BackBufferHeight = pParams->BackBufferHeight;
  //Logger::debug("pParams->BackBufferFormat: " + std::to_string(pParams->BackBufferFormat));
  params.BackBufferFormat = d3d8::D3DFORMAT(pParams->BackBufferFormat);
  //Logger::debug("pParams->BackBufferCount: " + std::to_string(pParams->BackBufferCount));
  params.BackBufferCount = pParams->BackBufferCount;

  //Logger::debug("pParams->MultiSampleType: " + std::to_string(pParams->MultiSampleType));
  params.MultiSampleType = d3d8::D3DMULTISAMPLE_TYPE(pParams->MultiSampleType);

  //Logger::debug("pParams->SwapEffect: " + std::to_string(pParams->SwapEffect));
  // Remap D3DSWAPEFFECT_COPY to D3DSWAPEFFECT_COPY_VSYNC in D3D8
  // if any VSYNC specific D3DPRESENT_INTERVAL values are used
  if (pParams->SwapEffect == D3DSWAPEFFECT_COPY &&
      pParams->PresentationInterval != D3DPRESENT_INTERVAL_IMMEDIATE) {
    params.SwapEffect = d3d8::D3DSWAPEFFECT_COPY_VSYNC;
  } else {
    params.SwapEffect = d3d8::D3DSWAPEFFECT(pParams->SwapEffect);
  }

  //Logger::debug("pParams->hDeviceWindow: " + std::to_string(pParams->hDeviceWindow));
  params.hDeviceWindow = pParams->hDeviceWindow;
  //Logger::debug("pParams->Windowed: " + std::to_string(pParams->Windowed));
  params.Windowed = pParams->Windowed;
  //Logger::debug("pParams->EnableAutoDepthStencil: " + std::to_string(pParams->EnableAutoDepthStencil));
  params.EnableAutoDepthStencil = pParams->EnableAutoDepthStencil;
  //Logger::debug("pParams->AutoDepthStencilFormat: " + std::to_string(pParams->AutoDepthStencilFormat));
  params.AutoDepthStencilFormat = d3d8::D3DFORMAT(pParams->AutoDepthStencilFormat);
  //Logger::debug("pParams->Flags: " + std::to_string(pParams->Flags));
  params.Flags = pParams->Flags;

  //Logger::debug("pParams->FullScreen_RefreshRateInHz: " + std::to_string(pParams->FullScreen_RefreshRateInHz));
  params.FullScreen_RefreshRateInHz = pParams->FullScreen_RefreshRateInHz;

  //Logger::debug("pParams->PresentationInterval: " + std::to_string(pParams->PresentationInterval));
  UINT PresentationInterval = pParams->PresentationInterval;
  // In D3D8 nothing except D3DPRESENT_INTERVAL_DEFAULT can be used as a flag for windowed presentation
  if (pParams->Windowed) {
    Logger::warn("ConvertPresentParameters8: Forcing D3DPRESENT_INTERVAL_DEFAULT for windowed presentation");
    PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
  }
  // FullScreen_PresentationInterval -> PresentationInterval
  params.FullScreen_PresentationInterval = PresentationInterval;

  return params;
}

// (9<-8) Convert D3DSURFACE_DESC
inline D3DSURFACE_DESC ConvertSurfaceDesc8(d3d8::D3DSURFACE_DESC* pSurf8) {
  D3DSURFACE_DESC surfDesc9;

  surfDesc9.Format  = D3DFORMAT(pSurf8->Format);
  surfDesc9.Type    = D3DRESOURCETYPE(pSurf8->Type);
  surfDesc9.Usage   = pSurf8->Usage;
  surfDesc9.Pool    = D3DPOOL(pSurf8->Pool);

  surfDesc9.MultiSampleType = D3DMULTISAMPLE_TYPE(pSurf8->MultiSampleType);
  surfDesc9.MultiSampleQuality = 0;
  surfDesc9.Width   = pSurf8->Width;
  surfDesc9.Height  = pSurf8->Height;

  return surfDesc9;
}

// If this D3DSAMPLERSTATETYPE has been remapped to a d3d8::D3DTEXTURESTAGESTATETYPE
// it will be returned, otherwise returns -1u
inline d3d8::D3DTEXTURESTAGESTATETYPE GetTextureStateType8(const D3DSAMPLERSTATETYPE SamplerType) {
  switch (SamplerType) {
    // 13-21:
    case D3DSAMP_ADDRESSU:      return d3d8::D3DTSS_ADDRESSU;
    case D3DSAMP_ADDRESSV:      return d3d8::D3DTSS_ADDRESSV;
    case D3DSAMP_BORDERCOLOR:   return d3d8::D3DTSS_BORDERCOLOR;
    case D3DSAMP_MAGFILTER:     return d3d8::D3DTSS_MAGFILTER;
    case D3DSAMP_MINFILTER:     return d3d8::D3DTSS_MINFILTER;
    case D3DSAMP_MIPFILTER:     return d3d8::D3DTSS_MIPFILTER;
    case D3DSAMP_MIPMAPLODBIAS: return d3d8::D3DTSS_MIPMAPLODBIAS;
    case D3DSAMP_MAXMIPLEVEL:   return d3d8::D3DTSS_MAXMIPLEVEL;
    case D3DSAMP_MAXANISOTROPY: return d3d8::D3DTSS_MAXANISOTROPY;
    // 25:
    case D3DSAMP_ADDRESSW:      return d3d8::D3DTSS_ADDRESSW;
    default:                    return d3d8::D3DTEXTURESTAGESTATETYPE(-1u);
  }
}

// Some formats simply won't be supported in D3D8, although they
// are expected to work just fine in D3D9 for various purposes
inline bool IsUnsupportedD3D9Format(const D3DFORMAT format) {
  return format == D3DFMT_A16B16G16R16
      || format == D3DFMT_A16B16G16R16F // Dawn of War: Definitive Edition requires it for cubemaps
      || format == D3DFMT_A32B32G32R32F;
}

// D3D8 doesn't support anything but these two formats as adapter formats
inline bool IsSupportedD3D8AdapterFormat(const D3DFORMAT format) {
  return format == D3DFMT_X8R8G8B8
      || format == D3DFMT_R5G6B5;
}

template<typename T, typename J>
inline T bitcast(const J& src) {
  static_assert(sizeof(T) == sizeof(J));
  static_assert(std::is_trivially_copyable<J>::value && std::is_trivial<T>::value);

  T dst;
  memcpy(&dst, &src, sizeof(T));
  return dst;
}

