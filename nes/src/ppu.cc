#include "ppu.h"
using namespace nes;

#include <algorithm>

Ppu::Ppu(Framebuffer& framebuffer)
    : framebuffer_(framebuffer)
{
    cycle_ = 0;
    scanline_ = 261;
    vblank_ = true;
}

void Ppu::reset() {
    cycle_ = 0;
    scanline_ = 261;
}

void Ppu::cycle() {
    if (scanline_ == 241) {
        vblank_ = true;
    }

    ++cycle_;
    if (cycle_ == 341) {
        cycle_ = 0;
        scanline_ = (scanline_ + 1) % 262;
    }
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}