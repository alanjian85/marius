#include "mapper.h"
using namespace nes;

#include <stdexcept>

#include <spdlog/fmt/fmt.h>

#include "mapper_nrom.h"

Mapper::Mapper(Cartridge& cartridge)
    : cartridge_(cartridge)
{

}

std::unique_ptr<Mapper> nes::MakeMapper(Cartridge& cartridge) {
    switch (cartridge.getMapperNum()) {
        case 0x00:
            return std::make_unique<MapperNrom>(cartridge);
        default:
            throw std::invalid_argument("Unknown mapper: " + std::to_string(cartridge.getMapperNum()));
    }
}