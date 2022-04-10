#include "emulator.h"
using namespace nes;

#include <iostream>
#include <stdexcept>

Emulator::Emulator()
    : cpu_(cpu_bus_)
{
    cycle_interval_ = std::chrono::nanoseconds(559);

    SDL_Init(SDL_INIT_VIDEO);
}

Emulator::~Emulator() {
    SDL_Quit();
}

void Emulator::run(std::istream& rom) {
    try {
        rom >> cartridge_;
    } catch (std::exception& e) {
        std::cerr << "Error loading ROM: " << e.what() << std::endl;
        return;
    }
    mapper_ = MakeMapper(cartridge_);
    cpu_bus_.setMapper(mapper_.get());
    cpu_.reset();

    SDL_CreateWindowAndRenderer(
        1280, 720,
        0,
        &window_,
        &renderer_
    );

    bool quit = false;
    prev_time_ = Clock::now();
    elapsed_time_ = prev_time_ - prev_time_;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        elapsed_time_ += Clock::now() - prev_time_;
        while (elapsed_time_ > cycle_interval_) {
            cpu_.cycle();
            elapsed_time_ -= cycle_interval_;
        }
        prev_time_ = Clock::now();

        SDL_RenderPresent(renderer_);        
    }

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
}