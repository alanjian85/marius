#include "emulator.h"
using namespace nes;

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#include "components/cpu_bus.h"
#include "components/cpu.h"
#include "components/ppu_bus.h"
#include "components/ppu.h"

Emulator::Emulator(const std::filesystem::path& path, Settings settings)
    : settings_(settings)
{
    cycle_interval_ = std::chrono::nanoseconds(559);

    spdlog::enable_backtrace(settings.dump_size);

    std::ifstream rom(path, std::ios::binary);
    if (!rom.is_open()) {
        throw std::runtime_error("Couldn't open ROM file: " + path.string());
    }
    rom >> cartridge_;
    controller1_.setKeymap(settings.keymap1);
    controller2_.setKeymap(settings.keymap2);

    mapper_ = MakeMapper(cartridge_);
    spdlog::info("Mapper: {}", mapper_->getName());

    width_ = 1024;
    height_ = 960;
    resize();
    window_ = Window(fmt::format("NES {}", path.filename().string()).c_str(), width_, height_, SDL_WINDOW_RESIZABLE);
    renderer_ = Renderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    framebuffer_ = Framebuffer(renderer_, Ppu::kWidth, Ppu::kHeight);
}

void Emulator::run() {
    CpuBus cpu_bus(*mapper_);
    Cpu cpu(cpu_bus);
    
    PpuBus ppu_bus(*mapper_);
    Ppu ppu(framebuffer_, ppu_bus, cpu);
    
    cpu_bus.setCpu(cpu);
    cpu_bus.setPpu(ppu);
    cpu_bus.setController1(controller1_);
    cpu_bus.setController2(controller2_);

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
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            width_ = event.window.data1;
                            height_ = event.window.data2;
                            resize();
                            break;
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

        if (!framebuffer_.isLocked()) {
            renderer_.setDrawColor(0, 0, 0, 255);
            renderer_.clear();

            renderer_.copy(framebuffer_.getTexture(), nullptr, &rect_);

            renderer_.present();
        }      
    }
}

void Emulator::resize() {
    if (static_cast<float>(width_) / height_ > Ppu::kAspect) {
        auto screen_width = static_cast<int>(height_ * Ppu::kAspect);
        rect_.x = (width_ - screen_width) / 2;
        rect_.y = 0;
        rect_.w = screen_width;
        rect_.h = height_;
    } else {
        auto screen_height = static_cast<int>(width_ / Ppu::kAspect);
        rect_.x = 0;
        rect_.y = (height_ - screen_height) / 2;
        rect_.w = width_;
        rect_.h = screen_height;
    }
}