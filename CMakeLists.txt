cmake_minimum_required(VERSION 3.29)
project(softcube VERSION 0.1.0 DESCRIPTION "A another minecraft clone" LANGUAGES CXX)

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/test")

# Scripts Packages
message(STATUS "Feching packages...")
include(scripts/bgfx.cmake)
include(scripts/sdl3.cmake)
include(scripts/imgui.cmake)
include(scripts/entt.cmake)
include(scripts/spdlog.cmake)

# Project Files
file(GLOB_RECURSE SRC_FILES
        "${SRC_DIR}/**.cpp"
        "${SRC_DIR}/**.hpp"
        "${SRC_DIR}/**.h"
)

if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SRC_FILES})
else ()
    add_executable(${PROJECT_NAME} ${SRC_FILES})
endif ()

# Compiler Options
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 26)

source_group(TREE "${SRC_DIR}" PREFIX "src" FILES ${SRC_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE "${SRC_DIR}/common.hpp")

# Shader Compilation
message(STATUS "Compiling shaders...")
file(GLOB VERTEX_SHADER_FILES "assets/shaders/v_*.sc")
file(GLOB FRAGMENT_SHADER_FILES "assets/shaders/f_*.sc")

# Create output directory for shader headers
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/generated/shaders")

# Compile vertex shaders
bgfx_compile_shaders(
        TYPE VERTEX
        SHADERS ${VERTEX_SHADER_FILES}
        VARYING_DEF "${PROJECT_SOURCE_DIR}/assets/shaders/varying.def.sc"
        OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/generated/shaders"
        AS_HEADERS
)

# Compile fragment shaders
bgfx_compile_shaders(
        TYPE FRAGMENT
        SHADERS ${FRAGMENT_SHADER_FILES}
        VARYING_DEF "${PROJECT_SOURCE_DIR}/assets/shaders/varying.def.sc"
        OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/generated/shaders"
        AS_HEADERS
)

# Create an interface library for shader dependencies
add_library(shaders INTERFACE)
target_sources(shaders INTERFACE ${VERTEX_SHADER_FILES} ${FRAGMENT_SHADER_FILES})
target_include_directories(shaders INTERFACE "${CMAKE_BINARY_DIR}/include/generated/shaders")

# Add shader dependency to main project
add_dependencies(${PROJECT_NAME} shaders)

target_include_directories(${PROJECT_NAME} PUBLIC
        "${SRC_DIR}"
        "${BGFX_INCLUDE_DIR}"
        "${BIMG_INCLUDE_DIR}"
        "${BX_INCLUDE_DIR}"
        "${SDL3_INCLUDE_DIR}"
        "${imgui_SOURCE_DIR}"
        "${spdlog_SOURCE_DIR}"
        "${entt_SOURCE_DIR}/src"
        "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        bgfx
        bimg
        bx
        SDL3::SDL3
        imgui
        shaders
        spdlog
        ${CMAKE_DL_LIBS}
)

#Copy shaders directory to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets/shaders"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/shaders"
)

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            VERBATIM
    )
endif ()

# C++ Hot reloading
if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 142)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/ZI>)
endif ()

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SOFT_CUBE_PLATFORM_WINDOWS)
elseif (UNIX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SOFT_CUBE_PLATFORM_LINUX)
elseif (APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SOFT_CUBE_PLATFORM_MACOS)
elseif (ANDROID)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SOFT_CUBE_PLATFORM_ANDROID)
endif ()