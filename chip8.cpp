#include <algorithm>
#include <fstream>

#include "chip8.h"

Chip8::Chip8(char const* file)
    : rng(std::random_device{}()), 
      dist(0, 255) {
    std::ifstream readFile(file, std::ios::binary | std::ios::in | std::ios::ate);

    if (readFile.is_open()) {
        std::streampos size = readFile.tellg();

        char* buffer = new char[size];

        readFile.seekg(0, std::ios::beg);
        readFile.read(buffer, size);
        readFile.close();


        for (int i = 0; i < size; ++i) {
            memory[INST_START + i] = buffer[i];
        }

        delete [] buffer;

        for (int i = 0; i < FONT_SIZE; ++i) {
            memory[FONT_START + i] = fontset[i];
        }
    }
}

void Chip8::readOpcode() {
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: OP_00E0(); break;
                case 0x00EE: OP_00EE(); break;
                default: OP_0NNN(); break;
            }
            break;
        case 0x1000: OP_1NNN(); break;
        case 0x2000: OP_2NNN(); break;
        case 0x3000: OP_3XNN(); break;
        case 0x4000: OP_4XNN(); break;
        case 0x5000: OP_5XY0(); break;
        case 0x6000: OP_6XNN(); break;
        case 0x7000: OP_7XNN(); break;
        case 0x8000:
            switch (opcode & 0x000F) {
                case 0x0000: OP_8XY0(); break;
                case 0x0001: OP_8XY1(); break;
                case 0x0002: OP_8XY2(); break;
                case 0x0003: OP_8XY3(); break;
                case 0x0004: OP_8XY4(); break;
                case 0x0005: OP_8XY5(); break;
                case 0x0006: OP_8XY6(); break;
                case 0x0007: OP_8XY7(); break;
                case 0x000E: OP_8XYE(); break;
            }
            break;
        case 0x9000: OP_9XY0(); break;
        case 0xA000: OP_ANNN(); break;
        case 0xB000: OP_BNNN(); break;
        case 0xC000: OP_CXNN(); break;
        case 0xD000: OP_DXYN(); break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x009E: OP_EX9E(); break;
                case 0x00A1: OP_EXA1(); break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
                case 0x0007: OP_FX07(); break;
                case 0x000A: OP_FX0A(); break;
                case 0x0015: OP_FX15(); break;
                case 0x0018: OP_FX18(); break;
                case 0x001E: OP_FX1E(); break;
                case 0x0029: OP_FX29(); break;
                case 0x0033: OP_FX33(); break;
                case 0x0055: OP_FX55(); break;
                case 0x0065: OP_FX65(); break;
            }
            break;
    }
}

void Chip8::cycle() {
    readOpcode();

    if (delayTimer > 0) {
        delayTimer--;
    }

    if (soundTimer > 0) {
        soundTimer--;
    }
}

void Chip8::OP_0NNN() {
    uint16_t address = opcode & 0x0fff;

    pc = address;
}

void Chip8::OP_00E0() {
    for (size_t i = 0; i < display.size(); ++i) {
        display[i] = 0x00;  
    }
}

void Chip8::OP_00EE() {
    sp--;
    pc = stack[sp];
}

void Chip8::OP_1NNN() {
    uint16_t address = opcode & 0x0fff;

    pc = address;
}

void Chip8::OP_2NNN() {
    uint16_t address = opcode & 0x0fff;

    stack[sp++] = pc;
    pc = address;
}

void Chip8::OP_3XNN() {
    uint8_t value = opcode & 0x00ff;
    uint8_t reg = (opcode & 0x0f00) >> 8;

    if (V[reg] == value) {
        pc += 2;
    }
}

void Chip8::OP_4XNN() {
    uint8_t value = opcode & 0x00ff;
    uint8_t reg = (opcode & 0x0f00) >> 8;

    if (V[reg] != value) {
        pc += 2;
    }
}

void Chip8::OP_5XY0() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    if (V[reg1] == V[reg2]) {
        pc += 2;
    }
}

void Chip8::OP_6XNN() {
    uint8_t value = opcode & 0x00ff;
    uint8_t reg = (opcode & 0x0f00) >> 8;

    V[reg] = value;
}

void Chip8::OP_7XNN() {
    uint8_t value = opcode & 0x00ff;
    uint8_t reg = (opcode & 0x0f00) >> 8;

    V[reg] += value;
}

void Chip8::OP_8XY0() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] = V[reg2];
}

void Chip8::OP_8XY1() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] |= V[reg2];
    V[0xf] = 0;
}

void Chip8::OP_8XY2() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] &= V[reg2];
    V[0xf] = 0;
}

