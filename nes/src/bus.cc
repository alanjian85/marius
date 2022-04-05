#include "bus.h"
using namespace nes;

#include <cassert>

std::uint8_t Bus::read(std::uint16_t addr) const {
    if (addr < 0xFFFF) {
        return ram_[addr & 0xFFFF];
    } else {
        assert(false);
    }
}

void Bus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0xFFFF) {
        ram_[addr & 0xFFFF] = val;
    } else {
        assert(false);
    }
}