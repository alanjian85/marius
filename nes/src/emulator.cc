#include "emulator.h"
using namespace nes;

#include <cmath>
#include <functional>

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

Emulator::Emulator(int width, int height, const std::filesystem::path& path, const Settings& settings)
    : settings_(settings),
      controller1_(settings.keymap1),
      controller2_(settings.keymap2),
      cpu_bus_(cpu_, ppu_, controller1_, controller2_),
      cpu_(cpu_bus_),
      ppu_(framebuffer_, ppu_bus_, cpu_)
{
    cycle_interval_ = std::chrono::nanoseconds(559);
    prev_time_ = Clock::now();
    elapsed_time_ = prev_time_ - prev_time_;

    spdlog::enable_backtrace(settings.dump_size);

    std::ifstream rom(path, std::ios::binary);
    if (!rom.is_open()) {
        throw std::runtime_error("Couldn't open ROM file: " + path.string());
    }
    rom >> cartridge_;

    mapper_ = MakeMapper(cartridge_);
    spdlog::info("Mapper: {}", mapper_->getName());
    cpu_bus_.setMapper(*mapper_);
    ppu_bus_.setMapper(*mapper_);
    cpu_.reset();
    ppu_.reset();

    window_ = Window(fmt::format("NES {}", path.filename().string()).c_str(), width, height, SDL_WINDOW_RESIZABLE);
    renderer_ = Renderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    framebuffer_ = Framebuffer(renderer_, Ppu::kWidth, Ppu::kHeight);

#ifndef __EMSCRIPTEN__
    quit_ = false;
    resize(width, height);
#endif
}

void Emulator::loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
#ifndef __EMSCRIPTEN__
            case SDL_QUIT:
                quit_ = true;
                break;
#endif
            case SDL_KEYDOWN:
#ifndef __EMSCRIPTEN__
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    quit_ = true;
                }
#endif
                if (event.key.keysym.scancode == settings_.dump) {
                    spdlog::dump_backtrace();
                }
                if (event.key.keysym.scancode == settings_.reset) {
                    cpu_.reset();
                    ppu_.reset();
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
#ifndef __EMSCRIPTEN__
                    case SDL_WINDOWEVENT_RESIZED:
                        resize(event.window.data1, event.window.data2);
                        break;
#endif
                }
                break;
        }
    }

    elapsed_time_ += Clock::now() - prev_time_;
    prev_time_ = Clock::now();
    while (elapsed_time_ > cycle_interval_) {
        cpu_.cycle();

        // PPU clock is three times faster than CPU
        ppu_.cycle();
        ppu_.cycle();
        ppu_.cycle();

        elapsed_time_ -= cycle_interval_;
    }

    renderer_.setDrawColor(0, 0, 0, 255);
    renderer_.clear();

#ifndef __EMSCRIPTEN__
    renderer_.copy(framebuffer_.getTexture(), nullptr, &rect_);
#else
    renderer_.copy(framebuffer_.getTexture(), nullptr, nullptr);
#endif

    renderer_.present();
}

#ifndef __EMSCRIPTEN__
void Emulator::run() {
    while (!quit_) {
        loop();
    }
}

void Emulator::resize(int width, int height) {
    auto scale = std::min(
        static_cast<float>(width) / Ppu::kWidth,
        static_cast<float>(height) / Ppu::kHeight
    );
    rect_.w = static_cast<int>(Ppu::kWidth * scale);
    rect_.h = static_cast<int>(Ppu::kHeight * scale);
    rect_.x = (width - rect_.w) / 2;
    rect_.y = (height - rect_.h) / 2;
}
#endif