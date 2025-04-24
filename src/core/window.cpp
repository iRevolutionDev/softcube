#include "common.hpp"
#include "window.hpp"

#include "logging.hpp"

Window::Window() : window(nullptr), width(0), height(0), fullscreen(false) {
}

Window::~Window() {
    if (!window) {
        return;
    }

    SDL_DestroyWindow(window);
    window = nullptr;
}

bool Window::init(const int width, const int height, const std::string &title, const bool fullscreen) {
    this->width = width;
    this->height = height;
    this->fullscreen = fullscreen;

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SC_LOG_INFO("SDL_Init Error: {}", SDL_GetError());
        return false;
    }

    auto window_flags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;
    if (fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow(title.c_str(), width, height, window_flags);

    if (!window) {
        SC_LOG_INFO("Failed to create SDL window: {}", SDL_GetError());
        return false;
    }

    SDL_GetWindowSize(window, &this->width, &this->height);

    return true;
}

void Window::update() {
    SDL_GetWindowSize(window, &width, &height);
}

bool Window::get_should_close() const {
    return this->should_close;
}

void Window::set_title(const std::string &title) {
    this->title = title;
}





