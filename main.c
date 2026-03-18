#include "weave.h"
#include <SDL.h>
#include <stdio.h>
#include <time.h>

#define PIXEL_SCALE 8
#define DEBUG_MARGIN 4
#define TEX_W (WARP_ENDS + DEBUG_MARGIN)    // 68
#define TEX_H (VISIBLE_ROWS + DEBUG_MARGIN) // 68
#define WINDOW_W (TEX_W * PIXEL_SCALE)      // 544
#define WINDOW_H (TEX_H * PIXEL_SCALE)      // 544

// Render the fabric grid into the pixel buffer.
// tex_w = row stride in pixels, x_offset/y_offset = cell offset for debug margin.
static void render_grid(const Loom *loom, uint8_t *pixels, int tex_w,
                        int x_offset, int y_offset) {
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        int src_row = (loom->grid_head + y) % VISIBLE_ROWS;
        for (int x = 0; x < WARP_ENDS; x++) {
            uint8_t ci = loom->grid[src_row][x];
            Color c = loom->palette[ci % loom->palette_size];
            int off = ((y + y_offset) * tex_w + (x + x_offset)) * 3;
            pixels[off]     = c.r;
            pixels[off + 1] = c.g;
            pixels[off + 2] = c.b;
        }
    }
}

// Render the traditional draft notation in the margins:
//   Layout:  [Tie-up 4×4 ] [Threading 64×4     ]
//            [Treadling   ] [Drawdown 64×64     ]
//            [4×64        ] [                    ]
//
// Treadling on the left, tie-up in the top-left corner.
static void render_debug(const Loom *loom, uint8_t *pixels, int tex_w) {
    Color on  = {255, 255, 255};
    Color off = {40, 40, 40};

    // Threading: top 4 rows, shifted right by DEBUG_MARGIN
    for (int shaft_row = 0; shaft_row < SHAFTS; shaft_row++) {
        for (int x = 0; x < WARP_ENDS; x++) {
            Color c = (loom->threading.shaft[x] == (SHAFTS - 1 - shaft_row)) ? on : off;
            int px = DEBUG_MARGIN + x;
            int py = shaft_row;
            int idx = (py * tex_w + px) * 3;
            pixels[idx] = c.r; pixels[idx+1] = c.g; pixels[idx+2] = c.b;
        }
    }

    // Tie-up: top-left corner, 4×4
    for (int t = 0; t < TREADLES; t++) {
        for (int s = 0; s < SHAFTS; s++) {
            Color c = loom->tieup.matrix[t][SHAFTS - 1 - s] ? on : off;
            int px = t;
            int py = s;
            int idx = (py * tex_w + px) * 3;
            pixels[idx] = c.r; pixels[idx+1] = c.g; pixels[idx+2] = c.b;
        }
    }

    // Treadling: left side, 4 columns × 64 rows
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        uint32_t row_pick = loom->pick - VISIBLE_ROWS + (uint32_t)y;
        int seq_pos = (int)(row_pick % (uint32_t)loom->treadling.length);
        if (loom->treadling.direction < 0)
            seq_pos = loom->treadling.length - 1 - seq_pos;
        int active_treadle = loom->treadling.sequence[seq_pos];
        for (int t = 0; t < TREADLES; t++) {
            Color c = (t == active_treadle) ? on : off;
            int px = t;
            int py = SHAFTS + y;
            int idx = (py * tex_w + px) * 3;
            pixels[idx] = c.r; pixels[idx+1] = c.g; pixels[idx+2] = c.b;
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
        TEX_W, TEX_H);

    uint8_t pixels[TEX_H * TEX_W * 3];
    memset(pixels, 0, sizeof(pixels));

    Loom loom;
    loom_init(&loom, (uint32_t)time(NULL));

    fprintf(stderr, "Weave — Generative Loom\n");
    fprintf(stderr, "  Space     pause/resume\n");
    fprintf(stderr, "  Up/Down   speed up/slow down\n");
    fprintf(stderr, "  T         force threading change\n");
    fprintf(stderr, "  U         force tie-up mutation\n");
    fprintf(stderr, "  R         force treadling change\n");
    fprintf(stderr, "  C         cycle palette\n");
    fprintf(stderr, "  D         toggle debug overlay\n");
    fprintf(stderr, "  Esc/Q     quit\n");
    fprintf(stderr, "palette: %s\n", PALETTE_LIBRARY[loom.current_palette_index].name);

    uint32_t tick_interval_ms = 250; // 4 rows/sec
    uint32_t last_tick = SDL_GetTicks();
    int show_debug = 0;

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        loom.paused = !loom.paused;
                        break;
                    case SDLK_UP:
                        if (tick_interval_ms > 20) tick_interval_ms -= 20;
                        fprintf(stderr, "speed: %ums/row\n", tick_interval_ms);
                        break;
                    case SDLK_DOWN:
                        tick_interval_ms += 20;
                        fprintf(stderr, "speed: %ums/row\n", tick_interval_ms);
                        break;
                    case SDLK_c: {
                        loom.current_palette_index =
                            (loom.current_palette_index + 1) % PALETTE_COUNT;
                        const PaletteEntry *pal =
                            &PALETTE_LIBRARY[loom.current_palette_index];
                        memcpy(loom.palette, pal->colors,
                               sizeof(Color) * pal->size);
                        loom.palette_size = pal->size;
                        fprintf(stderr, "palette: %s\n", pal->name);
                        break;
                    }
                    case SDLK_t:
                        randomize_threading(&loom);
                        break;
                    case SDLK_u:
                        mutate_tieup(&loom);
                        break;
                    case SDLK_r:
                        evolve_treadling(&loom);
                        break;
                    case SDLK_d:
                        show_debug = !show_debug;
                        break;
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
                check_evolutions(&loom);
                last_tick = now;
            }
        }

        // Render into the full 68×68 buffer, drawdown offset by margin
        render_grid(&loom, pixels, TEX_W, DEBUG_MARGIN, DEBUG_MARGIN);
        if (show_debug)
            render_debug(&loom, pixels, TEX_W);

        // Update window title with current state
        {
            char title[128];
            snprintf(title, sizeof(title), "Weave — pick %u | %s",
                     loom.pick, loom.paused ? "PAUSED" : "running");
            SDL_SetWindowTitle(window, title);
        }

        SDL_UpdateTexture(texture, NULL, pixels, TEX_W * 3);

        // When debug is off, show only the 64×64 drawdown area (skip margins)
        if (show_debug) {
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        } else {
            SDL_Rect src = {DEBUG_MARGIN, DEBUG_MARGIN, WARP_ENDS, VISIBLE_ROWS};
            SDL_RenderCopy(renderer, texture, &src, NULL);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
