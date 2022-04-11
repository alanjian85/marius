#include "ppu.h"
using namespace nes;

#include <algorithm>

Ppu::Ppu()
    : texture_(nullptr)
{
    vblank_ = true;
}

Ppu::Ppu(SDL_Renderer* renderer)
    : Ppu()
{
    texture_ = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        kWidth, kHeight
    );
}

Ppu::~Ppu() {
    if (texture_) {
        SDL_DestroyTexture(texture_);
    }
}

SDL_Texture* Ppu::getTexture() const {
    return texture_;
}

float Ppu::getAspect() const {
    return static_cast<float>(kWidth) / kHeight;
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}

void Ppu::update() {
    vblank_ = true;
}