void Chip8::OP_8XY3() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] ^= V[reg2];
    V[0xf] = 0;
}

void Chip8::OP_8XY4() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    uint16_t total = V[reg1] + V[reg2];

    V[reg1] = total & 0xff;

    if (total > 255) {
        V[0xf] = 1;
    }
    else {
        V[0xf] = 0;
    }
}

void Chip8::OP_8XY5() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    uint16_t total = V[reg1] - V[reg2];

    uint8_t carry;
    if (V[reg1] >= V[reg2]) {
        carry = 1;
    }
    else {
        carry = 0;
    }

    V[reg1] = total & 0xff;
    V[0xf] = carry;
}

void Chip8::OP_8XY6() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] = V[reg2];
    uint8_t carry = V[reg1] & 0x01;
    V[reg1] >>= 1;
    V[0xf] = carry & 0x1;
}

void Chip8::OP_8XY7() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    uint16_t total = V[reg2] - V[reg1];

    uint8_t carry;
    if (V[reg2] >= V[reg1]) {
        carry = 1;
    }
    else {
        carry = 0;
    }

    V[reg1] = total & 0xff;
    V[0xf] = carry;
}

void Chip8::OP_8XYE() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    V[reg1] = V[reg2];
    uint8_t carry = (V[reg1] & 0x80) >> 7;
    V[reg1] <<= 1;
    V[0xf] = carry;
}

void Chip8::OP_9XY0() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;

    if (V[reg1] != V[reg2]) {
        pc += 2;
    }
}

void Chip8::OP_ANNN() {
    uint16_t address = opcode & 0x0fff;

    index = address;
}

void Chip8::OP_BNNN() {
    uint16_t address = opcode & 0x0fff;

    pc = address + V[0];
}

void Chip8::OP_CXNN() {
    uint8_t value = opcode & 0x00ff;
    uint8_t reg = (opcode & 0x0f00) >> 8;

    uint8_t random = static_cast<uint8_t>(dist(rng));
    V[reg] = random & value;
}

void Chip8::OP_DXYN() {
    uint8_t reg1 = (opcode & 0x0f00) >> 8;
    uint8_t reg2 = (opcode & 0x00f0) >> 4;
    uint8_t height = (opcode & 0x000f);

    uint8_t x = V[reg1];
    uint8_t y = V[reg2];
    const uint8_t sprite_width = 8;

    V[0xf] = 0;

    for (uint32_t i = 0; i < height; ++i) {
        uint8_t row_byte = memory[index + i];
        for (uint32_t j = 0; j < sprite_width; ++j) {
            uint8_t pixel = row_byte & (0x80 >> j);

            if (pixel) {
                size_t display_index = ((y + i) % DISPLAY_HEIGHT) * DISPLAY_WIDTH + ((x + j) % DISPLAY_WIDTH);
                
                if (display[display_index]) {
                    V[0xf] = 1;
                }
                
                display[display_index] ^= 1;
            }
        }
    }
}


void Chip8::OP_EX9E() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    uint8_t key = V[reg];

    if (keys[key]) {
        pc += 2;
    }
}

void Chip8::OP_EXA1() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    uint8_t key = V[reg];

    if (!keys[key]) {
        pc += 2;
    }
}

void Chip8::OP_FX07() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    V[reg] = delayTimer;
}

void Chip8::OP_FX0A() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    bool keyPressed = false;
    
    for (uint8_t i = 0; i < keys.size(); ++i) {
        if (keys[i]) {
            V[reg] = i;
            keyPressed = true;
            break;
        }
    }
    
    if (!keyPressed) {
        pc -= 2;
    }
}

void Chip8::OP_FX15() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    delayTimer = V[reg];
}

void Chip8::OP_FX18() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    soundTimer = V[reg];
}

void Chip8::OP_FX1E() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    index += V[reg];
}

void Chip8::OP_FX29() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    uint8_t ch = V[reg] & 0x0f;
    index = FONT_START + (ch * 5);
}

void Chip8::OP_FX33() {
    uint8_t reg = (opcode & 0x0f00) >> 8;
    uint8_t value = V[reg];

	memory[index + 2] = value % 10;
	value /= 10;

	memory[index + 1] = value % 10;
	value /= 10;

	memory[index] = value % 10;
}

void Chip8::OP_FX55() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    for (uint8_t i = 0; i <= reg; ++i) {
        memory[index + i] = V[i];
    }
    index += reg + 1;
}

void Chip8::OP_FX65() {
    uint8_t reg = (opcode & 0x0f00) >> 8;

    for (uint8_t i = 0; i <= reg; ++i) {
        V[i] = memory[index + i];
    }
    index += reg + 1;
}
