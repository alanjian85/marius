#include "mapper.h"
using namespace nes;

#include "mapper_nrom.h"

Mapper::Mapper(Cartridge& cartridge)
    : cartridge_(cartridge)
{

}

std::unique_ptr<Mapper> nes::MakeMapper(Cartridge& cartridge) {
    switch (cartridge.getMapperNum()) {
        case 0x00:
            return std::make_unique<MapperNROM>(cartridge);
    }
    return nullptr;
}