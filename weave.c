#include "weave.h"
#include <stdio.h>

// --- Pattern Libraries ---
// Each threading entry is a repeating unit of shaft assignments (x-axis)
// that tiles across 64 warp ends. The numbers are shaft indices (0–3).
const ThreadingEntry THREADING_LIBRARY[THREADING_COUNT] = {
    {"Straight draw",       {0,1,2,3},                                     4},
    {"Point twill",         {0,1,2,3,2,1},                                 6},
    {"Rosepath",            {0,1,2,3,2,1,0,3},                             8},
    {"Broken twill",        {0,1,2,3,1,0,3,2},                             8},
    {"Extended point",      {0,1,2,2,3,3,2,2,1,0},                        10},
    {"Monk's belt",         {0,1,0,1,2,3,2,3},                             8},
    {"M's and O's",         {0,1,0,1,2,3,2,3},                             8},
    {"Overshot",            {0,1,0,2,1,2,1,3,0,3,0,2,1,2,1,0},           16},
    {"Huck lace",           {0,1,0,2,3,2},                                 6},
    {"Advancing twill",     {0,1,2,3,1,2,3,0},                             8},
    {"Crepe",               {0,2,1,3,2,0,3,1},                             8},
    {"Undulating twill",    {0,1,2,3,2,3,0,1,0,1,2,3},                   12},
};

// Each treadling entry is a repeating sequence of pedal presses (y-axis).
// The numbers are treadle indices (0–3), one per pick (row).
const TreadlingEntry TREADLING_LIBRARY[TREADLING_SEQ_COUNT] = {
    {"Straight",    {0,1,2,3},                 4},
    {"Reverse",     {3,2,1,0},                 4},
    {"Point",       {0,1,2,3,2,1},             6},
    {"Alternating", {0,2,1,3},                 4},
    {"Rose",        {0,1,2,3,2,1,0,3},         8},
    {"Extended",    {0,0,1,1,2,2,3,3},         8},
    {"Syncopated",  {0,1,0,2,0,3},             6},
    {"Doubled",     {0,1,2,3,0,1,2,3},         8},
    {"Offset",      {0,2,1,3,2,0,3,1},         8},
};

// Starting tie-ups: each is a 4×4 matrix [treadle][shaft].
// 1 = pressing that pedal raises that shaft frame.
const uint8_t TIEUP_LIBRARY[TIEUP_COUNT][TREADLES][SHAFTS] = {
    // 2/2 Twill: two up, two down, shifting — produces diagonal lines
    {{1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1}},
    // Tabby (plain weave): over one, under one, alternating
    {{1,0,1,0}, {0,1,0,1}, {1,0,1,0}, {0,1,0,1}},
    // Point Twill: twill that reverses direction — produces chevrons/diamonds
    {{1,1,0,0}, {1,1,1,0}, {0,1,1,1}, {0,0,1,1}},
};

// --- Color Palettes ---

const PaletteEntry PALETTE_LIBRARY[PALETTE_COUNT] = {
    {"Indigo & Natural", {{45,52,94}, {210,195,170}, {80,85,128}, {180,165,140}}, 4},
    {"Earth",            {{101,67,33}, {181,137,85}, {139,90,43}, {222,199,163}}, 4},
    {"High Contrast",    {{20,20,20}, {235,235,230}, {180,40,30}, {50,50,55}},    4},
    {"Ocean",            {{15,30,60}, {40,80,120}, {100,160,180}, {200,220,225}}, 4},
    {"Forest",           {{25,50,25}, {60,100,45}, {140,160,80}, {200,190,140}},  4},
};

// --- RNG (xorshift32) ---

uint32_t rng_next(Loom *loom) {
    uint32_t x = loom->rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    loom->rng_state = x;
    return x;
}

int rng_range(Loom *loom, int min, int max) {
    return min + (int)(rng_next(loom) % (uint32_t)(max - min + 1));
}

// --- Initialization ---

