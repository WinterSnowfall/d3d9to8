#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"

#include <vector>

using Logger = ThreadSafeLogger;

// In invalid/unsupported cases return register 255, which will cause the vertex shader to fail
// compilation (the maximum supported register number in D3D8 is 16, for a total of 17 registers)
inline BYTE ConvertD3D9UsageToD3D8Register(D3DDECLUSAGE Usage, BYTE UsageIndex) {
  switch (Usage) {
    case D3DDECLUSAGE_POSITION:
      switch (UsageIndex) {
        case 0:
          return d3d8::D3DVSDE_POSITION;
        case 1:
          return d3d8::D3DVSDE_POSITION2;
        default:
          Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported D3DDECLUSAGE_POSITION index: " + std::to_string(UsageIndex));
          return 255;
      }

    case D3DDECLUSAGE_NORMAL:
      switch (UsageIndex) {
        case 0:
          return d3d8::D3DVSDE_NORMAL;
        case 1:
          return d3d8::D3DVSDE_NORMAL2;
        default:
          Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported D3DDECLUSAGE_NORMAL index: " + std::to_string(UsageIndex));
          return 255;
      }

    case D3DDECLUSAGE_BLENDWEIGHT:
      return d3d8::D3DVSDE_BLENDWEIGHT;

    case D3DDECLUSAGE_BLENDINDICES:
      return d3d8::D3DVSDE_BLENDINDICES;

    case D3DDECLUSAGE_PSIZE:
      return d3d8::D3DVSDE_PSIZE;

    case D3DDECLUSAGE_COLOR:
      switch (UsageIndex) {
        case 0:
          return d3d8::D3DVSDE_DIFFUSE;
        case 1:
          return d3d8::D3DVSDE_SPECULAR;
        default:
          Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported D3DDECLUSAGE_COLOR index: " + std::to_string(UsageIndex));
          return 255;
      }

    case D3DDECLUSAGE_TEXCOORD:
      if (unlikely(UsageIndex > 7)) {
        Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported D3DDECLUSAGE_TEXCOORD index: " + std::to_string(UsageIndex));
        return 255;
      }
      return d3d8::D3DVSDE_TEXCOORD0 + UsageIndex;

    // None of the below are supported in D3D8, but should be rare/unused with FF/SM1 shaders
    case D3DDECLUSAGE_TANGENT:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_TANGENT");
      return 255;

    case D3DDECLUSAGE_BINORMAL:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_BINORMAL");
      return 255;

    case D3DDECLUSAGE_TESSFACTOR:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_TESSFACTOR");
      return 255;

    case D3DDECLUSAGE_POSITIONT:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_POSITIONT");
      return 255;

    case D3DDECLUSAGE_FOG:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_FOG");
      return 255;

    case D3DDECLUSAGE_DEPTH:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_DEPTH");
      return 255;

    case D3DDECLUSAGE_SAMPLE:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unsupported use of D3DDECLUSAGE_SAMPLE");
      return 255;

    default:
      Logger::warn("ConvertD3D9UsageToD3D8Register:: Unknown usage: " + std::to_string(Usage));
      return 255;
  }
}

inline void ConvertD3D9Function(std::vector<DWORD>* function8, std::vector<DWORD>* function9) {
  const size_t tokenCount = function9->size();
  size_t tokenIndex = 0;

  // Version token
  if (likely(tokenIndex < tokenCount)) {
    function8->push_back(function9->at(tokenIndex));
    tokenIndex++;
  }

  while (tokenIndex < tokenCount) {
    DWORD token  = function9->at(tokenIndex);
    //Logger::debug("ConvertD3D9Function:: Token: " + std::to_string(token));
    DWORD opCode = token & D3DSI_OPCODE_MASK;
    //Logger::debug("ConvertD3D9Function:: OpCode: " + std::to_string(opCode));

    if (token == D3DVS_END()) {
      //Logger::debug("ConvertD3D9Function:: End token reached");
      function8->push_back(token);
      break;
    }

    // Skip DCL instructions
    if (opCode == D3DSIO_DCL) {
      // D3DSIO_DCL token + usage token + register token for SM1
      tokenIndex += 3u;
    } else {
      function8->push_back(function9->at(tokenIndex));
      tokenIndex++;
    }
  }

}
