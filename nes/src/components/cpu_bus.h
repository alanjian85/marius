#ifndef NES_COMPONENTS_CPU_BUS_H_
#define NES_COMPONENTS_CPU_BUS_H_

#include <array>
#include <cstdint>

#include "cpu.h"
#include "io/controller.h"
#include "mappers/mapper.h"
#include "ppu.h"

namespace nes {
    class CpuBus final {
    public:
        CpuBus(Cpu& cpu, Ppu& ppu, Controller& controller1, Controller& controller2);

        void setMapper(Mapper& mapper);

        [[nodiscard]] std::uint8_t read(std::uint16_t addr) const;

        void write(std::uint16_t addr, std::uint8_t val);
    private:
        std::array<std::uint8_t, 0x800> ram_;

        Cpu& cpu_;
        Ppu& ppu_;

        Controller& controller1_;
        Controller& controller2_;

        Mapper* mapper_;
    };
}

#endif // NES_COMPONENTS_CPU_BUS_H_