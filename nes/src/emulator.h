#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <iostream>

#include "cartridge.h"
#include "cpu_bus.h"
#include "cpu.h"

namespace nes {
    class Emulator {
    public:
        Emulator();

        void run(std::istream& rom);
    private:
        using Clock = std::chrono::high_resolution_clock;

        Cartridge cartridge_;

        CpuBus cpu_bus_;
        Cpu cpu_;

        std::chrono::nanoseconds cycle_interval_;
        Clock::time_point prev_time_;
        Clock::duration elapsed_time_;
    };
}

#endif // NES_EMULATOR_H_