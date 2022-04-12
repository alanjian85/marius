#include "mapper_nrom.h"
using namespace nes;

#include <cassert>

std::uint8_t MapperNrom::readPrg(std::uint16_t addr) {
    assert(addr >= 0x8000);
    if (cartridge_.getPrgRomBanks() == 1) {
        return cartridge_.getPrgRom()[addr & 0x3FFF];
    } else {
        return cartridge_.getPrgRom()[addr - 0x8000];
    }
}

void MapperNrom::writePrg(std::uint16_t addr, std::uint8_t val) {
    assert(false && "Attempted to write to PRG ROM");
}

std::uint8_t MapperNrom::readChr(std::uint16_t addr) {
    assert(addr < 0x2000);
    return cartridge_.getChrRom()[addr];
}

void MapperNrom::writeChr(std::uint16_t addr, std::uint8_t val) {
    assert(false && "Attempted to write to CHR ROM");
}

Mirroring MapperNrom::getMirroring() {
    return cartridge_.getMirroring();
}