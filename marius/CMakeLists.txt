if (EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "-s USE_SDL=2 -O3")
endif()

set(MARIUS_SRCS
    src/components/cpu_bus.cc
    src/components/cpu_bus.h
    src/components/cpu.cc
    src/components/cpu.h
    src/components/palette.h
    src/components/ppu_bus.cc
    src/components/ppu_bus.h
    src/components/ppu.cc
    src/components/ppu.h
    src/graphics/framebuffer.cc
    src/graphics/framebuffer.h
    src/graphics/renderer.cc
    src/graphics/renderer.h
    src/graphics/window.cc
    src/graphics/window.h
    src/io/cartridge.cc
    src/io/cartridge.h
    src/io/controller_keymap.h
    src/io/controller.cc
    src/io/controller.h
    src/mappers/mapper_nrom.cc
    src/mappers/mapper_nrom.h
    src/mappers/mapper.cc
    src/mappers/mapper.h
    src/emulator.cc
    src/emulator.h
    src/settings.cc
    src/settings.h
    src/main.cc
)
add_executable(marius ${MARIUS_SRCS})

find_package(nlohmann_json REQUIRED)
find_package(SDL2 REQUIRED)
find_package(spdlog REQUIRED)

target_include_directories(marius PRIVATE
    src
    ${SDL2_INCLUDE_DIRS}
)

if (EMSCRIPTEN)
    target_link_libraries(marius
        ${SDL2_LIBRARIES}
    )

    configure_file(res/index.html index.html COPYONLY)

    set_target_properties(marius PROPERTIES LINK_FLAGS "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/res/mario.nes@mario.nes")
else()
    target_link_libraries(marius
        nlohmann_json::nlohmann_json
        ${SDL2_LIBRARIES}
        spdlog::spdlog
    )
endif()
