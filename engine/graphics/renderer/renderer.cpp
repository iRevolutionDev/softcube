#include "core/common.hpp"
#include "renderer.hpp"
#include <backends/imgui_impl_sdl3.h>
#include "core/window.hpp"

Renderer::Renderer() : window(nullptr), reset_flags(0), clear_flags(0), width(0), height(0), vsync(false),
                       clear_color{},
                       initialized(false) {
}

Renderer::~Renderer() {
    if (isValid(frame_buffer)) {
        destroy(frame_buffer);
    }

    if (imgui_layer) {
        imgui_layer->shutdown();
        delete imgui_layer;
        imgui_layer = nullptr;
    }

    bgfx::shutdown();
}

bool Renderer::init(Window *window, bool vsync) {
    SC_INFO("Initializing renderer with dimensions {}x{}, vsync: {}",
            window->get_width(), window->get_height(), vsync);

    this->window = window;
    this->width = window->get_width();
    this->height = window->get_height();
    this->vsync = vsync;

    auto *sdl_window = static_cast<SDL_Window *>(window->get_native_handle()); // Cast to SDL_Window

    bgfx::PlatformData platform_data{};
    bgfx::Init init;

#ifdef SOFTCUBE_PLATFORM_WINDOWS
    const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window),
                                                               SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                                               nullptr));
    platform_data.nwh = hwnd;
#elif defined(SOFTCUBE_PLATFORM_LINUX)
    if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
        Display *xdisplay = (Display *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        Window xwindow = (Window)SDL_GetNumberProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (xdisplay && xwindow) {
            platform_data.ndt = xdisplay;
            platform_data.nwh = (void *)(uintptr_t)xwindow;
        }
    } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
        struct wl_display *display = (struct wl_display *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        struct wl_surface *surface = (struct wl_surface *)SDL_GetPointerProperty(SDL_GetWindowProperties(sdl_window), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
        if (display && surface) {
            platform_data.ndt = xdisplay;
            platform_data.nwh = (void *)(uintptr_t)xwindow;
        }
    }
#endif
    init.platformData = platform_data;

    init.type = bgfx::RendererType::Count;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE;
    if (!bgfx::init(init)) {
        SC_ERROR("Failed to initialize BGFX renderer");
        return false;
    }

    SC_INFO("BGFX renderer initialized successfully");
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOther(sdl_window);

    imgui_layer = new ImGuiLayer();
    imgui_layer->reset(width, height);

    initialized = true;
    return true;
}

void Renderer::begin_frame() {
    if (window->get_width() != width || window->get_height() != height) {
        resize(window->get_width(), window->get_height());
    }

    bgfx::touch(0);
}

void Renderer::begin_imgui() {
    imgui_layer->new_frame();
}

void Renderer::end_imgui() {
    ImGui::Render();
    imgui_layer->render(ImGui::GetDrawData());
}

void Renderer::end_frame() {
    bgfx::frame();
}

void Renderer::resize(const int width, const int height) {
    SC_DEBUG("Resizing renderer: {}x{} -> {}x{}", this->width, this->height, width, height);
    this->width = width;
    this->height = height;
    bgfx::reset(width, height, vsync ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);
    bgfx::setViewRect(0, 0, 0, width, height);

    if (imgui_layer) {
        imgui_layer->reset(static_cast<uint16_t>(width), static_cast<uint16_t>(height));
    }
}

void Renderer::set_clear_color(const float r, const float g, const float b, const float a) {
    const auto rgba = static_cast<uint32_t>(static_cast<uint8_t>(r * 255) << 24) |
                      static_cast<uint32_t>(static_cast<uint8_t>(g * 255)) << 16 |
                      static_cast<uint32_t>(static_cast<uint8_t>(b * 255)) << 8 |
                      static_cast<uint32_t>(static_cast<uint8_t>(a * 255));

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, rgba, 1.0f, 0);
}

