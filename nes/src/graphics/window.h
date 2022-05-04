#ifndef NES_GRAPHICS_WINDOW_H_
#define NES_GRAPHICS_WINDOW_H_

#include <algorithm>

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

namespace nes {
    class Window {
    public:
        Window() : handle_(nullptr) {}

        Window(const char* title, int width, int height) {
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
            }
        }

        Window(Window&& rhs) noexcept {
            handle_ = std::exchange(rhs.handle_, nullptr);
            width_ = rhs.width_;
            height_ = rhs.height_;
        }

        Window& operator=(Window&& rhs) noexcept {
            if (handle_) {
                SDL_DestroyWindow(handle_);
                spdlog::info("Window destroyed");
            }
            handle_ = std::exchange(rhs.handle_, nullptr);
            width_ = rhs.width_;
            height_ = rhs.height_;
            return *this;
        }

        ~Window() {
            if (handle_) {
                SDL_DestroyWindow(handle_);
                spdlog::info("Window destroyed");
            }
        }

        [[nodiscard]] SDL_Window* getHandle() const {
            return handle_;
        }

        [[nodiscard]] int getWidth() const {
            return width_;
        }

        [[nodiscard]] int getHeight() const {
            return height_;
        }
    private:
        SDL_Window* handle_;
        int width_;
        int height_;
    };
}

#endif // NES_GRAPHICS_WINDOW_H_