#ifndef NES_COMPONENTS_CPU_H_
#define NES_COMPONENTS_CPU_H_

#include <bitset>
#include <cstdint>
#include <cstdlib>

#include "cpu_bus.h"

namespace nes {
    class Cpu final {
    public:
        Cpu(CpuBus& bus);

        void irq();

        void nmi();

        void reset();

        void cycle();
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
            kResetVector = 0xFFFC,
            kStackBase = 0x0100
        };

        [[nodiscard]] std::uint8_t readByte(std::uint16_t addr);
        [[nodiscard]] std::uint16_t readAddress(std::uint16_t addr);
        void writeByte(std::uint16_t addr, std::uint8_t val);

        [[nodiscard]] std::uint8_t pull();
        void push(std::uint8_t val);

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

        [[nodiscard]] bool execBranch(std::uint8_t opcode);
        [[nodiscard]] bool execImplied(std::uint8_t opcode);
        [[nodiscard]] bool execGroup0(std::uint8_t opcode);
        [[nodiscard]] bool execGroup1(std::uint8_t opcode);
        [[nodiscard]] bool execGroup2(std::uint8_t opcode);

        CpuBus& bus_;
        int cycle_;
        std::uint16_t addr_;

        std::uint8_t a_;
        std::uint8_t x_;
        std::uint8_t y_;
        std::uint16_t pc_;
        std::uint8_t s_;
        std::uint8_t p_;
    };
}

#endif // NES_COMPONENTS_CPU_H_