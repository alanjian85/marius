#include "ppu.h"
using namespace nes;

#include <algorithm>
#include <cassert>

#include "cpu.h"

Ppu::Ppu(Framebuffer& framebuffer, PpuBus& ppu_bus)
    : framebuffer_(framebuffer),
      ppu_bus_(ppu_bus)
{
    cycle_ = 0;
    scanline_ = 261;
    vblank_ = true;
    vblank_nmi_ = false;
}

void Ppu::reset() {
    cycle_ = 0;
    scanline_ = 261;
}

void Ppu::cycle() {
    if (scanline_ == 241 && cycle_ == 0) {
        vblank_ = true;
        if (vblank_nmi_) {
            assert(cpu_);
            cpu_->nmi();
        }
    }

    ++cycle_;
    if (cycle_ == 341) {
        cycle_ = 0;
        scanline_ = (scanline_ + 1) % 262;
    }
}

void Ppu::bindCpu(Cpu& cpu) {
    cpu_ = &cpu;
}

void Ppu::setCtrl(std::uint8_t ctrl) {
    vblank_nmi_ = ctrl & 0x80;
}

std::uint8_t Ppu::getStatus() {
    std::uint8_t status = vblank_ << 7;
    vblank_ = false;
    return status;
}