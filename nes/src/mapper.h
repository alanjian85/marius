#ifndef NES_MAPPER_H_
#define NES_MAPPER_H_

#include <cstdint>
#include <memory>

#include "cartridge.h"

namespace nes {
    class Mapper {
    public:
        [[nodiscard]] virtual std::uint8_t readPrg(std::uint16_t addr) = 0;

        virtual void writePrg(std::uint16_t addr, std::uint8_t val) = 0;
    
        [[nodiscard]] virtual std::uint8_t readChr(std::uint16_t addr) = 0;

        virtual void writeChr(std::uint16_t addr, std::uint8_t val) = 0;
    };

    std::unique_ptr<Mapper> MakeMapper(Cartridge& cartridge);
}

#endif // NES_MAPPER_H_