#include "cpu_bus.h"
using namespace nes;

CpuBus::CpuBus(Mapper& mapper)
    : mapper_(mapper)
{
    cpu_ = nullptr;
    ppu_ = nullptr;
}

std::uint8_t CpuBus::read(std::uint16_t addr) const {
    if (addr < 0x2000) {
        return ram_[addr & 0x7FF];
    } else if (addr < 0x4000) {
        switch (addr & 0x7) {
            case 0x2:
                return ppu_->getStatus();
            case 0x4:
                return ppu_->getOamData();
            case 0x7:
                return ppu_->getData();
        }
    } else if (addr < 0x4020) {
        switch (addr) {
            case 0x4016:
                return controller1_->read();
            case 0x4017:
                return controller2_->read();
        }
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
                ppu_->setCtrl(val);
                break;
            case 0x1:
                ppu_->setMask(val);
                break;
            case 0x3:
                ppu_->setOamAddr(val);
                break;
            case 0x4:
                ppu_->setOamData(val);
                break;
            case 0x5:
                ppu_->setScroll(val);
                break;
            case 0x6:
                ppu_->setAddr(val);
                break;
            case 0x7:
                ppu_->setData(val);
                break;
        }
    } else if (addr < 0x4020) {
        switch (addr) {
            case 0x4014:
                cpu_->oamDma(val);
                break;
            case 0x4016:
                controller1_->write(val);
                controller2_->write(val);
                break;
        }
    } else {
        mapper_.writePrg(addr, val);
    }
}

void CpuBus::setCpu(Cpu& cpu) {
    cpu_ = &cpu;
}

void CpuBus::setPpu(Ppu& ppu) {
    ppu_ = &ppu;
}

void CpuBus::setController1(Controller& controller1) {
    controller1_ = &controller1;
}

void CpuBus::setController2(Controller& controller2) {
    controller2_ = &controller2;
}