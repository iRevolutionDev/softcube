#include "input_manager.hpp"

#include "window.hpp"

InputManager::InputManager() : window(nullptr) {
    current_key_states.fill(KeyState::Released);
    previous_key_states.fill(KeyState::Released);

    mouse_button_states.fill(KeyState::Released);
    prev_mouse_button_states.fill(KeyState::Released);
}

InputManager::~InputManager() {
}

bool InputManager::init(Window *window) {
    this->window = window;
    if (!this->window) {
        return false;
    }

    return true;
}

void InputManager::update(double deltaTime) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                window->set_should_close(true);
                break;
            default:
                break;
        }
    }
}
