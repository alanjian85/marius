#ifndef NES_BUS_H_
#define NES_BUS_H_

#include <array>
#include <cstddef>
#include <cstdint>

namespace nes {
    class Bus {
    public:
        std::byte read(std::uint16_t addr) const;

        void write(std::uint16_t addr, std::byte val);
    private:
        std::array<std::byte, 0x800> ram_;
    };
}

#endif // NES_BUS_H_