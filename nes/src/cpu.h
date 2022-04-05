#ifndef NES_CPU_H_
#define NES_CPU_H_

#include <bitset>
#include <cstdint>
#include <cstdlib>

#include "bus.h"

namespace nes {
    class CPU {
    public:
        CPU(Bus& bus);

        void reset();

        void step();
    private:
        static constexpr std::size_t kC = 0;
        static constexpr std::size_t kZ = 1;
        static constexpr std::size_t kI = 2;
        static constexpr std::size_t kD = 3;
        static constexpr std::size_t kV = 6;
        static constexpr std::size_t kN = 7;

        void addrImmediate();
        void addrRelative();
        void addrAbsolute();
        void addrZeroPage();
        void addrIndirect();
        void addrAbsoluteX();
        void addrAbsoluteY();
        void addrZeroPageX();
        void addrZeroPageY();
        void addrIndexedIndirect();
        void addrIndirectIndexed();

        void setZ(std::uint8_t val);
        void setN(std::uint8_t val);

        void push(std::uint8_t val);
        std::uint8_t pull();

        bool execBranch(std::uint8_t opcode);
        bool execImplied(std::uint8_t opcode);
        bool execGroup0(std::uint8_t opcode);
        bool execGroup1(std::uint8_t opcode);
        bool execGroup2(std::uint8_t opcode);

        Bus& bus_;
        std::uint16_t addr_;

        std::uint8_t a_;
        std::uint8_t x_;
        std::uint8_t y_;
        std::uint16_t pc_;
        std::uint8_t s_;
        std::bitset<8> p_;
    };
}

#endif // NES_CPU_H_