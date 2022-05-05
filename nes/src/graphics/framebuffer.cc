#include "framebuffer.h"
using namespace nes;

#include <stdexcept>
#include <string>

#include <spdlog/spdlog.h>

Framebuffer::Framebuffer()
    : texture_(nullptr)
{

}

Framebuffer::Framebuffer(const Renderer& renderer, int width, int height) {
    texture_ = SDL_CreateTexture(
        renderer.getHandle(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height
    );
    pixels_ = nullptr;

    if (!texture_) {
        throw std::runtime_error("Failed to create framebuffer texture: " + std::string(SDL_GetError()));
    } else {
        spdlog::info("Framebuffer texture created");
    }
}

Framebuffer::Framebuffer(Framebuffer&& rhs) noexcept {
    texture_ = std::exchange(rhs.texture_, nullptr);
    pixels_ = rhs.pixels_;
    pitch_ = rhs.pitch_;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& rhs) noexcept {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        spdlog::info("Framebuffer texture destroyed");
    }
    texture_ = std::exchange(rhs.texture_, nullptr);
    pixels_ = rhs.pixels_;
    pitch_ = rhs.pitch_;
    return *this;
}

Framebuffer::~Framebuffer() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
        spdlog::info("Framebuffer texture destroyed");
    }
}

SDL_Texture* Framebuffer::getTexture() const {
    return texture_;
}

void Framebuffer::lock() {
    int status = SDL_LockTexture(
        texture_, 
        nullptr,
        &pixels_,
        &pitch_
    );
    if (status < 0) {
        spdlog::critical("Unable to lock framebuffer texture");
    }
}

void Framebuffer::unlock() {
    SDL_UnlockTexture(texture_);
}

void Framebuffer::setPixel(int x, int y, std::uint32_t color) {
    auto pixel = reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(pixels_) + y * pitch_) + x;
    *pixel = color;
}