#ifndef COMMON_INC
#define COMMON_INC

// clang-format off

#define NOMINMAX

#include <sdkddkver.h>
#include <winsock2.h>
#include <windows.h>
#include <d3d11.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <set>
#include <unordered_set>
#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <format>

#include <dwmapi.h>
#include <tchar.h>
#include <uxtheme.h>

#include <tlhelp32.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/embedded_shader.h>

#include "imgui.h"
#include "misc/freetype/imgui_freetype.h"

#include "SDL3/SDL.h"

// clang-format on

using namespace std::chrono_literals;

inline std::atomic_bool g_running{false};

#endif
