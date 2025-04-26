cmake_minimum_required(VERSION 3.29)
project(softcube VERSION 0.1.0 DESCRIPTION "A modern Minecraft clone" LANGUAGES CXX)

# Define source directories
set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/engine")
set(GAME_DIR "${PROJECT_SOURCE_DIR}/game")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

# Scripts for external packages
message(STATUS "Fetching packages...")
include(scripts/bgfx.cmake)
include(scripts/sdl3.cmake)
include(scripts/freetype.cmake)
include(scripts/imgui.cmake)
include(scripts/entt.cmake)
include(scripts/spdlog.cmake)

# Project Files
file(GLOB_RECURSE ENGINE_SRC_FILES
        "${ENGINE_DIR}/**.cpp"
        "${ENGINE_DIR}/**.hpp"
        "${ENGINE_DIR}/**.h"
)

file(GLOB_RECURSE GAME_SRC_FILES
        "${GAME_DIR}/**.cpp"
        "${GAME_DIR}/**.hpp"
        "${GAME_DIR}/**.h"
)

# Create the engine library
add_library(softcube_engine STATIC ${ENGINE_SRC_FILES})

# Create the executable
if (WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${GAME_SRC_FILES})
else ()
    add_executable(${PROJECT_NAME} ${GAME_SRC_FILES})
endif ()

# Set compiler options
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 26)
set_property(TARGET softcube_engine PROPERTY CXX_STANDARD 26)

# Source grouping
source_group(TREE "${ENGINE_DIR}" PREFIX "engine" FILES ${ENGINE_SRC_FILES})
source_group(TREE "${GAME_DIR}" PREFIX "game" FILES ${GAME_SRC_FILES})

# Precompiled Headers
target_precompile_headers(softcube_engine PRIVATE "${ENGINE_DIR}/core/common.hpp")
target_precompile_headers(${PROJECT_NAME} PRIVATE "${ENGINE_DIR}/core/common.hpp")

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

# Add shader dependency to engine
add_dependencies(softcube_engine shaders)

# Configure engine library includes
target_include_directories(softcube_engine PUBLIC
        "${ENGINE_DIR}"
        "${BGFX_INCLUDE_DIR}"
        "${BIMG_INCLUDE_DIR}"
        "${BX_INCLUDE_DIR}"
        "${SDL3_INCLUDE_DIR}"
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/misc/freetype"
        "${freetype_SOURCE_DIR}/include"
        "${spdlog_SOURCE_DIR}"
        "${entt_SOURCE_DIR}/src"
        "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

# Configure game executable includes
target_include_directories(${PROJECT_NAME} PUBLIC
        "${ENGINE_DIR}"
        "${GAME_DIR}"
        "${CMAKE_BINARY_DIR}/include/generated/shaders"
)

# Link dependencies
target_link_libraries(softcube_engine PUBLIC
        bgfx
        bimg
        bx
        SDL3::SDL3
        imgui
        shaders
        spdlog
        freetype
        ${CMAKE_DL_LIBS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
        softcube_engine
)

target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_ENABLE_FREETYPE)
target_compile_definitions(softcube_engine PRIVATE IMGUI_ENABLE_FREETYPE)

# Copy assets directory to the build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets" "${CMAKE_BINARY_DIR}/assets"
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