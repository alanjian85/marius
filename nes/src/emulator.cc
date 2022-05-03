#include "emulator.h"
using namespace nes;

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include "components/cpu_bus.h"
#include "components/cpu.h"
#include "components/ppu_bus.h"
#include "components/ppu.h"
#include "io/cartridge.h"
#include "io/controller.h"
#include "mappers/mapper.h"

Emulator::Emulator(const std::filesystem::path& path, Keymap keymap1, Keymap keymap2) 
    : keymap1_(keymap1), keymap2_(keymap2)
{
    width_ = 1024;
    height_ = 960;
    cycle_interval_ = std::chrono::nanoseconds(559);

    rom_.open(path);
    if (!rom_.is_open()) {
        spdlog::error("Couldn't open ROM file: {}", path.string());
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
    }

    window_ = SDL_CreateWindow(
        fmt::format("NES {}", path.filename().string()).c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width_, height_,
        0
    );
    if (!window_) {
        spdlog::error("Failed to create window: {}", SDL_GetError());
    }

    renderer_ = SDL_CreateRenderer(
        window_, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!renderer_) {
        spdlog::error("Failed to create renderer: {}", SDL_GetError());
    }
}

Emulator::~Emulator() {
    SDL_DestroyRenderer(renderer_);
    spdlog::info("Renderer destroyed");
    SDL_DestroyWindow(window_);
    spdlog::info("Window destroyed");
    SDL_Quit();
    spdlog::info("SDL quit");
}

void Emulator::run() {
    Cartridge cartridge;
    rom_ >> cartridge;

    auto mapper = MakeMapper(cartridge);
    if (!mapper) {
        spdlog::error("Unknown mapper");
        return;
    }
    spdlog::info("Mapper: {}", mapper->getName());

    Framebuffer framebuffer(renderer_, Ppu::kWidth, Ppu::kHeight);
    
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
    if (static_cast<float>(width_) / height_ > Ppu::kAspect) {
        int screen_width = height_ * Ppu::kAspect;
        rect.x = (width_ - screen_width) / 2;
        rect.y = 0;
        rect.w = screen_width;
        rect.h = height_;
    } else {
        int screen_height = width_ / Ppu::kAspect;
        rect.x = 0;
        rect.y = (height_ - screen_height) / 2;
        rect.w = width_;
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
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_F1) {
                        spdlog::dump_backtrace();
                    }
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
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
            SDL_RenderClear(renderer_);

            SDL_RenderCopy(renderer_, framebuffer.getTexture(), nullptr, &rect);

            SDL_RenderPresent(renderer_);
        }      
    }
}