#include "window.h"
using namespace nes;

Window::Window()
    : handle_(nullptr)
{

}

Window::Window(const char* title, int width, int height) {
    handle_ = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        0
    );
    width_ = width;
    height_ = height;

    if (!handle_) {
        spdlog::error("Failed to create window: {}", SDL_GetError());
    } else {
        spdlog::info("Window created");
    }
}

Window::Window(Window&& rhs) noexcept {
    handle_ = std::exchange(rhs.handle_, nullptr);
    width_ = rhs.width_;
    height_ = rhs.height_;
}

Window& Window::operator=(Window&& rhs) noexcept {
    if (handle_) {
        SDL_DestroyWindow(handle_);
        spdlog::info("Window destroyed");
    }
    handle_ = std::exchange(rhs.handle_, nullptr);
    width_ = rhs.width_;
    height_ = rhs.height_;
    return *this;
}

Window::~Window() {
    if (handle_) {
        SDL_DestroyWindow(handle_);
        spdlog::info("Window destroyed");
    }
}

SDL_Window* Window::getHandle() const {
    return handle_;
}

int Window::getWidth() const {
    return width_;
}

int Window::getHeight() const {
    return height_;
}