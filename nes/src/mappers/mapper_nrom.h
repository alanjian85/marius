#ifndef NES_MAPPER_NROM_H_
#define NES_MAPPER_NROM_H_

#include "mapper.h"

namespace nes {
    class MapperNrom final : public Mapper {
    public:
        using Mapper::Mapper;

        virtual std::uint8_t readPrg(std::uint16_t addr) override;

        virtual void writePrg(std::uint16_t addr, std::uint8_t val) override;
    
        virtual std::uint8_t readChr(std::uint16_t addr) override;

        virtual void writeChr(std::uint16_t addr, std::uint8_t val) override;

        virtual Mirroring getMirroring() override;
    };
}

#endif // NES_MAPPER_NROM_H_