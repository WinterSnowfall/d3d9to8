#pragma once

#include "d3d9_include.h"
#include "d3d9_logger.h"
#include "d3d9_options.h"
#include "d3d9_caps.h"

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
      if (unlikely(UsageIndex >= D3DDP_MAXTEXCOORD)) {
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

inline void ConvertD3D9Shader(
        std::vector<DWORD>* declaration8,
        std::vector<D3DVERTEXELEMENT9>* declaration9,
        std::vector<DWORD>* function8,
        std::vector<DWORD>* function9) {
  std::vector<DWORD> constDefs;

  // Fixed function shader declarations won't have a function
  if (function9 != nullptr) {
    const size_t funcTokenCount = function9->size();
    size_t funcTokenIndex = 0;

    Logger::debug("ConvertD3D9Shader:: ****** FUNCTION ******");

    // Function version token
    if (likely(funcTokenIndex < funcTokenCount)) {
      const DWORD vsMajor = D3DSHADER_VERSION_MAJOR(function9->at(funcTokenIndex));
      const DWORD vsMinor = D3DSHADER_VERSION_MINOR(function9->at(funcTokenIndex));
      Logger::debug("ConvertD3D9Shader:: VS version: " + std::to_string(vsMajor) + "." + std::to_string(vsMinor));

      function8->push_back(function9->at(funcTokenIndex));
      funcTokenIndex++;
    }

    while (funcTokenIndex < funcTokenCount) {
      DWORD token  = function9->at(funcTokenIndex);
      //Logger::debug("ConvertD3D9Shader:: Token:  " + std::to_string(token));
      DWORD opCode = token & D3DSI_OPCODE_MASK;
      //Logger::debug("ConvertD3D9Shader:: OpCode: " + std::to_string(opCode));

      if (token == D3DVS_END()) {
        function8->push_back(token);
        break;
      }

      switch (opCode) {
        case D3DSIO_DCL:
          // TODO: Save the declaration and later use the tokens during definition
          // construction in case they overlap with the current shader definition,
          // and decide which takes priority over the other for a certain register
          Logger::debug("ConvertD3D9Shader:: Skipping D3DSIO_DCL block");
          // D3DSIO_DCL token + usage token + register token for SM1
          funcTokenIndex += 3u;
          break;
        case D3DSIO_DEF: {
          Logger::debug("ConvertD3D9Shader:: Parsing a D3DSIO_DEF block");
          const DWORD destReg = function9->at(funcTokenIndex + 1u);
          const DWORD startAddr = destReg & D3DSP_REGNUM_MASK;

          DWORD defCount = 1u;
          DWORD defIndex = funcTokenIndex + 6u; // Skip the current D3DSIO_DEF block
          while (defIndex + 5u < function9->size()
            && (function9->at(defIndex) & D3DSI_OPCODE_MASK) == D3DSIO_DEF) {
            defCount++;
            defIndex += 6u; // Skips an entire D3DSIO_DEF block
          }

          constDefs.push_back(D3DVSD_CONST_D3D8(startAddr, defCount));
          for (DWORD i = 0; i < defCount; i++) {
            const DWORD instIndex = funcTokenIndex + (i * 6);
            constDefs.push_back(function9->at(instIndex + 2));
            constDefs.push_back(function9->at(instIndex + 3));
            constDefs.push_back(function9->at(instIndex + 4));
            constDefs.push_back(function9->at(instIndex + 5));
          }

          funcTokenIndex += (6u * defCount); // Skip all continuous D3DSIO_DEF blocks
          break;
        }
        case D3DSIO_DEFI: // Integer constants aren't present in D3D8
          Logger::warn("ConvertD3D9Shader:: Unsupported use of D3DSIO_DEFI");
          if (likely(!D3D9TO8_LENIENT_SHADERS)) {
            funcTokenIndex += 6u;
            break;
          }
          [[fallthrough]];
        case D3DSIO_DEFB: // SM2+ VS only
          Logger::warn("ConvertD3D9Shader:: Unsupported use of D3DSIO_DEFB");
          if (likely(!D3D9TO8_LENIENT_SHADERS)) {
            funcTokenIndex += 3u;
            break;
          }
          [[fallthrough]];
        default:
          function8->push_back(function9->at(funcTokenIndex));
          funcTokenIndex++;
          break;
      }
    }
  }

  Logger::debug("ConvertD3D9Shader:: ***** DEFINITION *****");

  const size_t defTokenCount = declaration9->size();
  size_t defTokenIndex = 0;
  BYTE currentStream8 = 0xFF;

  // Process until we hit D3DDECL_END()
  while (defTokenIndex < defTokenCount) {
    D3DVERTEXELEMENT9& defToken = declaration9->at(defTokenIndex);

    if (defToken.Type == D3DDECLTYPE_UNUSED) {
      // Before ending the definition, slot in any constant declarations
      if (!constDefs.empty()) {
        Logger::debug("ConvertD3D9Shader:: Including " + std::to_string(constDefs.size() / 5) + " constant definitions");
        for (auto& constDef : constDefs) {
          declaration8->push_back(constDef);
        }
      }

      declaration8->push_back(D3DVSD_END());
      Logger::debug("ConvertD3D9Shader:: **********************");
      break;
    }

    //Logger::debug("ConvertD3D9Shader:: Element:       " + std::to_string(defTokenIndex));
    //Logger::debug("ConvertD3D9Shader:: Stream:        " + std::to_string(defToken.Stream));
    //Logger::debug("ConvertD3D9Shader:: Offset:        " + std::to_string(defToken.Offset)); // Ignored in D3D8
    //Logger::debug("ConvertD3D9Shader:: Type:          " + std::to_string(defToken.Type));
    //Logger::debug("ConvertD3D9Shader:: Method:        " + std::to_string(defToken.Method)); // Ignored in D3D8
    //Logger::debug("ConvertD3D9Shader:: Usage:         " + std::to_string(defToken.Usage));
    //Logger::debug("ConvertD3D9Shader:: UsageIndex:    " + std::to_string(defToken.UsageIndex));

    // Translate the D3D9 vertex declaration to a D3D8 vertex declaration
    //Logger::debug("ConvertD3D9Shader:: ----------------------");
    if (defToken.Stream != currentStream8) {
      // Emit a D3DVSD_STREAM token if the current stream changes
      currentStream8 = defToken.Stream;
    Logger::debug("ConvertD3D9Shader:: D3D8 Stream:   " + std::to_string(currentStream8));
      declaration8->push_back(D3DVSD_STREAM_D3D8(currentStream8));
    }
    const BYTE Reg8 = ConvertD3D9UsageToD3D8Register(static_cast<D3DDECLUSAGE>(defToken.Usage), defToken.UsageIndex);
    const d3d8::D3DVSDT_TYPE Type8 = d3d8::D3DVSDT_TYPE(defToken.Type);
    Logger::debug("ConvertD3D9Shader:: D3D8 Register: " + std::to_string(Reg8) + ", Type: " + std::to_string(Type8));
    // D3D8 doesn't support anything beyond D3DDECLTYPE_SHORT4
    if (unlikely(defToken.Type > D3D9TO8_MAX_VS_DECL_TYPE)) {
      Logger::warn("ConvertD3D9Shader:: Unsupported type: " + std::to_string(defToken.Type));
    } else if (Reg8 != 255) {
      // Only add a register declaration if we have a valid register
      declaration8->push_back(D3DVSD_REG_D3D8(Reg8, Type8));
    }
    //Logger::debug("ConvertD3D9Shader:: ---------------------");

    defTokenIndex++;
  }

}
