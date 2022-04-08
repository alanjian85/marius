#include "cpu_bus.h"
using namespace nes;

#include <cassert>

std::uint8_t CpuBus::read(std::uint16_t addr) const {
    if (addr <= 0xFFFF) {
        return ram_[addr & 0xFFFF];
    } else {
        assert(false);
    }
}

void CpuBus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr <= 0xFFFF) {
        ram_[addr & 0xFFFF] = val;
    } else {
        assert(false);
    }
}