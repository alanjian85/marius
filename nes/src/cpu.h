#ifndef NES_CPU_H_
#define NES_CPU_H_

#include <cstdint>

#include "bus.h"

namespace nes {
    class CPU {
    public:
        CPU(Bus& bus);

        void step();
    private:
        std::uint8_t fetch();

        bool execBranch(std::uint8_t opcode);
        bool execImplied(std::uint8_t opcode);
        bool execGroup0(std::uint8_t opcode);
        bool execGroup1(std::uint8_t opcode);
        bool execGroup2(std::uint8_t opcode);

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