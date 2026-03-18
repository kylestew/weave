# Weave — Implementation Plan

## File Structure

```
weave/
├── Makefile
├── weave.h      # Constants, types, data tables, function declarations
├── weave.c      # Core loom logic (drawdown, evolution, validation, color)
├── main.c       # SDL2 setup, main loop, rendering, keyboard input
├── SPEC.md
└── PLAN.md
```

Three source files following PXL's minimal approach. Everything compiles to a single `./weave` binary.

---

## Step 1: Scaffolding — Makefile + empty main

Get a window on screen with a black 64x64 grid. Proves SDL2 is linked and rendering works.

**Makefile** (modeled on PXL's):

```makefile
CC = cc
CFLAGS = -std=c99 -Wall -Wextra -O2
LDFLAGS = -lm

SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LDFLAGS := $(shell sdl2-config --libs 2>/dev/null)

weave: weave.c main.c weave.h
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -o $@ weave.c main.c $(LDFLAGS) $(SDL2_LDFLAGS)

clean:
	rm -f weave

.PHONY: clean
```

**main.c** skeleton:

```c
#include "weave.h"
#include <SDL2/SDL.h>

#define PIXEL_SCALE 8
#define WINDOW_W (WARP_ENDS * PIXEL_SCALE)   // 512
#define WINDOW_H (VISIBLE_ROWS * PIXEL_SCALE) // 512

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
    memset(pixels, 0, sizeof(pixels));

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }
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
```

**weave.h** (just constants for now):

```c
#ifndef WEAVE_H
#define WEAVE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SHAFTS       4
#define TREADLES     4
#define WARP_ENDS    64
#define VISIBLE_ROWS 64

#endif
```

**weave.c** (empty stub):

```c
#include "weave.h"
```

**Checkpoint:** `make && ./weave` opens a 512x512 black window. Esc/close quits.

---

## Step 2: Data Structures + Loom Initialization

Define all types from the spec and write `loom_init()` to set up a starting state with hardcoded values.

**weave.h** additions:

```c
#define MAX_SEQUENCE 32
#define MAX_SETT     16
#define MAX_PALETTE  8
#define MAX_FLOAT    7

// Threading: which shaft (frame) each vertical warp thread is tied to.
// This is the x-axis lookup — displayed across the top in draft notation.
// On a real loom, changing this means physically re-threading every heddle.
typedef struct {
    uint8_t shaft[WARP_ENDS];               // shaft assignment per warp end (0–3)
} Threading;

// Tie-up: the 4×4 matrix connecting treadles (foot pedals) to shafts (frames).
// Each row is a treadle; a 1 means pressing that pedal raises that shaft.
// Displayed in the top-right corner of draft notation.
typedef struct {
    uint8_t matrix[TREADLES][SHAFTS];       // [pedal][frame] → raise?
} Tieup;

// Treadling: the repeating sequence of which treadle (foot pedal) to press
// for each pick (row). This is the y-axis lookup — displayed down the right
// side in draft notation.
typedef struct {
    uint8_t sequence[MAX_SEQUENCE];         // treadle indices per pick
    int length;                             // sequence period
    int direction;                          // +1 or -1 (for reversal)
} Treadling;

// Color sett: the repeating color-stripe pattern across the warp threads.
// "Sett" from "setting the warp" — the pre-planned thread color order.
// On a real loom this is fixed at setup time (you'd have to re-warp to change it).
typedef struct {
    uint8_t indices[MAX_SETT];              // palette indices per warp position
    int length;                             // repeat length
} ColorSett;

typedef struct {
    uint8_t r, g, b;
} Color;

// Loom: the complete state of a virtual 4-shaft floor loom.
// A real floor loom has a frame you sit at with foot pedals below,
// vertical warp threads strung front-to-back, and a shuttle carrying
// the weft thread side-to-side. This struct captures all three draft
// components (threading, tie-up, treadling) plus color and display state.
typedef struct {
    Threading threading;                    // x-axis: shaft per warp end
    Tieup tieup;                            // connector: pedals → shafts
    Treadling treadling;                    // y-axis: pedal per pick (row)

    ColorSett warp_sett;                    // color stripe pattern across warp
    uint8_t weft_color_index;               // current weft (horizontal thread) color
    Color palette[MAX_PALETTE];
    int palette_size;

    uint32_t pick;                          // current pick number (pick = one weft pass = one row)
    uint32_t next_threading_change;         // when to re-thread (rare, dramatic)
    uint32_t next_tieup_mutation;           // when to flip one tie-up bit (frequent, subtle)
    uint32_t next_treadling_change;         // when to swap treadling sequence (moderate)
    uint32_t next_treadling_rotation;       // when to rotate treadling start (frequent, subtle)
    uint32_t next_weft_color_change;        // when to step weft color (frequent)

    uint8_t grid[VISIBLE_ROWS][WARP_ENDS];  // ring buffer of palette indices (the fabric)
    int grid_head;                          // ring buffer write position

    int paused;
} Loom;

void loom_init(Loom *loom, uint32_t seed);
```

**weave.c** — `loom_init` sets up a basic straight-draw threading, 2/2 twill tie-up, straight treadling, a 2-color palette (indigo + natural), and schedules the first evolution events:

```c
void loom_init(Loom *loom, uint32_t seed) {
    memset(loom, 0, sizeof(Loom));

    // Straight draw threading: 0,1,2,3 repeating
    for (int i = 0; i < WARP_ENDS; i++)
        loom->threading.shaft[i] = i % SHAFTS;

    // 2/2 Twill tie-up
    uint8_t twill[4][4] = {
        {1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1}
    };
    memcpy(loom->tieup.matrix, twill, sizeof(twill));

    // Straight treadling
    loom->treadling.sequence[0] = 0;
    loom->treadling.sequence[1] = 1;
    loom->treadling.sequence[2] = 2;
    loom->treadling.sequence[3] = 3;
    loom->treadling.length = 4;
    loom->treadling.direction = 1;

    // Palette: indigo + natural
    loom->palette[0] = (Color){45, 52, 94};   // indigo
    loom->palette[1] = (Color){210, 195, 170}; // natural
    loom->palette_size = 2;

    // Solid warp sett, alternating weft
    loom->warp_sett.indices[0] = 0;
    loom->warp_sett.length = 1;
    loom->weft_color_index = 1;

    // Fill grid with empty (all zeros — will get overwritten as picks advance)
    loom->grid_head = 0;
    loom->pick = 0;

    // Schedule first evolution events (using seed-based RNG — details in Step 6)
    // For now, hardcode initial targets
    loom->next_threading_change   = 200;
    loom->next_tieup_mutation     = 30;
    loom->next_treadling_change   = 100;
    loom->next_treadling_rotation = 50;
    loom->next_weft_color_change  = 20;
}
```

**Checkpoint:** Compiles. Loom struct is populated. No visible change yet (still black window).

---

## Step 3: Drawdown + Color Resolution + One Row

Implement the core formula: compute one row of drawdown and convert to RGB.

**weave.h** additions:

```c
// Compute drawdown for one cell: returns 1 (warp on top) or 0 (weft on top)
int drawdown_cell(const Loom *loom, int x);

// Resolve a cell to a palette index
uint8_t resolve_color(const Loom *loom, int x, int drawdown);

// Compute one new row and append to the ring buffer
void advance_loom(Loom *loom);
```

**weave.c**:

```c
int drawdown_cell(const Loom *loom, int x) {
    // Which treadle is active for the current pick?
    int seq_pos = (int)(loom->pick % (uint32_t)loom->treadling.length);
    if (loom->treadling.direction < 0)
        seq_pos = loom->treadling.length - 1 - seq_pos;
    int treadle = loom->treadling.sequence[seq_pos];

    // Which shaft is this warp end on?
    int shaft = loom->threading.shaft[x];

    // Look up in tie-up matrix
    return loom->tieup.matrix[treadle][shaft];
}

uint8_t resolve_color(const Loom *loom, int x, int drawdown) {
    if (drawdown) {
        // Warp on top — use warp color sett
        return loom->warp_sett.indices[x % loom->warp_sett.length];
    } else {
        // Weft on top — use current weft color
        return loom->weft_color_index;
    }
}

void advance_loom(Loom *loom) {
    int row = loom->grid_head;
    for (int x = 0; x < WARP_ENDS; x++) {
        int dd = drawdown_cell(loom, x);
        loom->grid[row][x] = resolve_color(loom, x, dd);
    }
    loom->grid_head = (loom->grid_head + 1) % VISIBLE_ROWS;
    loom->pick++;
}
```

**main.c** — render the ring buffer to the pixel array:

```c
void render_grid(const Loom *loom, uint8_t *pixels) {
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        // Ring buffer: oldest row is at grid_head, newest is at grid_head-1
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
```

In the main loop, add a tick timer:

```c
Loom loom;
loom_init(&loom, (uint32_t)time(NULL));

// Pre-fill the grid so we don't start with a black screen
for (int i = 0; i < VISIBLE_ROWS; i++)
    advance_loom(&loom);

uint32_t tick_interval_ms = 100; // ~10 rows/sec
uint32_t last_tick = SDL_GetTicks();

// Inside the loop:
if (!loom.paused) {
    uint32_t now = SDL_GetTicks();
    if (now - last_tick >= tick_interval_ms) {
        advance_loom(&loom);
        last_tick = now;
    }
}
render_grid(&loom, pixels);
```

**Checkpoint:** Running the app shows a 2/2 twill pattern in indigo and natural, scrolling upward. The pattern repeats because nothing is evolving yet.

---

## Step 4: Data Libraries (Threading, Treadling, Tie-Up)

Define the curated pattern libraries from the spec as static data. These are just arrays — no logic yet.

**weave.h** additions:

```c
#define MAX_THREADING_REPEAT 16
#define THREADING_COUNT      12
#define TREADLING_SEQ_COUNT  9
#define TIEUP_COUNT          3

typedef struct {
    const char *name;
    uint8_t pattern[MAX_THREADING_REPEAT];
    int length;
} ThreadingEntry;

typedef struct {
    const char *name;
    uint8_t pattern[MAX_SEQUENCE];
    int length;
} TreadlingEntry;

extern const ThreadingEntry THREADING_LIBRARY[THREADING_COUNT];
extern const TreadlingEntry TREADLING_LIBRARY[TREADLING_SEQ_COUNT];
extern const uint8_t TIEUP_LIBRARY[TIEUP_COUNT][TREADLES][SHAFTS];
```

**weave.c** — the data tables:

```c
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

const uint8_t TIEUP_LIBRARY[TIEUP_COUNT][TREADLES][SHAFTS] = {
    // 2/2 Twill
    {{1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1}},
    // Tabby
    {{1,0,1,0}, {0,1,0,1}, {1,0,1,0}, {0,1,0,1}},
    // Point Twill
    {{1,1,0,0}, {1,1,1,0}, {0,1,1,1}, {0,0,1,1}},
};
```

**Checkpoint:** Data compiles. No behavioral change yet — still the same static twill.

---

## Step 5: Simple RNG

A lightweight xorshift PRNG seeded from `time()`. Avoids `rand()`/`srand()` global state. Stored in the Loom struct so results are reproducible given a seed.

**weave.h** additions:

```c
// Add to Loom struct:
uint32_t rng_state;

// RNG functions
uint32_t rng_next(Loom *loom);
int rng_range(Loom *loom, int min, int max); // inclusive range
```

**weave.c**:

```c
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
```

Update `loom_init` to seed:

```c
loom->rng_state = seed ? seed : 1;
```

And randomize the initial scheduling:

```c
loom->next_threading_change   = rng_range(loom, 200, 600);
loom->next_tieup_mutation     = rng_range(loom, 30, 90);
loom->next_treadling_change   = rng_range(loom, 100, 300);
loom->next_treadling_rotation = rng_range(loom, 50, 150);
loom->next_weft_color_change  = rng_range(loom, 20, 80);
```

Also randomize the initial threading/tieup/treadling selection in `loom_init` so each run starts differently:

```c
// Random starting threading
int ti = rng_range(loom, 0, THREADING_COUNT - 1);
for (int i = 0; i < WARP_ENDS; i++)
    loom->threading.shaft[i] = THREADING_LIBRARY[ti].pattern[i % THREADING_LIBRARY[ti].length];

// Random starting tie-up
int ui = rng_range(loom, 0, TIEUP_COUNT - 1);
memcpy(loom->tieup.matrix, TIEUP_LIBRARY[ui], sizeof(loom->tieup.matrix));

// Random starting treadling
int ri = rng_range(loom, 0, TREADLING_SEQ_COUNT - 1);
memcpy(loom->treadling.sequence, TREADLING_LIBRARY[ri].pattern, TREADLING_LIBRARY[ri].length);
loom->treadling.length = TREADLING_LIBRARY[ri].length;
loom->treadling.direction = 1;
```

**Checkpoint:** Each run shows a different starting pattern. Still no evolution.

---

## Step 6: Evolution System

The heart of Weave — each tick, check if any evolution events should fire. Broken into substeps so each event type can be implemented and tested independently.

### Step 6a: check_evolutions scaffold + weft color stepping

The simplest evolution event: step the weft (horizontal thread) color to the next palette entry every 20–80 picks. Also wire up `check_evolutions` in the main loop so all future substeps are automatically active.

**weave.h**:

```c
void check_evolutions(Loom *loom);
```

**weave.c**:

```c
void check_evolutions(Loom *loom) {
    uint32_t p = loom->pick;

    // Weft color step: cycle the horizontal thread color through the palette
    if (p >= loom->next_weft_color_change) {
        loom->weft_color_index = (loom->weft_color_index + 1) % loom->palette_size;
        loom->next_weft_color_change = p + rng_range(loom, 20, 80);
    }
}
```

**main.c** — call `check_evolutions` after `advance_loom`:

```c
if (!loom.paused) {
    uint32_t now = SDL_GetTicks();
    if (now - last_tick >= tick_interval_ms) {
        advance_loom(&loom);
        check_evolutions(&loom);
        last_tick = now;
    }
}
```

**Checkpoint:** You'll see horizontal color banding as the weft color alternates between indigo and natural.

---

### Step 6b: Tie-up mutation

Flip a single bit in the 4×4 tie-up matrix every 30–90 picks. Validate after each flip: reject all-zero rows (pedal raises nothing), all-one rows (no interlacement), and duplicate rows (wasted pedals). Invalid mutations are retried up to 16 times.

**weave.h**:

```c
void mutate_tieup(Loom *loom);
bool validate_tieup(const Tieup *tieup);
```

**weave.c**:

```c
bool validate_tieup(const Tieup *tieup) {
    for (int t = 0; t < TREADLES; t++) {
        int sum = 0;
        for (int s = 0; s < SHAFTS; s++)
            sum += tieup->matrix[t][s];
        if (sum == 0 || sum == SHAFTS) return false;
    }
    // Check for duplicate rows
    for (int a = 0; a < TREADLES; a++)
        for (int b = a + 1; b < TREADLES; b++)
            if (memcmp(tieup->matrix[a], tieup->matrix[b], SHAFTS) == 0)
                return false;
    return true;
}

void mutate_tieup(Loom *loom) {
    for (int attempt = 0; attempt < 16; attempt++) {
        Tieup trial = loom->tieup;
        int t = rng_range(loom, 0, TREADLES - 1);
        int s = rng_range(loom, 0, SHAFTS - 1);
        trial.matrix[t][s] ^= 1;
        if (validate_tieup(&trial)) {
            loom->tieup = trial;
            return;
        }
    }
}
```

Add to `check_evolutions`:

```c
if (p >= loom->next_tieup_mutation) {
    mutate_tieup(loom);
    loom->next_tieup_mutation = p + rng_range(loom, 30, 90);
}
```

**Checkpoint:** The weave texture gradually morphs — twill drifts toward satin, then something unexpected. Changes are subtle since only one bit flips at a time.

---

### Step 6c: Treadling evolution (replace, reverse, tromp-as-writ) and rotation

Two events on separate timers:

- **Treadling replacement** (every 100–300 picks): swap the entire pedal sequence. Randomly picks one of three actions — replace from library, reverse direction, or tromp-as-writ (copy threading as treadling for symmetrical results).
- **Treadling rotation** (every 50–150 picks): shift the sequence start by one position (`[0,1,2,3]` → `[1,2,3,0]`), creating a diagonal drift.

**weave.h**:

```c
void evolve_treadling(Loom *loom);
void rotate_treadling(Loom *loom);
```

**weave.c**:

```c
void evolve_treadling(Loom *loom) {
    int action = rng_range(loom, 0, 2);
    switch (action) {
        case 0: { // Replace from library
            int ri = rng_range(loom, 0, TREADLING_SEQ_COUNT - 1);
            memcpy(loom->treadling.sequence, TREADLING_LIBRARY[ri].pattern,
                   TREADLING_LIBRARY[ri].length);
            loom->treadling.length = TREADLING_LIBRARY[ri].length;
            break;
        }
        case 1: // Reverse direction
            loom->treadling.direction *= -1;
            break;
        case 2: { // Tromp-as-writ — copy threading pattern into treadling
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
            break;
        }
    }
}

void rotate_treadling(Loom *loom) {
    uint8_t first = loom->treadling.sequence[0];
    for (int i = 0; i < loom->treadling.length - 1; i++)
        loom->treadling.sequence[i] = loom->treadling.sequence[i + 1];
    loom->treadling.sequence[loom->treadling.length - 1] = first;
}
```

Add to `check_evolutions`:

```c
if (p >= loom->next_treadling_change) {
    evolve_treadling(loom);
    loom->next_treadling_change = p + rng_range(loom, 100, 300);
}

if (p >= loom->next_treadling_rotation) {
    rotate_treadling(loom);
    loom->next_treadling_rotation = p + rng_range(loom, 50, 150);
}
```

**Checkpoint:** The rhythm of the weave shifts — diagonal drifts from rotation, bigger structural changes from replacement. Direction reversals create mirror/chevron effects.

---

### Step 6d: Threading change + warp sett

The rarest and most dramatic event (every 200–600 picks): pick a new threading from the library, replacing the shaft assignments for all 64 warp ends instantly. Also randomize the warp color sett at the same time, since on a real loom the warp colors are set when you thread up.

**weave.h**:

```c
void randomize_threading(Loom *loom);
```

**weave.c**:

```c
void randomize_threading(Loom *loom) {
    int ti = rng_range(loom, 0, THREADING_COUNT - 1);
    const ThreadingEntry *e = &THREADING_LIBRARY[ti];
    for (int i = 0; i < WARP_ENDS; i++)
        loom->threading.shaft[i] = e->pattern[i % e->length];

    // Also change warp sett (aligned with threading per spec)
    int sett_type = rng_range(loom, 0, 3);
    switch (sett_type) {
        case 0: // Solid
            loom->warp_sett.indices[0] = rng_range(loom, 0, loom->palette_size - 1);
            loom->warp_sett.length = 1;
            break;
        case 1: // Alternating
            loom->warp_sett.indices[0] = 0;
            loom->warp_sett.indices[1] = 1;
            loom->warp_sett.length = 2;
            break;
        case 2: // Tartan-like
            loom->warp_sett.indices[0] = 0; loom->warp_sett.indices[1] = 0;
            loom->warp_sett.indices[2] = 1; loom->warp_sett.indices[3] = 1;
            loom->warp_sett.indices[4] = 0; loom->warp_sett.indices[5] = 0;
            loom->warp_sett.indices[6] = 2 % loom->palette_size;
            loom->warp_sett.indices[7] = 2 % loom->palette_size;
            loom->warp_sett.length = 8;
            break;
        case 3: // Gradient
            for (int i = 0; i < loom->palette_size && i < MAX_SETT; i++)
                loom->warp_sett.indices[i] = i;
            loom->warp_sett.length = loom->palette_size;
            break;
    }
}
```

Add to `check_evolutions`:

```c
if (p >= loom->next_threading_change) {
    randomize_threading(loom);
    loom->next_threading_change = p + rng_range(loom, 200, 600);
}
```

**Checkpoint:** All five evolution events are now active. The fabric evolves at multiple timescales — slow dramatic threading swaps, moderate treadling shifts, fast tie-up morphing, and rapid weft color banding.

---

## Step 7: Float Validation

Add float-length checking so tie-up mutations that create degenerate patterns are rejected.

**weave.c**:

```c
// Check max float length for a given column over recent rows.
// We check a window of rows around the current pick by simulating
// what the drawdown would look like with the candidate state.
bool validate_floats(const Loom *loom, int max_float) {
    // Check warp floats (vertical runs of same state in each column)
    // Simulate enough rows to catch max_float violations
    int check_rows = max_float + 1;
    for (int x = 0; x < WARP_ENDS; x++) {
        int run = 1;
        int prev = -1;
        for (int dy = 0; dy < check_rows * loom->treadling.length; dy++) {
            // Simulate drawdown at pick (loom->pick + dy)
            int seq_pos = (int)((loom->pick + dy) % (uint32_t)loom->treadling.length);
            if (loom->treadling.direction < 0)
                seq_pos = loom->treadling.length - 1 - seq_pos;
            int treadle = loom->treadling.sequence[seq_pos];
            int shaft = loom->threading.shaft[x];
            int val = loom->tieup.matrix[treadle][shaft];

            if (val == prev) {
                run++;
                if (run > max_float) return false;
            } else {
                run = 1;
            }
            prev = val;
        }
    }

    // Check weft floats (horizontal runs in a single row)
    for (int dy = 0; dy < loom->treadling.length; dy++) {
        int run = 1;
        int prev = -1;
        for (int x = 0; x < WARP_ENDS; x++) {
            int seq_pos = (int)((loom->pick + dy) % (uint32_t)loom->treadling.length);
            if (loom->treadling.direction < 0)
                seq_pos = loom->treadling.length - 1 - seq_pos;
            int treadle = loom->treadling.sequence[seq_pos];
            int shaft = loom->threading.shaft[x];
            int val = loom->tieup.matrix[treadle][shaft];

            if (val == prev) {
                run++;
                if (run > max_float) return false;
            } else {
                run = 1;
            }
            prev = val;
        }
    }
    return true;
}
```

Update `mutate_tieup` to include float validation:

```c
void mutate_tieup(Loom *loom) {
    Tieup original = loom->tieup;
    for (int attempt = 0; attempt < 16; attempt++) {
        Tieup trial = original;
        int t = rng_range(loom, 0, TREADLES - 1);
        int s = rng_range(loom, 0, SHAFTS - 1);
        trial.matrix[t][s] ^= 1;
        if (validate_tieup(&trial)) {
            // Temporarily apply and check floats
            loom->tieup = trial;
            if (validate_floats(loom, MAX_FLOAT)) {
                return; // Accept
            }
            loom->tieup = original; // Revert
        }
    }
}
```

**Checkpoint:** No visible change in most cases — floats were already short with 2/2 twill. But now degenerate mutations are rejected. Edge cases won't produce ugly long runs.

---

## Step 8: Color Palettes

Add a palette library and palette cycling.

**weave.h** additions:

```c
#define PALETTE_COUNT 5

typedef struct {
    const char *name;
    Color colors[MAX_PALETTE];
    int size;
} PaletteEntry;

extern const PaletteEntry PALETTE_LIBRARY[PALETTE_COUNT];

// Add to Loom struct:
int current_palette_index;
```

**weave.c**:

```c
const PaletteEntry PALETTE_LIBRARY[PALETTE_COUNT] = {
    {"Indigo & Natural", {{45,52,94}, {210,195,170}, {80,85,128}, {180,165,140}}, 4},
    {"Earth",            {{101,67,33}, {181,137,85}, {139,90,43}, {222,199,163}}, 4},
    {"High Contrast",    {{20,20,20}, {235,235,230}, {180,40,30}, {50,50,55}},    4},
    {"Ocean",            {{15,30,60}, {40,80,120}, {100,160,180}, {200,220,225}}, 4},
    {"Forest",           {{25,50,25}, {60,100,45}, {140,160,80}, {200,190,140}},  4},
};
```

In `loom_init`, load a random palette:

```c
loom->current_palette_index = rng_range(loom, 0, PALETTE_COUNT - 1);
const PaletteEntry *pal = &PALETTE_LIBRARY[loom->current_palette_index];
memcpy(loom->palette, pal->colors, sizeof(Color) * pal->size);
loom->palette_size = pal->size;
```

**Checkpoint:** Each run starts with a different color palette. The `C` key (Step 9) will cycle through them.

---

## Step 9: Keyboard Controls

Handle all the keys from the spec table.

**main.c** — inside the event loop:

```c
case SDL_KEYDOWN:
    switch (e.key.keysym.sym) {
        case SDLK_SPACE:
            loom.paused = !loom.paused;
            break;
        case SDLK_UP:
            if (tick_interval_ms > 20) tick_interval_ms -= 20;
            break;
        case SDLK_DOWN:
            tick_interval_ms += 20;
            break;
        case SDLK_t:
            randomize_threading(&loom);
            break;
        case SDLK_u:
            mutate_tieup(&loom);
            break;
        case SDLK_r:
            evolve_treadling(&loom);
            break;
        case SDLK_c: {
            loom.current_palette_index =
                (loom.current_palette_index + 1) % PALETTE_COUNT;
            const PaletteEntry *pal =
                &PALETTE_LIBRARY[loom.current_palette_index];
            memcpy(loom.palette, pal->colors,
                   sizeof(Color) * pal->size);
            loom.palette_size = pal->size;
            break;
        }
        case SDLK_d:
            show_debug = !show_debug;
            break;
        case SDLK_ESCAPE:
        case SDLK_q:
            running = 0;
            break;
    }
    break;
```

Skip screenshot (`S`) for now — it requires either stb_image_write or raw BMP writing. Can add later or skip entirely for V1.

**Checkpoint:** Full interactive control. Speed up/down, force mutations, cycle palettes, pause/resume.

---

## Step 10: Debug Overlay

When `D` is pressed, draw the traditional draft notation as additional colored rectangles in the margins. This requires resizing the window to make room, or drawing on top of the grid.

**Approach:** When debug is on, expand the texture to include margin areas. Use a larger texture with the draft notation rendered into the margin regions.

```
Layout with debug overlay:

     [Threading: 64 cols × 4 rows] [Tie-up: 4×4]
     [                           ] [           ]
     [     Drawdown: 64×64       ] [Treadling: ]
     [                           ] [ 4 cols ×  ]
     [                           ] [ 64 rows   ]
```

Total size with margins: (64+4) × (64+4) = 68×68 logical cells.

**main.c**:

```c
#define DEBUG_MARGIN 4 // 4 cells for shaft/treadle display
#define TEX_W (WARP_ENDS + DEBUG_MARGIN)   // 68
#define TEX_H (VISIBLE_ROWS + DEBUG_MARGIN) // 68

// Allocate for the larger size always; only populate margins when debug is on
uint8_t pixels[TEX_H * TEX_W * 3];
```

Rendering function for the debug overlay:

```c
void render_debug(const Loom *loom, uint8_t *pixels, int tex_w) {
    Color on  = {255, 255, 255};
    Color off = {40, 40, 40};

    // Threading: top 4 rows, first 64 columns
    // Each cell (x, shaft_row) is filled if threading.shaft[x] == shaft_row
    for (int shaft_row = 0; shaft_row < SHAFTS; shaft_row++) {
        for (int x = 0; x < WARP_ENDS; x++) {
            Color c = (loom->threading.shaft[x] == (SHAFTS - 1 - shaft_row)) ? on : off;
            int off3 = (shaft_row * tex_w + x) * 3;
            pixels[off3] = c.r; pixels[off3+1] = c.g; pixels[off3+2] = c.b;
        }
    }

    // Tie-up: top-right corner, 4×4
    for (int t = 0; t < TREADLES; t++) {
        for (int s = 0; s < SHAFTS; s++) {
            Color c = loom->tieup.matrix[t][SHAFTS - 1 - s] ? on : off;
            int px = WARP_ENDS + t;
            int py = s;
            int off3 = (py * tex_w + px) * 3;
            pixels[off3] = c.r; pixels[off3+1] = c.g; pixels[off3+2] = c.b;
        }
    }

    // Treadling: right side, 4 columns × 64 rows
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        int seq_pos = (int)((loom->pick - VISIBLE_ROWS + y) % (uint32_t)loom->treadling.length);
        if (loom->treadling.direction < 0)
            seq_pos = loom->treadling.length - 1 - seq_pos;
        int active_treadle = loom->treadling.sequence[seq_pos < 0 ? seq_pos + loom->treadling.length : seq_pos];
        for (int t = 0; t < TREADLES; t++) {
            Color c = (t == active_treadle) ? on : off;
            int px = WARP_ENDS + t;
            int py = SHAFTS + y; // below the tie-up
            int off3 = (py * tex_w + px) * 3;
            pixels[off3] = c.r; pixels[off3+1] = c.g; pixels[off3+2] = c.b;
        }
    }
}
```

When debug is on, use the 68×68 texture; when off, use a 64×64 texture. To keep it simple, always allocate the 68×68 texture and adjust `SDL_RenderCopy` source rect:

```c
SDL_Rect src;
if (show_debug) {
    src = (SDL_Rect){0, 0, TEX_W, TEX_H};
} else {
    src = (SDL_Rect){0, DEBUG_MARGIN, WARP_ENDS, VISIBLE_ROWS};
}
```

Offset the drawdown rendering to start at row `DEBUG_MARGIN` in the pixel buffer, so the top 4 rows are reserved for threading:

```c
void render_grid(const Loom *loom, uint8_t *pixels, int tex_w, int y_offset) {
    for (int y = 0; y < VISIBLE_ROWS; y++) {
        int src_row = (loom->grid_head + y) % VISIBLE_ROWS;
        for (int x = 0; x < WARP_ENDS; x++) {
            uint8_t ci = loom->grid[src_row][x];
            Color c = loom->palette[ci % loom->palette_size];
            int off = ((y + y_offset) * tex_w + x) * 3;
            pixels[off]     = c.r;
            pixels[off + 1] = c.g;
            pixels[off + 2] = c.b;
        }
    }
}
```

**Checkpoint:** Pressing `D` shows the full draft notation — threading at top, tie-up in corner, treadling on right. The relationship between the three components and the visual output is immediately visible.

---

## Step 11: Polish + Final Wiring

Clean up, test edge cases, finalize:

1. **Palette hue drift** — slowly rotate palette HSV hue each tick:
   ```c
   // In check_evolutions, every N picks, nudge each color's hue by 1°
   // Simple approximation: rotate R→G→B channels slightly
   ```
   This is subtle — skip if it adds complexity. Can add post-V1.

2. **Window title** — show current state info:
   ```c
   char title[128];
   snprintf(title, sizeof(title), "Weave — pick %u | %s",
            loom.pick, loom.paused ? "PAUSED" : "running");
   SDL_SetWindowTitle(window, title);
   ```

3. **Pre-fill grid** — so the window isn't blank on startup (already in Step 3).

4. **Verify all spec constraints** — float limit, tie-up validation, evolution intervals.

5. **Test with each starting tie-up/threading/treadling** — force `T`, `U`, `R` keys and confirm transitions look good.

**Checkpoint:** Feature-complete V1. The loom runs, evolves, responds to keys, shows debug overlay.

---

## Progress

- [x] Step 1 — Scaffolding (Makefile, SDL2 window)
- [x] Step 2 — Data structures + Loom initialization
- [x] Step 3 — Drawdown formula + color resolution + scrolling render
- [x] Step 4 — Data libraries (threading, treadling, tie-up tables)
- [x] Step 5 — RNG + randomized starting state each run
- [x] Step 6a — Weft color stepping
- [ ] Step 6b — Tie-up mutation + validation
- [ ] Step 6c — Treadling evolution + rotation
- [ ] Step 6d — Threading change + warp sett
- [ ] Step 7 — Float validation (fold into 6b)
- [x] Step 8 — Color palettes (5 palettes, 4 colors each)
- [x] Step 9 — Keyboard controls (pause, speed, palette cycle, debug toggle)
  - [ ] T/U/R force-mutation keys (blocked on 6b–6d)
- [x] Step 10 — Debug overlay (threading/tie-up/treadling in margins)
- [ ] Step 11 — Polish (window title, edge cases, final testing)
