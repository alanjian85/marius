#ifndef NES_CPU_BUS_H_
#define NES_CPU_BUS_H_

#include <array>
#include <cstdint>

#include "mappers/mapper.h"

namespace nes {
    class CpuBus {
    public:
        void setMapper(Mapper* mapper);

        [[nodiscard]] std::uint8_t read(std::uint16_t addr) const;

        void write(std::uint16_t addr, std::uint8_t val);
    private:
        std::array<std::uint8_t, 0x800> ram_;
        Mapper* mapper_;
    };
}

#endif // NES_CPU_BUS_H_