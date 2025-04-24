#include "input_manager.hpp"

#include <backends/imgui_impl_sdl3.h>

#include "window.hpp"

InputManager::InputManager() : window(nullptr), next_callback_id(0) {
    current_key_states.fill(KeyState::Released);
    previous_key_states.fill(KeyState::Released);

    mouse_button_states.fill(KeyState::Released);
    prev_mouse_button_states.fill(KeyState::Released);
}

InputManager::~InputManager() {
}

bool InputManager::init(Window *window) {
    SC_LOG_INFO("Initializing input manager");
    this->window = window;
    if (!this->window) {
        SC_LOG_ERROR("Failed to initialize input manager: window pointer is null");
        return false;
    }

    SC_LOG_INFO("Input manager initialized successfully");
    return true;
}

void InputManager::update(double deltaTime) {
    std::swap(previous_key_states, current_key_states);
    std::swap(prev_mouse_button_states, mouse_button_states);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        switch (event.type) {
            case SDL_EVENT_QUIT:
                SC_LOG_INFO("Received quit event, setting window to close");
                window->set_should_close(true);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                SC_LOG_DEBUG("Window resize event: {}x{}", event.window.data1, event.window.data2);
                break;
            default:
                break;
        }
    }
}
