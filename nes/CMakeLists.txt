set(NES_SRCS
    src/components/cpu_bus.cc
    src/components/cpu_bus.h
    src/components/cpu.cc
    src/components/cpu.h
    src/components/ppu_bus.cc
    src/components/ppu_bus.h
    src/components/ppu_latch.cc
    src/components/ppu_latch.h
    src/components/ppu.cc
    src/components/ppu.h
    src/io/cartridge.cc
    src/io/cartridge.h
    src/io/framebuffer.cc
    src/io/framebuffer.h
    src/mappers/mapper_nrom.cc
    src/mappers/mapper_nrom.h
    src/mappers/mapper.cc
    src/mappers/mapper.h
    src/emulator.cc
    src/emulator.h
    src/main.cc
)
add_executable(nes ${NES_SRCS})

find_package(SDL2 REQUIRED)

target_include_directories(nes PRIVATE
    src
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(nes
    ${SDL2_LIBRARIES}
)