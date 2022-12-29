#include "controller.h"
using namespace nes;

#include <SDL.h>

Controller::Controller(const ControllerKeymap& keymap) : keymap_(keymap) {}

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
    bool a = state[keymap_.a];
    bool b = state[keymap_.b];
    bool select = state[keymap_.select];
    bool start = state[keymap_.start];
    bool up = state[keymap_.up];
    bool down = state[keymap_.down];
    bool left = state[keymap_.left];
    bool right = state[keymap_.right];
    state_ = a << 0 | b << 1 | select << 2 | start << 3;

    if (up != down) {
        state_ |= up << 4 | down << 5;
    }

    if (right != left) {
        state_ |= left << 6 | right << 7;
    }
}