#pragma once

constexpr uint32_t D3D9TO8_MAX_TEXTURE_STAGES = 8;
constexpr uint32_t D3D9TO8_MAX_STREAMS        = 16;

// ZBIAS can be an integer from 0 to 16 and needs to be remapped to float
constexpr float    D3D9TO8_ZBIAS_SCALE        = -1.0f / ((1u << 16) - 1); // Consider D16 precision
constexpr float    D3D9TO8_ZBIAS_SCALE_INV    = 1 / D3D9TO8_ZBIAS_SCALE;
