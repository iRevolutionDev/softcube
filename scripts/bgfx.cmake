include(FetchContent)

message("BGFX")
FetchContent_Declare(
        bgfx
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake
        GIT_TAG v1.128.8832-488
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(bgfx)