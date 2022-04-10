#ifndef NES_EMULATOR_H_
#define NES_EMULATOR_H_

#include <chrono>
#include <iostream>

#include <SDL2/SDL.h>

#include "mappers/mapper.h"
#include "cartridge.h"
#include "cpu_bus.h"
#include "cpu.h"
#include "ppu.h"

namespace nes {
    class Emulator final {
    public:
        Emulator();

        Emulator(const Emulator&) = delete;

        Emulator& operator=(const Emulator&) = delete;

        ~Emulator();

        void run(std::istream& rom);
    private:
        using Clock = std::chrono::high_resolution_clock;

        SDL_Window* window_;
        SDL_Renderer* renderer_;

        Cartridge cartridge_;
        std::unique_ptr<Mapper> mapper_;

        CpuBus cpu_bus_;
        Cpu cpu_;
        Ppu ppu_;

        std::chrono::nanoseconds cycle_interval_;
        Clock::time_point prev_time_;
        Clock::duration elapsed_time_;
    };
}

#endif // NES_EMULATOR_H_