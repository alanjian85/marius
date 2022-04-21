#include "controller.h"
using namespace nes;

#include <SDL.h>

void Controller::write(std::uint8_t val) {
    if (strobe_ && !(val & 0x01)) {
        update();
    }
    strobe_ = val & 0x01;
}

std::uint8_t Controller::read() {
    if (strobe_) {
        update();
    }
    std::uint8_t result = state_ & 0x01 | 0x40;
    state_ = state_ >> 1 | 0x80;
    return result;
}

void Controller::update() {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    bool a = state[SDL_SCANCODE_J];
    bool b = state[SDL_SCANCODE_K];
    bool select = state[SDL_SCANCODE_RSHIFT];
    bool start = state[SDL_SCANCODE_RETURN];
    bool up = state[SDL_SCANCODE_W];
    bool down = state[SDL_SCANCODE_S];
    bool left = state[SDL_SCANCODE_A];
    bool right = state[SDL_SCANCODE_D];
    state_ = 
        a      << 0 |
        b      << 1 |
        select << 2 |
        start  << 3;
    
    if (up != down) {
        state_ |= 
            up   << 4 |
            down << 5;
    }

    if (right != left) {
        state_ |= 
            left  << 6 |
            right << 7;
    }
}