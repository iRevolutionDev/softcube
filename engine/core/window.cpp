#include "common.hpp"
#include "window.hpp"

namespace softcube {
    Window::Window()
        : window(nullptr)
          , width(0)
          , height(0)
          , should_close(false)
          , is_fullscreen(false) {
    }

    Window::~Window() {
        if (window) {
            SDL_DestroyWindow(static_cast<SDL_Window *>(window));
            window = nullptr;
        }
    }

    bool Window::init(int width, int height, const std::string &title, bool fullscreen) {
        SC_INFO("Creating window: {}x{}, title: {}", width, height, title);

        this->width = width;
        this->height = height;
        this->title = title;
        this->is_fullscreen = fullscreen;

        Uint32 flags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;
        if (fullscreen) {
            flags |= SDL_WINDOW_FULLSCREEN;
        }

        window = SDL_CreateWindow(title.c_str(), width, height, flags);

        if (!window) {
            SC_ERROR("Failed to create window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    void Window::update() {
        int old_width = width;
        int old_height = height;
        SDL_GetWindowSize(static_cast<SDL_Window *>(window), &width, &height);

        if (old_width != width || old_height != height) {
            SC_DEBUG("Window resized: {}x{} -> {}x{}", old_width, old_height, width, height);
        }
    }

    void Window::set_title(const std::string &new_title) {
        title = new_title;
        SDL_SetWindowTitle(static_cast<SDL_Window *>(window), title.c_str());
    }

    void Window::set_fullscreen(bool fullscreen) {
        if (is_fullscreen == fullscreen)
            return;

        is_fullscreen = fullscreen;
        SDL_SetWindowFullscreen(static_cast<SDL_Window *>(window), fullscreen);
    }
}
