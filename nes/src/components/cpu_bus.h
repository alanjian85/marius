#ifndef NES_COMPONENTS_CPU_BUS_H_
#define NES_COMPONENTS_CPU_BUS_H_

#include <array>
#include <cstdint>

#include "cpu.h"
#include "mappers/mapper.h"
#include "ppu.h"

namespace nes {
    class CpuBus final {
    public:
        CpuBus(Mapper& mapper);

        [[nodiscard]] std::uint8_t read(std::uint16_t addr) const;

        void write(std::uint16_t addr, std::uint8_t val);

        void setCpu(Cpu& cpu);

        void setPpu(Ppu& ppu);
    private:
        std::array<std::uint8_t, 0x800> ram_;
        Mapper& mapper_;
        Cpu* cpu_;
        Ppu* ppu_;
    };
}

#endif // NES_COMPONENTS_CPU_BUS_H_