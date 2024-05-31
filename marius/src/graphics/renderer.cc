#include "renderer.h"
using namespace nes;

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <string>

Renderer::Renderer() : handle_(nullptr) {}

Renderer::Renderer(const Window& window, int index, Uint32 flags) {
    handle_ = SDL_CreateRenderer(window.getHandle(), index, flags);
    if (!handle_) {
        throw std::runtime_error("Failed to create renderer: " +
                                 std::string(SDL_GetError()));
    } else {
        spdlog::info("Renderer created");
    }
}

Renderer::Renderer(Renderer&& rhs) noexcept {
    handle_ = std::exchange(rhs.handle_, nullptr);
}

Renderer& Renderer::operator=(Renderer&& rhs) noexcept {
    if (handle_) {
        SDL_DestroyRenderer(handle_);
        spdlog::info("Renderer destroyed");
    }
    handle_ = std::exchange(rhs.handle_, nullptr);
    return *this;
}

Renderer::~Renderer() {
    if (handle_) {
        SDL_DestroyRenderer(handle_);
        spdlog::info("Renderer destroyed");
    }
}

SDL_Renderer* Renderer::getHandle() const { return handle_; }

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(handle_, r, g, b, a);
}

void Renderer::clear() { SDL_RenderClear(handle_); }

void Renderer::copy(SDL_Texture* texture, SDL_Rect* src_rect,
                    SDL_Rect* dst_rect) {
    SDL_RenderCopy(handle_, texture, src_rect, dst_rect);
}

void Renderer::present() { SDL_RenderPresent(handle_); }