#include "framebuffer.h"
using namespace nes;

Framebuffer::Framebuffer(SDL_Renderer* renderer, int width, int height) {
    texture_ = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    pixels_ = nullptr;
}

Framebuffer::~Framebuffer() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
}

void Framebuffer::destroy() {
    SDL_DestroyTexture(texture_);
    texture_ = nullptr;
}

SDL_Texture* Framebuffer::getTexture() const {
    return texture_;
}

void Framebuffer::lock() {
    SDL_LockTexture(
        texture_, 
        nullptr,
        &pixels_,
        &pitch_
    );
}

void Framebuffer::unlock() {
    SDL_UnlockTexture(texture_);
    pixels_ = nullptr;
}

void Framebuffer::setPixel(int x, int y, std::uint32_t color) {
    auto pixel = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(pixels_) + y * pitch_) + x;
    *pixel = color;
}

bool Framebuffer::isLocked() const {
    return pixels_;
}