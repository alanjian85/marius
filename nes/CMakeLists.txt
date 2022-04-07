set(NES_SRCS
    src/bus.cc
    src/bus.h
    src/cpu.cc
    src/cpu.h
    src/emulator.cc
    src/emulator.h
    src/main.cc
)
add_executable(nes ${NES_SRCS})