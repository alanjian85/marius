#include "cpu_bus.h"
using namespace nes;

#include <iostream>

CpuBus::CpuBus(Mapper& mapper, Ppu& ppu)
    : mapper_(mapper),
      ppu_(ppu)
{

}

std::uint8_t CpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return ram_[addr & 0x7FF];
    } else if (addr < 0x4000) {
        switch (addr & 0x7) {
            case 0x2:
                return ppu_.getStatus();
            case 0x7:
                return ppu_.getData();
        }
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
        switch (addr & 0x7) {
            case 0x0:
                ppu_.setCtrl(val);
                break;
            case 0x6:
                ppu_.setAddr(val);
                break;
            case 0x7:
                ppu_.setData(val);
                break;
        }
    } else if (addr < 0x4020) {
    
    } else {
        mapper_.writePrg(addr, val);
    }
}