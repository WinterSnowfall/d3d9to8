#pragma once

// General logging options
//
// DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3, NONE = 4
constexpr uint8_t D3D9TO8_LOG_LEVEL          = 1;
// Will slow down things considerably, especially when using DEBUG
constexpr bool    D3D9TO8_WRITE_TO_LOG_FILE  = false;

// Various experimental toggles
//
// Reports full SM3 capabilities and doesn't error out on SM2+ shader
// creation, however it will only provide dummy shaders and capabilities
// to games, hence rendering is expected to be wildly broken when enabled
constexpr bool    D3D9TO8_LENIENT_SHADERS    = false;
// May help some games together with DXVK, but not otherwise, as it
// leaves inline bool and integer constant declarations intact in SM1
// shaders, both of which aren't technically supported in native D3D8
constexpr bool    D3D9TO8_LENIENT_SM1_CTYPES = false;