void loom_init(Loom *loom, uint32_t seed) {
    memset(loom, 0, sizeof(Loom));
    loom->rng_state = seed ? seed : 1;

    // Random starting threading from library
    int ti = rng_range(loom, 0, THREADING_COUNT - 1);
    const ThreadingEntry *te = &THREADING_LIBRARY[ti];
    for (int i = 0; i < WARP_ENDS; i++)
        loom->threading.shaft[i] = te->pattern[i % te->length];

    // Random starting tie-up from library
    int ui = rng_range(loom, 0, TIEUP_COUNT - 1);
    memcpy(loom->tieup.matrix, TIEUP_LIBRARY[ui], sizeof(loom->tieup.matrix));

    // Random starting treadling from library
    int ri = rng_range(loom, 0, TREADLING_SEQ_COUNT - 1);
    const TreadlingEntry *re = &TREADLING_LIBRARY[ri];
    memcpy(loom->treadling.sequence, re->pattern, re->length);
    loom->treadling.length = re->length;
    loom->treadling.direction = 1;

    // Random starting palette from library
    loom->current_palette_index = rng_range(loom, 0, PALETTE_COUNT - 1);
    const PaletteEntry *pal = &PALETTE_LIBRARY[loom->current_palette_index];
    memcpy(loom->palette, pal->colors, sizeof(Color) * pal->size);
    loom->palette_size = pal->size;

    // Solid warp sett (all indigo), weft is natural
    loom->warp_sett.indices[0] = 0;
    loom->warp_sett.length = 1;
    loom->weft_color_index = 1;

    loom->grid_head = 0;
    loom->pick = 0;

    // Schedule first evolution events at random intervals
    loom->next_threading_change   = rng_range(loom, 200, 600);
    loom->next_tieup_mutation     = rng_range(loom, 30, 90);
    loom->next_treadling_change   = rng_range(loom, 100, 300);
    loom->next_treadling_rotation = rng_range(loom, 50, 150);
    loom->next_weft_color_change  = rng_range(loom, 20, 80);

    loom->paused = 0;
}

// Core drawdown formula: Tieup[Treadling[y]][Threading[x]]
// For the current pick, which treadle is pressed? Then check if that
// treadle raises the shaft this warp end is on.
int drawdown_cell(const Loom *loom, int x) {
    // Which treadle (foot pedal) is active for the current pick?
    int seq_pos = (int)(loom->pick % (uint32_t)loom->treadling.length);
    if (loom->treadling.direction < 0)
        seq_pos = loom->treadling.length - 1 - seq_pos;
    int treadle = loom->treadling.sequence[seq_pos];

    // Which shaft (frame) is this warp end threaded on?
    int shaft = loom->threading.shaft[x];

    // Tie-up lookup: does this pedal raise this frame?
    return loom->tieup.matrix[treadle][shaft];
}

// Map a drawdown result to a color palette index.
// If warp is on top (drawdown=1), use the warp sett color for this column.
// If weft is on top (drawdown=0), use the current weft color.
uint8_t resolve_color(const Loom *loom, int x, int drawdown) {
    if (drawdown) {
        return loom->warp_sett.indices[x % loom->warp_sett.length];
    } else {
        return loom->weft_color_index;
    }
}

// Compute one new row of fabric and write it into the ring buffer.
void advance_loom(Loom *loom) {
    int row = loom->grid_head;
    for (int x = 0; x < WARP_ENDS; x++) {
        int dd = drawdown_cell(loom, x);
        loom->grid[row][x] = resolve_color(loom, x, dd);
    }
    loom->grid_head = (loom->grid_head + 1) % VISIBLE_ROWS;
    loom->pick++;
}

// --- Evolution ---

void check_evolutions(Loom *loom) {
    uint32_t p = loom->pick;

    // Weft color step: cycle the horizontal thread color through the palette.
    // Creates horizontal banding that interacts with the weave structure.
    if (p >= loom->next_weft_color_change) {
        // Step to next color, skipping any that match the dominant warp sett color
        // (otherwise warp and weft become identical and the pattern disappears)
        uint8_t warp_dominant = loom->warp_sett.indices[0];
        uint8_t next = loom->weft_color_index;
        for (int i = 0; i < loom->palette_size; i++) {
            next = (next + 1) % loom->palette_size;
            if (next != warp_dominant) break;
        }
        loom->weft_color_index = next;
        fprintf(stderr, "[pick %u] weft color → %d\n", p, next);
        loom->next_weft_color_change = p + rng_range(loom, 20, 80);
    }
}
