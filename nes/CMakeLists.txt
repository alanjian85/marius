set(NES_SRCS
    src/cartridge.cc
    src/cartridge.h
    src/cpu_bus.cc
    src/cpu_bus.h
    src/cpu.cc
    src/cpu.h
    src/emulator.cc
    src/emulator.h
    src/main.cc
    src/mappers/mapper_nrom.cc
    src/mappers/mapper_nrom.h
    src/mappers/mapper.cc
    src/mappers/mapper.h
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