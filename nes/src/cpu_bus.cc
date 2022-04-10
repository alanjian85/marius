#include "cpu_bus.h"
using namespace nes;

#include <cassert>

CpuBus::CpuBus() {
    mapper_ = nullptr;
    ppu_ = nullptr;
}

void CpuBus::setMapper(Mapper* mapper) {
    mapper_ = mapper;
}

void CpuBus::setPpu(Ppu* ppu) {
    ppu_ = ppu;
}

std::uint8_t CpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return ram_[addr & 0x7FF];
    } else if (addr < 0x4000) {
        assert(ppu_);
        switch (addr & 0x7) {
            case 0x2:
                return ppu_->getStatus();
        }
    } else if (addr < 0x4020) {
    
    } else {
        assert(mapper_);
        return mapper_->readPrg(addr);
    }
    return 0;
}

void CpuBus::write(std::uint16_t addr, std::uint8_t val) {
    if (addr < 0x2000) {
        ram_[addr & 0x7FF] = val;
    } else if (addr < 0x4000) {
        
    } else if (addr < 0x4020) {
    
    } else {
        assert(mapper_);
        mapper_->writePrg(addr, val);
    }
}