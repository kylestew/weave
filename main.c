#include "weave.h"
#include <SDL.h>
#include <stdio.h>
#include <time.h>

#define PIXEL_SCALE 8
#define DEBUG_MARGIN 4
#define GUTTER_H 20
#define TEX_W  (WARP_ENDS + DEBUG_MARGIN)                    // 68
#define TEX_H  (VISIBLE_ROWS + DEBUG_MARGIN + GUTTER_H)      // 88 (full buffer with gutter)
#define WINDOW_W (TEX_W * PIXEL_SCALE)                        // 544
#define WINDOW_H (TEX_W * PIXEL_SCALE)                        // 544 (square, matches debug width)

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

// 3×5 bitmap font — each glyph is 5 rows, 3 bits per row (bit2=left, bit0=right).
// Indexed by (char - 32) for printable ASCII range.
static const uint8_t FONT_3X5[96][5] = {
    [' ' - 32] = {0,0,0,0,0},
    ['!' - 32] = {2,2,2,0,2},
    ['\'' - 32] = {2,2,0,0,0},
    ['(' - 32] = {1,2,2,2,1},
    [')' - 32] = {4,2,2,2,4},
    ['+' - 32] = {0,2,7,2,0},
    ['-' - 32] = {0,0,7,0,0},
    ['.' - 32] = {0,0,0,0,2},
    ['/' - 32] = {1,1,2,4,4},
    ['0' - 32] = {7,5,5,5,7},
    ['1' - 32] = {6,2,2,2,7},
    ['2' - 32] = {7,1,7,4,7},
    ['3' - 32] = {7,1,7,1,7},
    ['4' - 32] = {5,5,7,1,1},
    ['5' - 32] = {7,4,7,1,7},
    ['6' - 32] = {7,4,7,5,7},
    ['7' - 32] = {7,1,1,1,1},
    ['8' - 32] = {7,5,7,5,7},
    ['9' - 32] = {7,5,7,1,7},
    [':' - 32] = {0,2,0,2,0},
    ['A' - 32] = {2,5,7,5,5},
    ['B' - 32] = {6,5,6,5,6},
    ['C' - 32] = {3,4,4,4,3},
    ['D' - 32] = {6,5,5,5,6},
    ['E' - 32] = {7,4,6,4,7},
    ['F' - 32] = {7,4,6,4,4},
    ['G' - 32] = {3,4,5,5,3},
    ['H' - 32] = {5,5,7,5,5},
    ['I' - 32] = {7,2,2,2,7},
    ['J' - 32] = {1,1,1,5,2},
    ['K' - 32] = {5,5,6,5,5},
    ['L' - 32] = {4,4,4,4,7},
    ['M' - 32] = {5,7,5,5,5},
    ['N' - 32] = {5,7,7,5,5},
    ['O' - 32] = {2,5,5,5,2},
    ['P' - 32] = {6,5,6,4,4},
    ['Q' - 32] = {2,5,5,7,3},
    ['R' - 32] = {6,5,6,5,5},
    ['S' - 32] = {3,4,2,1,6},
    ['T' - 32] = {7,2,2,2,2},
    ['U' - 32] = {5,5,5,5,7},
    ['V' - 32] = {5,5,5,5,2},
    ['W' - 32] = {5,5,5,7,5},
    ['X' - 32] = {5,5,2,5,5},
    ['Y' - 32] = {5,5,2,2,2},
    ['Z' - 32] = {7,1,2,4,7},
};

// Render a null-terminated string into the pixel buffer at (x, y).
// Characters are 3px wide with 1px gap = 4px per character advance.
// Lowercase is mapped to uppercase.
static void render_text(uint8_t *pixels, int tex_w, int x, int y,
                        const char *text, Color color) {
    int cx = x;
    for (const char *p = text; *p; p++) {
        char ch = *p;
        if (ch >= 'a' && ch <= 'z') ch -= 32;
        int gi = ch - 32;
        if (gi < 0 || gi >= 96) { cx += 4; continue; }
        const uint8_t *glyph = FONT_3X5[gi];
        for (int row = 0; row < 5; row++) {
            uint8_t bits = glyph[row];
            for (int col = 0; col < 3; col++) {
                if (bits & (4 >> col)) {
                    int px = cx + col;
                    int py = y + row;
                    if (px >= 0 && px < tex_w && py >= 0 && py < TEX_H) {
                        int idx = (py * tex_w + px) * 3;
                        pixels[idx] = color.r;
                        pixels[idx+1] = color.g;
                        pixels[idx+2] = color.b;
                    }
                }
            }
        }
        cx += 4;
    }
}

// Render the bottom gutter with pattern names for each loom component.
static void render_gutter(const Loom *loom, uint8_t *pixels, int tex_w) {
    int gutter_y = DEBUG_MARGIN + VISIBLE_ROWS;  // row 68

    // Clear gutter area to dark background
    for (int y = gutter_y; y < gutter_y + GUTTER_H; y++) {
        for (int x = 0; x < tex_w; x++) {
            int idx = (y * tex_w + x) * 3;
            pixels[idx] = 20; pixels[idx+1] = 20; pixels[idx+2] = 20;
        }
    }

    // Separator line at top of gutter
    for (int x = 0; x < tex_w; x++) {
        int idx = (gutter_y * tex_w + x) * 3;
        pixels[idx] = 50; pixels[idx+1] = 50; pixels[idx+2] = 50;
    }

    Color text_color = {160, 160, 160};
    int line_y = gutter_y + 2;  // 2px top padding

    // Line 1: threading name
    render_text(pixels, tex_w, 1, line_y, loom->threading_name, text_color);

    // Line 2: treadling name (+ direction indicator)
    render_text(pixels, tex_w, 1, line_y + 6, loom->treadling_name, text_color);
    if (loom->treadling.direction < 0) {
        int name_end = 1 + (int)strlen(loom->treadling_name) * 4;
        render_text(pixels, tex_w, name_end, line_y + 6, "REV", text_color);
    }

    // Line 3: tie-up name
    render_text(pixels, tex_w, 1, line_y + 12, loom->tieup_name, text_color);
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
                        if (show_debug)
                            SDL_SetWindowSize(window, TEX_W * PIXEL_SCALE, TEX_H * PIXEL_SCALE);
                        else
                            SDL_SetWindowSize(window, WINDOW_W, WINDOW_H);
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
        if (show_debug) {
            render_debug(&loom, pixels, TEX_W);
            render_gutter(&loom, pixels, TEX_W);
        }

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
