#include "ppu_bus.h"
using namespace nes;

PpuBus::PpuBus(Mapper& mapper)
    : mapper_(mapper)
{

}

std::uint8_t PpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return mapper_.readChr(addr);
    } else if (addr < 0x3F00) {
        return ram_[addr & 0xFFF];
    } else {

    }
    return 0;
}

void PpuBus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0x2000) {
        mapper_.writeChr(addr, val);
    } else if (addr < 0x3F00) {
        ram_[addr & 0xFFF] = val; 
    } else {

    }
}

std::uint8_t PpuBus::readOam(std::uint8_t addr) const {
    return oam_[addr];
}

void PpuBus::writeOam(std::uint8_t addr, std::uint8_t val) {
    oam_[addr] = val;
}