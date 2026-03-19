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
    // 1/3 Twill: very open, shows lots of warp
    {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}},
    // 3/1 Twill: weft-dominant, heavy coverage
    {{1,1,1,0}, {0,1,1,1}, {1,0,1,1}, {1,1,0,1}},
    // Summer/Winter: block patterns
    {{1,1,0,0}, {1,0,1,0}, {1,0,0,1}, {0,1,0,1}},
};

const char *TIEUP_NAMES[TIEUP_COUNT] = {
    "2/2 Twill", "Tabby", "Point Twill",
    "1/3 Twill", "3/1 Twill", "Summer/Winter",
};

// --- Color Palettes ---

const PaletteEntry PALETTE_LIBRARY[PALETTE_COUNT] = {
    {"Indigo & Natural", {{45,52,94}, {210,195,170}, {80,85,128}, {180,165,140}}, 4},
    {"Earth",            {{101,67,33}, {181,137,85}, {139,90,43}, {222,199,163}}, 4},
    {"High Contrast",    {{20,20,20}, {235,235,230}, {180,40,30}, {50,50,55}},    4},
    {"Ocean",            {{15,30,60}, {40,80,120}, {100,160,180}, {200,220,225}}, 4},
    {"Forest",           {{25,50,25}, {60,100,45}, {140,160,80}, {200,190,140}},  4},
    {"Berry & Sage",     {{140,30,80}, {85,45,90}, {120,150,95}, {230,220,195}}, 4},
    {"Sunset",           {{210,95,75}, {200,155,50}, {75,40,100}, {230,200,130}}, 4},
    {"Slate & Rust",     {{100,105,110}, {70,75,80}, {185,95,45}, {160,80,35}},  4},
    {"Nordic",           {{20,30,70}, {190,35,40}, {235,235,235}, {60,90,150}},  4},
    {"Jewel",            {{15,110,60}, {170,25,40}, {30,55,140}, {200,165,40}},  4},
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
    loom->threading_name = te->name;

    // Random starting tie-up from library
    int ui = rng_range(loom, 0, TIEUP_COUNT - 1);
    memcpy(loom->tieup.matrix, TIEUP_LIBRARY[ui], sizeof(loom->tieup.matrix));
    loom->tieup_name = TIEUP_NAMES[ui];

    // Random starting treadling from library
    int ri = rng_range(loom, 0, TREADLING_SEQ_COUNT - 1);
    const TreadlingEntry *re = &TREADLING_LIBRARY[ri];
    memcpy(loom->treadling.sequence, re->pattern, re->length);
    loom->treadling.length = re->length;
    loom->treadling.direction = 1;
    loom->treadling_name = re->name;

    // Random starting palette from library
    loom->current_palette_index = rng_range(loom, 0, PALETTE_COUNT - 1);
    const PaletteEntry *pal = &PALETTE_LIBRARY[loom->current_palette_index];
    memcpy(loom->palette, pal->colors, sizeof(Color) * pal->size);
    loom->palette_size = pal->size;

    // Random non-solid warp sett (1-6, skipping solid)
    int sett_type = rng_range(loom, 1, 6);
    switch (sett_type) {
        case 1: { // Alternating
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a;
            loom->warp_sett.indices[1] = b;
            loom->warp_sett.length = 2;
            break;
        }
        case 2: { // Tartan
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a; loom->warp_sett.indices[1] = a;
            loom->warp_sett.indices[2] = b; loom->warp_sett.indices[3] = b;
            loom->warp_sett.indices[4] = a; loom->warp_sett.indices[5] = a;
            loom->warp_sett.indices[6] = c; loom->warp_sett.indices[7] = c;
            loom->warp_sett.length = 8;
            break;
        }
        case 3: { // Gradient
            uint8_t order[MAX_SETT];
            for (int i = 0; i < loom->palette_size; i++)
                order[i] = i;
            for (int i = loom->palette_size - 1; i > 0; i--) {
                int j = rng_range(loom, 0, i);
                uint8_t tmp = order[i]; order[i] = order[j]; order[j] = tmp;
            }
            for (int i = 0; i < loom->palette_size && i < MAX_SETT; i++)
                loom->warp_sett.indices[i] = order[i];
            loom->warp_sett.length = loom->palette_size;
            break;
        }
        case 4: { // Herringbone
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a; loom->warp_sett.indices[1] = a;
            loom->warp_sett.indices[2] = b; loom->warp_sett.indices[3] = b;
            loom->warp_sett.indices[4] = c; loom->warp_sett.indices[5] = c;
            loom->warp_sett.indices[6] = b; loom->warp_sett.indices[7] = b;
            loom->warp_sett.length = 8;
            break;
        }
        case 5: { // Windowpane
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            int width = rng_range(loom, 4, 6);
            int len = 0;
            for (int i = 0; i < width && len < MAX_SETT - 1; i++)
                loom->warp_sett.indices[len++] = a;
            loom->warp_sett.indices[len++] = b;
            loom->warp_sett.length = len;
            break;
        }
        case 6: { // District check
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            int wa = rng_range(loom, 2, 4);
            int wb = rng_range(loom, 2, 4);
            int wc = rng_range(loom, 1, 3);
            int len = 0;
            for (int i = 0; i < wa && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = a;
            for (int i = 0; i < wb && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = b;
            for (int i = 0; i < wc && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = c;
            loom->warp_sett.length = len;
            break;
        }
    }
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

// Validate a tie-up: no all-zero rows (pedal raises nothing), no all-one rows
// (no interlacement), no duplicate rows (wasted pedals).
bool validate_tieup(const Tieup *tieup) {
    for (int t = 0; t < TREADLES; t++) {
        int sum = 0;
        for (int s = 0; s < SHAFTS; s++)
            sum += tieup->matrix[t][s];
        if (sum == 0 || sum == SHAFTS) return false;
    }
    for (int a = 0; a < TREADLES; a++)
        for (int b = a + 1; b < TREADLES; b++)
            if (memcmp(tieup->matrix[a], tieup->matrix[b], SHAFTS) == 0)
                return false;
    return true;
}

// Check that no warp or weft float exceeds max_float.
// Simulates the drawdown over one full treadling cycle to detect long runs.
bool validate_floats(const Loom *loom, int max_float) {
    // Warp floats: vertical runs in each column across the treadling cycle
    for (int x = 0; x < WARP_ENDS; x++) {
        int run = 1;
        int prev = -1;
        for (int dy = 0; dy < loom->treadling.length; dy++) {
            int seq_pos = dy % loom->treadling.length;
            if (loom->treadling.direction < 0)
                seq_pos = loom->treadling.length - 1 - seq_pos;
            int treadle = loom->treadling.sequence[seq_pos];
            int val = loom->tieup.matrix[treadle][loom->threading.shaft[x]];
            if (val == prev) {
                if (++run > max_float) return false;
            } else {
                run = 1;
            }
            prev = val;
        }
    }
    // Weft floats: horizontal runs in each row of the treadling cycle
    for (int dy = 0; dy < loom->treadling.length; dy++) {
        int run = 1;
        int prev = -1;
        int seq_pos = dy % loom->treadling.length;
        if (loom->treadling.direction < 0)
            seq_pos = loom->treadling.length - 1 - seq_pos;
        int treadle = loom->treadling.sequence[seq_pos];
        for (int x = 0; x < WARP_ENDS; x++) {
            int val = loom->tieup.matrix[treadle][loom->threading.shaft[x]];
            if (val == prev) {
                if (++run > max_float) return false;
            } else {
                run = 1;
            }
            prev = val;
        }
    }
    return true;
}

// Flip one random bit in the tie-up. Reject if the result is invalid
// (bad row sums, duplicates) or would create floats longer than MAX_FLOAT.
void mutate_tieup(Loom *loom) {
    Tieup original = loom->tieup;
    for (int attempt = 0; attempt < 16; attempt++) {
        Tieup trial = original;
        int t = rng_range(loom, 0, TREADLES - 1);
        int s = rng_range(loom, 0, SHAFTS - 1);
        trial.matrix[t][s] ^= 1;
        if (validate_tieup(&trial)) {
            loom->tieup = trial;
            if (validate_floats(loom, MAX_FLOAT)) {
                fprintf(stderr, "[pick %u] tie-up flip [%d][%d]\n", loom->pick, t, s);
                return;
            }
            loom->tieup = original;
        }
    }
}

// Pick a new threading from the library (x-axis: which shaft per warp end).
// Also randomize the warp color sett — on a real loom these change together
// since warp colors are fixed when you thread up.
void randomize_threading(Loom *loom) {
    int ti = rng_range(loom, 0, THREADING_COUNT - 1);
    const ThreadingEntry *e = &THREADING_LIBRARY[ti];
    for (int i = 0; i < WARP_ENDS; i++)
        loom->threading.shaft[i] = e->pattern[i % e->length];
    loom->threading_name = e->name;
    fprintf(stderr, "[pick %u] threading → %s\n", loom->pick, e->name);

    // Randomize warp color sett
    int sett_type = rng_range(loom, 0, 6);
    switch (sett_type) {
        case 0: // Solid — all one color
            loom->warp_sett.indices[0] = rng_range(loom, 0, loom->palette_size - 1);
            loom->warp_sett.length = 1;
            break;
        case 1: { // Alternating — two random colors
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a;
            loom->warp_sett.indices[1] = b;
            loom->warp_sett.length = 2;
            break;
        }
        case 2: { // Tartan-like — bands of random colors
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a; loom->warp_sett.indices[1] = a;
            loom->warp_sett.indices[2] = b; loom->warp_sett.indices[3] = b;
            loom->warp_sett.indices[4] = a; loom->warp_sett.indices[5] = a;
            loom->warp_sett.indices[6] = c; loom->warp_sett.indices[7] = c;
            loom->warp_sett.length = 8;
            break;
        }
        case 3: { // Gradient — shuffled palette
            uint8_t order[MAX_SETT];
            for (int i = 0; i < loom->palette_size; i++)
                order[i] = i;
            for (int i = loom->palette_size - 1; i > 0; i--) {
                int j = rng_range(loom, 0, i);
                uint8_t tmp = order[i]; order[i] = order[j]; order[j] = tmp;
            }
            for (int i = 0; i < loom->palette_size && i < MAX_SETT; i++)
                loom->warp_sett.indices[i] = order[i];
            loom->warp_sett.length = loom->palette_size;
            break;
        }
        case 4: { // Herringbone — A A B B C C B B (mirrors)
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            loom->warp_sett.indices[0] = a; loom->warp_sett.indices[1] = a;
            loom->warp_sett.indices[2] = b; loom->warp_sett.indices[3] = b;
            loom->warp_sett.indices[4] = c; loom->warp_sett.indices[5] = c;
            loom->warp_sett.indices[6] = b; loom->warp_sett.indices[7] = b;
            loom->warp_sett.length = 8;
            break;
        }
        case 5: { // Windowpane — long run of one color with thin accent stripe
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            int width = rng_range(loom, 4, 6);
            int len = 0;
            for (int i = 0; i < width && len < MAX_SETT - 1; i++)
                loom->warp_sett.indices[len++] = a;
            loom->warp_sett.indices[len++] = b;
            loom->warp_sett.length = len;
            break;
        }
        case 6: { // District check — 3 colors in unequal bands
            uint8_t a = rng_range(loom, 0, loom->palette_size - 1);
            uint8_t b = (a + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            uint8_t c = (b + rng_range(loom, 1, loom->palette_size - 1)) % loom->palette_size;
            int wa = rng_range(loom, 2, 4);
            int wb = rng_range(loom, 2, 4);
            int wc = rng_range(loom, 1, 3);
            int len = 0;
            for (int i = 0; i < wa && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = a;
            for (int i = 0; i < wb && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = b;
            for (int i = 0; i < wc && len < MAX_SETT; i++)
                loom->warp_sett.indices[len++] = c;
            loom->warp_sett.length = len;
            break;
        }
    }
}

// Evolve the treadling sequence (y-axis: which pedal per row).
// Randomly picks one of three actions:
//   0: replace from library
//   1: reverse direction (creates mirror/chevron effects)
//   2: tromp-as-writ (copy threading pattern as treadling for symmetry)
void evolve_treadling(Loom *loom) {
    int action = rng_range(loom, 0, 2);
    switch (action) {
        case 0: {
            int ri = rng_range(loom, 0, TREADLING_SEQ_COUNT - 1);
            const TreadlingEntry *re = &TREADLING_LIBRARY[ri];
            memcpy(loom->treadling.sequence, re->pattern, re->length);
            loom->treadling.length = re->length;
            loom->treadling_name = re->name;
            fprintf(stderr, "[pick %u] treadling → %s\n", loom->pick, re->name);
            break;
        }
        case 1:
            loom->treadling.direction *= -1;
            fprintf(stderr, "[pick %u] treadling reversed\n", loom->pick);
            break;
        case 2: {
            // Tromp-as-writ: copy one repeat of the threading into the treadling
            int len = 0;
            for (int i = 0; i < MAX_SEQUENCE && i < WARP_ENDS; i++) {
                loom->treadling.sequence[i] = loom->threading.shaft[i];
                len++;
                if (i > 0 && loom->threading.shaft[i] == loom->threading.shaft[0]) {
                    bool repeats = true;
                    for (int j = 0; j < len && i + j < WARP_ENDS; j++) {
                        if (loom->threading.shaft[j] != loom->threading.shaft[i + j]) {
                            repeats = false;
                            break;
                        }
                    }
                    if (repeats) break;
                }
            }
            loom->treadling.length = len;
            loom->treadling_name = "tromp-as-writ";
            fprintf(stderr, "[pick %u] treadling → tromp-as-writ (len %d)\n", loom->pick, len);
            break;
        }
    }
}

// Rotate the treadling start by one position: [0,1,2,3] → [1,2,3,0]
// Creates a subtle diagonal shift in the pattern.
void rotate_treadling(Loom *loom) {
    uint8_t first = loom->treadling.sequence[0];
    for (int i = 0; i < loom->treadling.length - 1; i++)
        loom->treadling.sequence[i] = loom->treadling.sequence[i + 1];
    loom->treadling.sequence[loom->treadling.length - 1] = first;
}

void check_evolutions(Loom *loom) {
    uint32_t p = loom->pick;

    // Threading change: swap all 64 warp end shaft assignments + warp sett.
    // The rarest and most dramatic event.
    if (p >= loom->next_threading_change) {
        randomize_threading(loom);
        loom->next_threading_change = p + rng_range(loom, 200, 600);
    }

    // Tie-up mutation: flip one bit in the 4×4 pedal-to-shaft matrix.
    // Gradual texture drift — twill morphs into satin, then something new.
    if (p >= loom->next_tieup_mutation) {
        mutate_tieup(loom);
        loom->next_tieup_mutation = p + rng_range(loom, 30, 90);
    }

    // Treadling replacement: swap the entire pedal sequence.
    if (p >= loom->next_treadling_change) {
        evolve_treadling(loom);
        loom->next_treadling_change = p + rng_range(loom, 100, 300);
    }

    // Treadling rotation: shift start by one for diagonal drift.
    if (p >= loom->next_treadling_rotation) {
        rotate_treadling(loom);
        loom->next_treadling_rotation = p + rng_range(loom, 50, 150);
    }

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
        loom->next_weft_color_change = p + rng_range(loom, 20, 80);
    }
}
