#include "window.h"
using namespace nes;

#include <stdexcept>
#include <string>

Window::Window() : handle_(nullptr) {}

Window::Window(const char* title, int width, int height, Uint32 flags) {
    handle_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, width, height, flags);

    if (!handle_) {
        throw std::runtime_error("Failed to create window: " +
                                 std::string(SDL_GetError()));
    } else {
        spdlog::info("Window created");
    }
}

Window::Window(Window&& rhs) noexcept {
    handle_ = std::exchange(rhs.handle_, nullptr);
}

Window& Window::operator=(Window&& rhs) noexcept {
    if (handle_) {
        SDL_DestroyWindow(handle_);
        spdlog::info("Window destroyed");
    }
    handle_ = std::exchange(rhs.handle_, nullptr);
    return *this;
}

Window::~Window() {
    if (handle_) {
        SDL_DestroyWindow(handle_);
        spdlog::info("Window destroyed");
    }
}

SDL_Window* Window::getHandle() const { return handle_; }