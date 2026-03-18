#include "weave.h"
#include <SDL.h>
#include <time.h>

#define PIXEL_SCALE 8
#define WINDOW_W (WARP_ENDS * PIXEL_SCALE)
#define WINDOW_H (VISIBLE_ROWS * PIXEL_SCALE)

static void render_grid(const Loom *loom, uint8_t *pixels) {
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        // Ring buffer: grid_head points to the oldest row
        int src_row = (loom->grid_head + y) % VISIBLE_ROWS;
        for (int x = 0; x < WARP_ENDS; x++) {
            uint8_t ci = loom->grid[src_row][x];
            Color c = loom->palette[ci % loom->palette_size];
            int off = (y * WARP_ENDS + x) * 3;
            pixels[off]     = c.r;
            pixels[off + 1] = c.g;
            pixels[off + 2] = c.b;
        }
    }
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window *window = SDL_CreateWindow("Weave",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
        WARP_ENDS, VISIBLE_ROWS);

    uint8_t pixels[VISIBLE_ROWS * WARP_ENDS * 3];

    Loom loom;
    loom_init(&loom, (uint32_t)time(NULL));

    uint32_t tick_interval_ms = 250; // 4 rows/sec
    uint32_t last_tick = SDL_GetTicks();

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        running = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        if (!loom.paused) {
            uint32_t now = SDL_GetTicks();
            if (now - last_tick >= tick_interval_ms) {
                advance_loom(&loom);
                last_tick = now;
            }
        }

        render_grid(&loom, pixels);
        SDL_UpdateTexture(texture, NULL, pixels, WARP_ENDS * 3);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
