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
)
add_executable(nes ${NES_SRCS})