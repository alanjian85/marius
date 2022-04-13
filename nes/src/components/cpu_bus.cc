#include "cpu_bus.h"
using namespace nes;

CpuBus::CpuBus(Mapper& mapper, PpuLatch& ppu_latch)
    : mapper_(mapper),
      ppu_latch_(ppu_latch)
{

}

std::uint8_t CpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return ram_[addr & 0x7FF];
    } else if (addr < 0x4000) {
        return ppu_latch_.read(addr);
    } else if (addr < 0x4020) {
        
    } else {
        return mapper_.readPrg(addr);
    }
    return 0;
}

void CpuBus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0x2000) {
        ram_[addr & 0x7FF] = val;
    } else if (addr < 0x4000) {
        ppu_latch_.write(addr, val);
    } else if (addr < 0x4020) {
    
    } else {
        mapper_.writePrg(addr, val);
    }
}