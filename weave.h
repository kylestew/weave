#ifndef WEAVE_H
#define WEAVE_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Warp = vertical threads strung on the loom before weaving begins
// Weft = horizontal thread passed through on each pick (row)
// Shaft = frame holding heddles (wire loops) that warp threads pass through
// Treadle = foot pedal — pressing one raises whichever shafts are tied to it
#define SHAFTS       4   // number of shaft frames
#define TREADLES     4   // number of foot pedals
#define WARP_ENDS    64  // number of vertical warp threads (= grid columns)
#define VISIBLE_ROWS 64  // rows of fabric visible on screen
#define MAX_SEQUENCE 32  // max treadling sequence length
#define MAX_SETT     16  // max warp color sett repeat length
#define MAX_PALETTE  8   // max colors in a palette
#define MAX_FLOAT    7   // max consecutive same-state cells before rejection

typedef struct {
    uint8_t r, g, b;
} Color;

// --- Library types and constants ---

#define MAX_THREADING_REPEAT 16  // longest threading pattern repeat
#define THREADING_COUNT      12  // number of classic threading patterns
#define TREADLING_SEQ_COUNT  9   // number of classic treadling sequences
#define TIEUP_COUNT          6   // number of starting tie-up presets
#define PALETTE_COUNT        10  // number of color palettes

// Threading library entry: a named repeating pattern of shaft assignments.
// The pattern tiles across all 64 warp ends.
typedef struct {
    const char *name;
    uint8_t pattern[MAX_THREADING_REPEAT];  // shaft numbers (0–3)
    int length;                             // repeat length
} ThreadingEntry;

// Treadling library entry: a named repeating sequence of treadle (pedal) presses.
// Cycles for each successive pick (row).
typedef struct {
    const char *name;
    uint8_t pattern[MAX_SEQUENCE];          // treadle numbers (0–3)
    int length;                             // sequence length
} TreadlingEntry;

// A named color palette — the actual RGB values for the fabric.
typedef struct {
    const char *name;
    Color colors[MAX_PALETTE];
    int size;
} PaletteEntry;

extern const ThreadingEntry THREADING_LIBRARY[THREADING_COUNT];
extern const TreadlingEntry TREADLING_LIBRARY[TREADLING_SEQ_COUNT];
extern const uint8_t TIEUP_LIBRARY[TIEUP_COUNT][TREADLES][SHAFTS];
extern const char *TIEUP_NAMES[TIEUP_COUNT];
extern const PaletteEntry PALETTE_LIBRARY[PALETTE_COUNT];

// --- Loom component types ---

// Threading: which shaft (frame) each vertical warp thread is tied to.
// This is the x-axis lookup — displayed across the top in draft notation.
// On a real loom, changing this means physically re-threading every heddle.
typedef struct {
    uint8_t shaft[WARP_ENDS];               // shaft assignment per warp end (0–3)
} Threading;

// Tie-up: the 4×4 matrix connecting treadles (foot pedals) to shafts (frames).
// Each row is a treadle; a 1 means pressing that pedal raises that shaft.
// One pedal can raise multiple shafts. Displayed in the top-right corner
// of draft notation.
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
    int current_palette_index;              // which palette from the library

    uint32_t pick;                          // current pick number (pick = one weft pass = one row)
    uint32_t next_threading_change;         // when to re-thread (rare, dramatic)
    uint32_t next_tieup_mutation;           // when to flip one tie-up bit (frequent, subtle)
    uint32_t next_treadling_change;         // when to swap treadling sequence (moderate)
    uint32_t next_treadling_rotation;       // when to rotate treadling start (frequent, subtle)
    uint32_t next_weft_color_change;        // when to step weft color (frequent)

    uint8_t grid[VISIBLE_ROWS][WARP_ENDS];  // ring buffer of palette indices (the fabric)
    int grid_head;                          // ring buffer write position

    uint32_t rng_state;                     // xorshift PRNG state

    const char *threading_name;             // current threading pattern name
    const char *treadling_name;             // current treadling sequence name
    const char *tieup_name;                 // current tie-up preset name

    int paused;
} Loom;

void loom_init(Loom *loom, uint32_t seed);
uint32_t rng_next(Loom *loom);
int rng_range(Loom *loom, int min, int max);

// Compute drawdown for one cell: returns 1 (warp on top) or 0 (weft on top)
int drawdown_cell(const Loom *loom, int x);

// Resolve a drawdown cell to a palette index using warp sett or weft color
uint8_t resolve_color(const Loom *loom, int x, int drawdown);

// Compute one new row of fabric and append to the ring buffer
void advance_loom(Loom *loom);

// Check all evolution timers and fire any that are due
void check_evolutions(Loom *loom);

// Flip one bit in the tie-up matrix; rejects invalid or degenerate results
void mutate_tieup(Loom *loom);
bool validate_tieup(const Tieup *tieup);
bool validate_floats(const Loom *loom, int max_float);

// Pick a new threading from the library + randomize the warp color sett
void randomize_threading(Loom *loom);
// Treadling: replace sequence, reverse, or switch to tromp-as-writ
void evolve_treadling(Loom *loom);
// Shift the treadling sequence start by one position (diagonal drift)
void rotate_treadling(Loom *loom);

#endif
