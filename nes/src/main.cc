#include <cassert>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <iostream>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "emulator.h"
#include "settings.h"
using namespace nes;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

EM_JS(int, GetCanvasWidth, (), {
    return Module.canvas.width
});

EM_JS(int, GetCanvasHeight, (), {
    return Module.canvas.height
});

#endif

int main(int argc, char** argv) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: nes <ROM>";
            return EXIT_FAILURE;
        }

        spdlog::info("ROM Path: {}", argv[1]);

        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        } else {
            spdlog::info("SDL initialized");
        }

        {
            Settings settings("settings.json");
#ifndef __EMSCRIPTEN__
            Emulator emulator(1024, 960, argv[1], settings);
            emulator.run();
#else
            Emulator emulator(GetCanvasWidth(), GetCanvasHeight(), path, settings);
            emscripten_set_main_loop_arg(
                [](void* arg) {
                    static_cast<Emulator*>(arg)->loop();
                },
            &emulator, 0, true);
#endif
        }

        SDL_Quit();
        spdlog::info("SDL quit");
    } catch (std::exception& e) {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}