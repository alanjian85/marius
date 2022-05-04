#ifndef NES_GRAPHICS_WINDOW_H_
#define NES_GRAPHICS_WINDOW_H_

#include <algorithm>

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

namespace nes {
    class Window final {
    public:
        Window();

        Window(const char* title, int width, int height);

        Window(Window&& rhs) noexcept;

        Window& operator=(Window&& rhs) noexcept;

        ~Window();

        [[nodiscard]] SDL_Window* getHandle() const;

        [[nodiscard]] int getWidth() const;

        [[nodiscard]] int getHeight() const;
    private:
        SDL_Window* handle_;
        int width_;
        int height_;
    };
}

#endif // NES_GRAPHICS_WINDOW_H_