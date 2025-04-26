#ifndef ENGINE_COMMON_HPP
#define ENGINE_COMMON_HPP

// clang-format off

// Platform-specific defines
#ifdef _WIN32
    #define NOMINMAX
    #include <sdkddkver.h>
    #include <winsock2.h>
    #include <windows.h>
    #define SOFTCUBE_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #define SOFTCUBE_PLATFORM_APPLE
    #if TARGET_OS_IPHONE
        #define SOFTCUBE_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define SOFTCUBE_PLATFORM_MACOS
    #endif
#elif defined(__linux__)
    #define SOFTCUBE_PLATFORM_LINUX
#elif defined(__ANDROID__)
    #define SOFTCUBE_PLATFORM_ANDROID
#endif

// Standard C libraries
#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>

// Time related libraries
#include <chrono>
#include <ctime>

// File and IO related libraries
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

// Threading and synchronization
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

// Memory management
#include <memory>
#include <new>

// Containers
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <list>
#include <forward_list>

// Algorithms and utility
#include <algorithm>
#include <functional>
#include <utility>
#include <type_traits>
#include <initializer_list>
#include <tuple>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <limits>
#include <random>
#include <numeric>

// BGFX
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/embedded_shader.h>

//EntT
#include "entt/entt.hpp"

// SDL3
#include <SDL3/SDL.h>

// Engine-wide type definitions
namespace softcube {
    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using f32 = float;
    using f64 = double;
    using byte = u8;
}

using namespace softcube;

#define SOFTCUBE_VERSION_MAJOR 0
#define SOFTCUBE_VERSION_MINOR 1
#define SOFTCUBE_VERSION_PATCH 0

#if defined(_DEBUG) || defined(DEBUG)
    #define SOFTCUBE_DEBUG
    #define SOFTCUBE_ASSERT(x, msg) assert((x) && (msg))
#else
    #define SOFTCUBE_ASSERT(x, msg) ((void)0)
#endif

// clang-format on

#endif // ENGINE_COMMON_HPP
