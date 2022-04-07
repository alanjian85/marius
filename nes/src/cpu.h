#ifndef NES_CPU_H_
#define NES_CPU_H_

#include <bitset>
#include <cstdint>
#include <cstdlib>

#include "bus.h"

namespace nes {
    class Cpu {
    public:
        Cpu(Bus& bus);

        void irq();

        void nmi();

        void reset();

        int step();
    private:
        enum {
            kC = 1 << 0,
            kZ = 1 << 1,
            kI = 1 << 2,
            kD = 1 << 3,
            kB = 1 << 4,
            kV = 1 << 6,
            kN = 1 << 7
        };

        enum {
            kBrkVector = 0xFFFE,
            kIrqVector = 0xFFFE,
            kNmiVector = 0xFFFA,
            kResetVector = 0xFFFC
        };

        static constexpr std::uint16_t kStackBase = 0x0100;

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

        void setC(bool val);
        void setZ(bool val);
        void setI(bool val);
        void setD(bool val);
        void setV(bool val);
        void setN(bool val);
        void setZN(std::uint8_t val);

        void push(std::uint8_t val);
        std::uint8_t pull();

        bool execBranch(std::uint8_t opcode);
        bool execImplied(std::uint8_t opcode);
        bool execGroup0(std::uint8_t opcode);
        bool execGroup1(std::uint8_t opcode);
        bool execGroup2(std::uint8_t opcode);

        Bus& bus_;
        int cycles_;
        std::uint16_t addr_;

        std::uint8_t a_;
        std::uint8_t x_;
        std::uint8_t y_;
        std::uint16_t pc_;
        std::uint8_t s_;
        std::uint8_t p_;
    };
}

#endif // NES_CPU_H_