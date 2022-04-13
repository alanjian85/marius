#include "emulator.h"
using namespace nes;

#include <cassert>
#include <stdexcept>

#include <SDL2/SDL.h>

#include "components/cpu_bus.h"
#include "components/cpu.h"
#include "components/ppu_bus.h"
#include "components/ppu.h"
#include "io/cartridge.h"
#include "mappers/mapper.h"

Emulator::Emulator() {
    cycle_interval_ = std::chrono::nanoseconds(559);

    SDL_Init(SDL_INIT_VIDEO);
}

Emulator::~Emulator() {
    SDL_Quit();
}

void Emulator::run(std::istream& rom) {
    const int width = 1280, height = 720;
    auto window = SDL_CreateWindow(
        "NES",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        0
    );

    auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Ppu::kWidth, Ppu::kHeight);
    
    Cartridge cartridge;
    try {
        rom >> cartridge;
    } catch (std::exception& e) {
        std::cerr << "Error loading ROM: " << e.what() << std::endl;
        return;
    }

    auto mapper = MakeMapper(cartridge);
    assert(mapper);
    Framebuffer framebuffer(Ppu::kWidth, Ppu::kWidth);
    PpuBus ppu_bus(*mapper);
    Ppu ppu(framebuffer, ppu_bus);
    CpuBus cpu_bus(*mapper, ppu);
    Cpu cpu(cpu_bus);
    ppu.bindCpu(cpu);

    bool quit = false;
    auto prev_time = Clock::now();
    auto elapsed_time = prev_time - prev_time;
    int screen_width = height / Ppu::kAspect;
    int screen_x = (width - screen_width) / 2.0f;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        elapsed_time += Clock::now() - prev_time;
        while (elapsed_time > cycle_interval_) {
            cpu.cycle();

            // PPU clock is three times faster than CPU
            ppu.cycle();
            ppu.cycle();
            ppu.cycle();

            elapsed_time -= cycle_interval_;
        }
        prev_time = Clock::now();

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        SDL_UpdateTexture(texture, nullptr, framebuffer.getPixels(), sizeof(std::uint32_t) * Ppu::kWidth);

        SDL_Rect rect = { screen_x, 0, screen_width, height };
        SDL_RenderCopy(renderer, texture, nullptr, &rect);

        SDL_RenderPresent(renderer);        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}