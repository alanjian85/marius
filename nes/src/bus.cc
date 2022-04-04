#include "bus.h"
using namespace nes;

#include <cassert>

std::byte Bus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return ram_[addr & 0x7ff];
    }
    assert(false);
}

void Bus::write(std::uint16_t addr, std::byte val) {
    if (addr < 0x2000) {
        ram_[addr & 0x7ff] = val;
    }
    assert(false);
}