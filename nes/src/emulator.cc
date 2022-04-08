#include "emulator.h"
using namespace nes;

#include <iostream>
#include <stdexcept>

Emulator::Emulator()
    : cpu_(cpu_bus_)
{
    cycle_interval_ = std::chrono::nanoseconds(559);
}

void Emulator::run(std::istream& rom) {
    try {
        rom >> cartridge_;
    } catch (std::exception& e) {
        std::cerr << "Error loading ROM: " << e.what() << std::endl;
        return;
    }
    cpu_.reset();

    prev_time_ = Clock::now();
    elapsed_time_ = prev_time_ - prev_time_;
    for (;;) {
        elapsed_time_ += Clock::now() - prev_time_;
        while (elapsed_time_ > cycle_interval_) {
            cpu_.cycle();
            elapsed_time_ -= cycle_interval_;
        }
        prev_time_ = Clock::now();
    }
}