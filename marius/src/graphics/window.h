#ifndef NES_GRAPHICS_WINDOW_H_
#define NES_GRAPHICS_WINDOW_H_

#include <SDL.h>
#include <spdlog/spdlog.h>

#include <algorithm>

namespace nes {
class Window final {
   public:
    Window();

    Window(const char* title, int width, int height, Uint32 flags);

    Window(Window&& rhs) noexcept;

    Window& operator=(Window&& rhs) noexcept;

    ~Window();

    [[nodiscard]] SDL_Window* getHandle() const;

   private:
    SDL_Window* handle_;
};
}  // namespace nes

#endif  // NES_GRAPHICS_WINDOW_H_