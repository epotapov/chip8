#include <iostream>
#include "chip8.h"

int main(int argc, char** argv) {
    Chip8 chip8(argv[1]);
    std::cout << "Hello, World!" << std::endl;
    return 0;
}