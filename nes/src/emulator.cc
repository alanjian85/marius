#include "emulator.h"
using namespace nes;

#include <functional>

#include <SDL.h>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Emulator::Emulator(const std::filesystem::path& path, const Settings& settings)
    : settings_(settings),
      controller1_(settings.keymap1),
      controller2_(settings.keymap2),
      cpu_bus_(cpu_, ppu_, controller1_, controller2_),
      cpu_(cpu_bus_),
      ppu_(framebuffer_, ppu_bus_, cpu_)
{
    cycle_interval_ = std::chrono::nanoseconds(559);

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

    width_ = 1024;
    height_ = 960;
    resize();
    window_ = Window(fmt::format("NES {}", path.filename().string()).c_str(), width_, height_, SDL_WINDOW_RESIZABLE);
    renderer_ = Renderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    framebuffer_ = Framebuffer(renderer_, Ppu::kWidth, Ppu::kHeight);
}

void Emulator::run() {


#ifndef __EMSCRIPTEN__
    bool quit = false;
    while (!quit) {
#else
    std::function<void()> callback = [this]() {
#endif
        static auto prev_time = Clock::now();
        static auto elapsed_time = prev_time - prev_time;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
#ifndef __EMSCRIPTEN__
                case SDL_QUIT:
                    quit = true;
                    break;
#endif
                case SDL_KEYDOWN:
#ifndef __EMSCRIPTEN__
                    if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        quit = true;
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
            cpu_.cycle();

            // PPU clock is three times faster than CPU
            ppu_.cycle();
            ppu_.cycle();
            ppu_.cycle();

            elapsed_time -= cycle_interval_;
        }
        prev_time = Clock::now();

        if (!framebuffer_.isLocked()) {
            renderer_.setDrawColor(0, 0, 0, 255);
            renderer_.clear();

            renderer_.copy(framebuffer_.getTexture(), nullptr, &rect_);

            renderer_.present();
        }
#ifndef __EMSCRIPTEN__
    }
#else
    };
    emscripten_set_main_loop_arg(
        [](void* callback) {
            (*static_cast<std::function<void()>*>(callback))();
        },
        &callback,
        0, true
    );
#endif
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