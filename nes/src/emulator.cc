#include "emulator.h"
using namespace nes;

#include <cassert>
#include <stdexcept>

#include <SDL.h>

#include "components/cpu_bus.h"
#include "components/cpu.h"
#include "components/ppu_bus.h"
#include "components/ppu.h"
#include "io/cartridge.h"
#include "io/controller.h"
#include "mappers/mapper.h"

Emulator::Emulator(Keymap keymap1, Keymap keymap2) 
    : keymap1_(keymap1), keymap2_(keymap2)
{
    cycle_interval_ = std::chrono::nanoseconds(559);

    SDL_Init(SDL_INIT_VIDEO);
}

Emulator::~Emulator() {
    SDL_Quit();
}

void Emulator::run(std::istream& rom) {
    const int width = 1024, height = 960;
    auto window = SDL_CreateWindow(
        "NES",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        0
    );

    auto renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    Cartridge cartridge;
    try {
        rom >> cartridge;
    } catch (std::exception& e) {
        std::cerr << "Error loading ROM: " << e.what() << std::endl;
        return;
    }

    auto mapper = MakeMapper(cartridge);
    assert(mapper);
    Framebuffer framebuffer(renderer, Ppu::kWidth, Ppu::kWidth);
    
    CpuBus cpu_bus(*mapper);
    Cpu cpu(cpu_bus);
    
    PpuBus ppu_bus(*mapper);
    Ppu ppu(framebuffer, ppu_bus, cpu);

    Controller controller1(keymap1_);
    Controller controller2(keymap2_);
    
    cpu_bus.setCpu(cpu);
    cpu_bus.setPpu(ppu);
    cpu_bus.setController1(controller1);
    cpu_bus.setController2(controller2);

    SDL_Rect rect;
    if (static_cast<float>(width) / height > Ppu::kAspect) {
        int screen_width = height * Ppu::kAspect;
        rect.x = (width - screen_width) / 2;
        rect.y = 0;
        rect.w = screen_width;
        rect.h = height;
    } else {
        int screen_height = width / Ppu::kAspect;
        rect.x = 0;
        rect.y = (height - screen_height) / 2;
        rect.w = width;
        rect.h = screen_height;
    }

    bool quit = false;
    auto prev_time = Clock::now();
    auto elapsed_time = prev_time - prev_time;
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

        if (!framebuffer.isLocked()) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_RenderCopy(renderer, framebuffer.getTexture(), nullptr, &rect);

            SDL_RenderPresent(renderer);
        }      
    }

    framebuffer.destroy();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}