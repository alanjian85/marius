#include "ppu_latch.h"
using namespace nes;

PpuLatch::PpuLatch(Ppu& ppu)
    : ppu_(ppu)
{
    val_ = 0x00;
}

void PpuLatch::write(std::uint16_t addr, std::uint8_t val) {
    val_ = val;
    switch (addr & 0x7) {
        case 0x1:
            ppu_.setCtrl(val);
            break;
    }
}

std::uint8_t PpuLatch::read(std::uint16_t addr) {
    switch (addr & 0x7) {
        case 0x2:
            val_ = ppu_.getStatus();
            break;
    }
    return val_;
}