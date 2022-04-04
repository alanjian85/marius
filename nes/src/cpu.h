#ifndef NES_CPU_H_
#define NES_CPU_H_

#include <cstdint>

#include "bus.h"

namespace {
    class CPU {
    public:
        CPU(Bus& bus);
    private:
        Bus& bus_;

        std::uint8_t a_;
        std::uint8_t x_;
        std::uint8_t y_;
        std::uint16_t pc_;
        std::uint8_t s_;
        std::uint8_t p_;
    };
}

#endif // NES_CPU_H_