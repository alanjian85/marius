#include "emulator.h"
using namespace nes;

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include "components/cpu_bus.h"
#include "components/cpu.h"
#include "components/ppu_bus.h"
#include "components/ppu.h"
#include "io/controller.h"
#include "mappers/mapper.h"

Emulator::Emulator(const std::filesystem::path& path, Settings settings)
    : settings_(settings)
{
    cycle_interval_ = std::chrono::nanoseconds(559);

    spdlog::enable_backtrace(settings.dump_size);

    std::ifstream rom(path, std::ios::binary);
    if (!rom.is_open()) {
        spdlog::error("Couldn't open ROM file: {}", path.string());
    }
    rom >> cartridge_;

    window_ = Window(fmt::format("NES {}", path.filename().string()).c_str(), 1024, 960);
    renderer_ = Renderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void Emulator::run() {
    auto mapper = MakeMapper(cartridge_);
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

    Controller controller1(settings_.keymap1);
    Controller controller2(settings_.keymap2);
    
    cpu_bus.setCpu(cpu);
    cpu_bus.setPpu(ppu);
    cpu_bus.setController1(controller1);
    cpu_bus.setController2(controller2);

    SDL_Rect rect;
    if (static_cast<float>(window_.getWidth()) / window_.getHeight() > Ppu::kAspect) {
        auto screen_width = static_cast<int>(window_.getHeight() * Ppu::kAspect);
        rect.x = (window_.getWidth() - screen_width) / 2;
        rect.y = 0;
        rect.w = screen_width;
        rect.h = window_.getHeight();
    } else {
        auto screen_height = static_cast<int>(window_.getWidth() / Ppu::kAspect);
        rect.x = 0;
        rect.y = (window_.getHeight() - screen_height) / 2;
        rect.w = window_.getWidth();
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
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        quit = true;
                    }
                    if (event.key.keysym.scancode == settings_.dump) {
                        spdlog::dump_backtrace();
                    }
                    if (event.key.keysym.scancode == settings_.reset) {
                        cpu.reset();
                        ppu.reset();
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
            renderer_.setDrawColor(0, 0, 0, 255);
            renderer_.clear();

            renderer_.copy(framebuffer.getTexture(), nullptr, &rect);

            renderer_.present();
        }      
    }
}