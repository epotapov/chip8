#include <SDL2/SDL.h>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "chip8.h"

using Display = std::array<bool, DISPLAY_WIDTH * DISPLAY_HEIGHT>;

static void render(SDL_Renderer* renderer, const Display& display) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            if (display[y * DISPLAY_WIDTH + x]) {
                SDL_Rect r{ x * SCALE, y * SCALE, SCALE, SCALE };
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

static void clearDisplay(Display& display) {
    display.fill(false);
}

static void drawDemoPattern(Display& display) {
    clearDisplay(display);

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            if ((x + y) % 2 == 0) {
                display[y * DISPLAY_WIDTH + x] = true;
            }
            else {
                display[y * DISPLAY_WIDTH + x] = false;
            }
        }
    }
}

static bool processInput(SDL_Event* event, Chip8* chip8) {
    switch (event->type) {
        case SDL_KEYDOWN: {
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                {
                    return false;
                } break;
                case SDLK_x:
                {
                    chip8->keys[0] = 1;
                } break;

                case SDLK_1:
                {
                    chip8->keys[1] = 1;
                } break;

                case SDLK_2:
                {
                    chip8->keys[2] = 1;
                } break;

                case SDLK_3:
                {
                    chip8->keys[3] = 1;
                } break;

                case SDLK_q:
                {
                    chip8->keys[4] = 1;
                } break;

                case SDLK_w:
                {
                    chip8->keys[5] = 1;
                } break;

                case SDLK_e:
                {
                    chip8->keys[6] = 1;
                } break;

                case SDLK_a:
                {
                    chip8->keys[7] = 1;
                } break;

                case SDLK_s:
                {
                    chip8->keys[8] = 1;
                } break;

                case SDLK_d:
                {
                    chip8->keys[9] = 1;
                } break;

                case SDLK_z:
                {
                    chip8->keys[0xA] = 1;
                } break;

                case SDLK_c:
                {
                    chip8->keys[0xB] = 1;
                } break;

                case SDLK_4:
                {
                    chip8->keys[0xC] = 1;
                } break;

                case SDLK_r:
                {
                    chip8->keys[0xD] = 1;
                } break;

                case SDLK_f:
                {
                    chip8->keys[0xE] = 1;
                } break;

                case SDLK_v:
                {
                    chip8->keys[0xF] = 1;
                } break;
            }
        } break;
        case SDL_KEYUP: {
            switch (event->key.keysym.sym) {
                case SDLK_x:
                {
                    chip8->keys[0] = 0;
                } break;

                case SDLK_1:
                {
                    chip8->keys[1] = 0;
                } break;

                case SDLK_2:
                {
                    chip8->keys[2] = 0;
                } break;

                case SDLK_3:
                {
                    chip8->keys[3] = 0;
                } break;

                case SDLK_q:
                {
                    chip8->keys[4] = 0;
                } break;

                case SDLK_w:
                {
                    chip8->keys[5] = 0;
                } break;

                case SDLK_e:
                {
                    chip8->keys[6] = 0;
                } break;

                case SDLK_a:
                {
                    chip8->keys[7] = 0;
                } break;

                case SDLK_s:
                {
                    chip8->keys[8] = 0;
                } break;

                case SDLK_d:
                {
                    chip8->keys[9] = 0;
                } break;

                case SDLK_z:
                {
                    chip8->keys[0xA] = 0;
                } break;

                case SDLK_c:
                {
                    chip8->keys[0xB] = 0;
                } break;

                case SDLK_4:
                {
                    chip8->keys[0xC] = 0;
                } break;

                case SDLK_r:
                {
                    chip8->keys[0xD] = 0;
                } break;

                case SDLK_f:
                {
                    chip8->keys[0xE] = 0;
                } break;

                case SDLK_v:
                {
                    chip8->keys[0xF] = 0;
                } break;
            }
        } break;
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    char const* rom = argv[1];
    
    std::cout << "Loading ROM: " << rom << "\n";

    Chip8 chip8(rom);
    
    std::cout << "ROM loaded. PC: 0x" << std::hex << (int)chip8.pc << std::dec << "\n";

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    // Crisp scaling (nearest-neighbor)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window* window = SDL_CreateWindow(
        "CHIP8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DISPLAY_WIDTH * SCALE, DISPLAY_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else {
                running = processInput(&event, &chip8);
            }
        }

        for (int i = 0; i < 3; ++i) {
            chip8.cycle();
        }
        render(renderer, chip8.display);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}