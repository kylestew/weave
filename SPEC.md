# Weave — Generative Loom

A generative application that simulates an endlessly weaving loom. The three components of a weaving draft _(the complete pattern specification)_ — threading _(which shaft each warp thread lives on)_, tie-up _(which treadles activate which shafts)_, and treadling _(which treadle is pressed each row)_ — independently evolve over time, producing a continuously scrolling fabric that never repeats.

The output is a 64x64 pixel grid rendered via SDL2, similar to [PXL](https://github.com/kylestew/PXL). New rows appear at the bottom; the whole grid scrolls upward. The viewer watches the fabric grow, transition, and transform.

---

## Core Model: The Virtual Loom

The program simulates a 4-shaft floor loom with 64 warp ends _(warp = the vertical threads strung on the loom before weaving; weft = the horizontal thread passed through on each pick/row)_. At each tick, it produces one new row of fabric by evaluating the standard drawdown formula _(drawdown = the simulated view of how the fabric looks, computed cell by cell)_:

```
Drawdown[x] = Tieup[Treadling[y]][Threading[x]]
```

Where:

- `x` is the column (0–63), representing a warp end _(one vertical thread)_
- `y` is the current pick number _(pick = one pass of the weft; one row of fabric)_ (monotonically increasing, never resets)
- `Threading[x]` returns a shaft number (0–3) for warp end x _(which of the 4 shaft frames this thread is tied to)_
- `Treadling[y]` returns a treadle number (0–3) for pick y _(which foot pedal is pressed for this row)_
- `Tieup[treadle][shaft]` returns 1 (warp on top) or 0 (weft on top) _(the matrix connecting pedals to shaft frames)_

The result is a 64-wide binary row: each cell is either warp-visible or weft-visible. With a color sett _(the repeating pattern of warp thread colors)_ applied, this becomes the fabric's visual output.

### Constraints

The simulation respects real loom constraints to keep output looking like plausible fabric:

- **4 shafts, 4 treadles** — the sweet spot for handweaving. Enough complexity for twills _(diagonal-line weave structures, like denim)_, overshot _(complex supplementary-weft patterns)_, and satin-like structures, but constrained enough to feel woven rather than arbitrary.
  _(A **shaft** is a frame holding heddles — wire loops that warp threads pass through. Raising a shaft lifts all the warp ends threaded through it, opening a "shed" _(the gap between raised and lowered threads)_ for the weft to pass through. A **treadle** is a foot pedal. Pressing a treadle raises whichever shafts are connected to it, as defined by the tie-up matrix. So: threading determines which shaft each warp end lives on; the tie-up wires treadles to shafts; treadling says which treadle you press for each pick.)_
- **No impossible sheds** — all-up and all-down are excluded from the tie-up (these produce no interlacement _(no crossing of warp and weft)_ and aren't physically weave-able). _(If every shaft is up, every warp thread is raised — the weft slides under all of them without ever going over one, so nothing is bound. If no shaft is up, the weft floats over the entire width. Either way, the two thread systems never cross, so there's no interlacement and no fabric is formed.)_
- **Float limit** — maximum warp or weft float length of 7. If a mutation would create longer floats, it is rejected. This prevents the degenerate case of long unbound floats that would fall apart on a real loom.
  _(A **float** is when a thread passes over multiple consecutive threads of the other system without interlacing. A 4-shaft satin has warp floats of 4 (one warp thread goes over 4 weft threads before being bound). Long floats look smooth but are structurally weak: they snag, shift, and won't hold their position on a real loom. Float length 7 is a reasonable outer limit — beyond that the structure becomes impractical.)_

---

## The Three Evolving Components

### 1. Threading

**What it is:** An array of 64 shaft assignments (values 0–3), one per warp end. _(Think of it as the x-axis lookup: for each column, which shaft frame is that thread tied to? Displayed as a grid across the top in traditional draft notation.)_

**How it evolves:** Threading changes are the most dramatic — on a real loom, re-threading means physically moving each warp thread through different heddles _(the wire loops on each shaft)_, which takes hours. The program reflects this by changing threading infrequently and abruptly.

- At random intervals (every 200–600 picks), a new threading is selected from the **master threading library**.
- The new threading is **not** blended or crossfaded — it replaces the old one instantly, as it would on a real loom. The visual break this creates is intentional; it mirrors the distinct character change when a weaver re-threads.
- Optionally, the transition can happen in **sections** — re-threading the left half, then the right half over successive intervals — simulating partial re-threading.

**Master Threading Library:**

A curated list of classic threadings, each defined as a repeating unit that tiles across 64 ends. _(The Pattern column is **shaft assignments per warp end** — x-axis, not pedal presses per pick. e.g. Straight draw `0,1,2,3` tiling means end 0→shaft 0, end 1→shaft 1, end 2→shaft 2, end 3→shaft 3, end 4→shaft 0, … For pedal-press sequences, see the Treadling Library below.)_

| Name                     | Repeat | Pattern                                        |
| ------------------------ | ------ | ---------------------------------------------- |
| Straight draw            | 4      | 0, 1, 2, 3                                     |
| Point twill (diamond)    | 6      | 0, 1, 2, 3, 2, 1                               |
| Rosepath                 | 8      | 0, 1, 2, 3, 2, 1, 0, 3                         |
| Broken twill             | 8      | 0, 1, 2, 3, 1, 0, 3, 2                         |
| Extended point           | 10     | 0, 1, 2, 2, 3, 3, 2, 2, 1, 0                   |
| Monk's belt              | 8      | 0, 1, 0, 1, 2, 3, 2, 3                         |
| M's and O's              | 8      | 0, 1, 0, 1, 2, 3, 2, 3                         |
| Overshot (blooming leaf) | 16     | 0, 1, 0, 2, 1, 2, 1, 3, 0, 3, 0, 2, 1, 2, 1, 0 |
| Huck lace                | 6      | 0, 1, 0, 2, 3, 2                               |
| Advancing twill          | 8      | 0, 1, 2, 3, 1, 2, 3, 0                         |
| Crepe                    | 8      | 0, 2, 1, 3, 2, 0, 3, 1                         |
| Undulating twill         | 12     | 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3             |

The library is stored as data (an array of arrays) and is easy to extend.

### 2. Tie-Up

**What it is:** A 4x4 boolean matrix. `Tieup[treadle][shaft]` = 1 means pressing that treadle raises that shaft. _(This is the small grid in the top-right corner of a traditional draft — 4 rows (treadles/pedals) × 4 columns (shafts/frames), each cell a checkbox.)_

**How it evolves:** The tie-up mutates slowly and continuously — a single bit flip at a time.

- Every 30–90 picks, one randomly chosen cell in the tie-up is toggled (0→1 or 1→0).
- After each mutation, the tie-up is **validated**:
    - No all-zero rows (a treadle that raises nothing is useless).
    - No all-one rows (a treadle that raises everything produces no interlacement).
    - No duplicate rows (redundant treadles waste the limited set).
    - The resulting drawdown segment doesn't violate the float limit.
- If the mutation fails validation, it is rejected and a different random mutation is tried.
- This creates a slow drift in the character of the fabric — twill-like regions gradually morph into satin-like regions, then into something unexpected.

**Starting tie-ups:** The simulation begins with a randomly selected classic tie-up:

```
2/2 Twill:     Tabby:        Point Twill:
  1 1 0 0        1 0 1 0        1 1 0 0
  0 1 1 0        0 1 0 1        1 1 1 0
  0 0 1 1        1 0 1 0        0 1 1 1
  1 0 0 1        0 1 0 1        0 0 1 1
```

_(2/2 Twill = two up, two down, shifting each row — produces diagonal lines. Tabby = plain weave, the simplest structure: over one under one, alternating. Point Twill = twill that reverses direction, producing chevrons or diamonds.)_

### 3. Treadling

**What it is:** A repeating sequence that determines which treadle _(foot pedal)_ is pressed for each pick _(row)_. The sequence has a period (length) and cycles. _(Think of it as the y-axis lookup: for each row, which pedal does the weaver step on? Displayed as a column on the right side in traditional draft notation.)_

**How it evolves:** The treadling sequence changes at a moderate rate — faster than threading, slower than tie-up mutations.

- **Sequence rotation:** Every 50–150 picks, the treadling sequence shifts its starting point (e.g., `[0,1,2,3]` → `[1,2,3,0]`). This produces a diagonal shift in the pattern.
- **Sequence replacement:** Every 100–300 picks, the treadling sequence is replaced with a new one from the **treadling library** or generated procedurally.
- **Direction reversal:** Occasionally the treadling reverses direction (`[0,1,2,3]` → `[3,2,1,0]`), producing a mirror effect that creates diamond or chevron-like forms in combination with point threadings.
- **Tromp-as-writ:** Periodically, the treadling switches to **tromp-as-writ** mode — the treadling sequence mirrors the threading pattern _(same numbers, just read vertically instead of horizontally)_. This is a traditional technique that produces symmetrical, often surprising results.

**Treadling Library:**

| Name        | Sequence               |
| ----------- | ---------------------- |
| Straight    | 0, 1, 2, 3             |
| Reverse     | 3, 2, 1, 0             |
| Point       | 0, 1, 2, 3, 2, 1       |
| Alternating | 0, 2, 1, 3             |
| Rose        | 0, 1, 2, 3, 2, 1, 0, 3 |
| Extended    | 0, 0, 1, 1, 2, 2, 3, 3 |
| Syncopated  | 0, 1, 0, 2, 0, 3       |
| Doubled     | 0, 1, 2, 3, 0, 1, 2, 3 |
| Offset      | 0, 2, 1, 3, 2, 0, 3, 1 |

---

## Color

### Color Model

Each row of the drawdown is binary: warp-on-top (1) or weft-on-top (0). Color comes from two sources:

- **Warp color:** determined by the column position `x` and the current warp color sett _(the repeating color stripe pattern across the warp threads)_
- **Weft color:** determined by the current pick's weft color _(a single color per row — the color of the horizontal thread being woven in)_

At each pixel:

```
if drawdown[x] == 1:
    pixel_color = warp_sett[x % warp_sett_length]
else:
    pixel_color = weft_color
```

### Color Setts

A color sett _(from "setting" the warp — the pre-planned color order of warp threads)_ is a repeating sequence of color indices applied across the warp. Classic setts:

| Name        | Pattern                  | Example Colors |
| ----------- | ------------------------ | -------------- |
| Solid       | [0]                      | All one color  |
| Alternating | [0, 1]                   | Dark, light    |
| Tartan-like | [0, 0, 1, 1, 0, 0, 2, 2] | Bands of color |
| Gradient    | [0, 1, 2, 3]             | Gradual shift  |

### Palette

A small palette (4–8 colors) defines the actual RGB values. The palette itself can slowly rotate or shift over time to add visual variety without changing the structure.

**Starting palette ideas:**

- Indigo and natural (undyed cotton / deep blue) — classic Japanese shibori feel
- Earth tones — browns, tans, rusts
- High contrast — black and white with one accent color

### Color Evolution

- **Weft color** changes every 20–80 picks by stepping to the next color in the palette. This creates horizontal color banding that interacts with the weave structure.
- **Warp sett** changes infrequently (aligned with threading changes) since on a real loom, the warp colors are fixed at setup time _(you can't change warp colors mid-weave without re-warping the whole loom)_.
- **Palette drift:** The entire palette's hue can slowly rotate (1° per N picks) to create an imperceptible but cumulative shift in mood.

---

## Rendering

### Display

- **Grid:** 64 columns x 64 visible rows
- **Pixel scale:** Each grid cell rendered at 8x or larger (512x512 window minimum), nearest-neighbor upscaling for crisp pixels
- **Scroll:** Each tick, a new row is computed and inserted at the bottom. All existing rows shift up by one. The top row is discarded.
- **Tick rate:** Configurable. Default ~8–12 rows per second (slow enough to watch patterns form, fast enough to see structure emerge). Could tie to a BPM for rhythmic feel.

### SDL2 Implementation

Following PXL's architecture:

- Single SDL2 window with a streaming RGB24 texture
- CPU-rendered: compute the 64-wide row, write into a pixel buffer, upload texture each frame
- Nearest-neighbor scaling (`SDL_HINT_RENDER_SCALE_QUALITY = "0"`)
- The framebuffer is a ring buffer of 64 rows — instead of shifting memory, advance a write pointer and render with an offset

### Keyboard Controls

| Key            | Action                                                           |
| -------------- | ---------------------------------------------------------------- |
| `Space`        | Pause / resume                                                   |
| `Up/Down`      | Increase / decrease tick rate                                    |
| `T`            | Force a threading change now                                     |
| `U`            | Force a tie-up mutation now                                      |
| `R`            | Force a treadling change now                                     |
| `C`            | Cycle to next color palette                                      |
| `S`            | Screenshot (save current grid as PNG)                            |
| `D`            | Toggle debug overlay (show current threading, tie-up, treadling) |
| `Escape` / `Q` | Quit                                                             |

### Debug Overlay

When enabled, render the traditional draft notation in the margins:

- **Top:** Threading grid (64 columns x 4 rows, showing shaft assignments) _(x-axis: which shaft each warp end is on)_
- **Top-right corner:** Tie-up matrix (4x4) _(the connector: which treadles raise which shafts)_
- **Right side:** Treadling sequence (the last 64 picks, 4 columns x 64 rows) _(y-axis: which treadle was pressed each row)_

This makes the relationship between the draft components and the visual output immediately legible.

---

## Architecture

### Language

C99, following PXL's approach. Zero or minimal heap allocation. The entire state fits in a handful of small structs.

### Core Data Structures

```c
#define SHAFTS      4
#define TREADLES    4
#define WARP_ENDS   64
#define VISIBLE_ROWS 64
#define MAX_SEQUENCE 32
#define MAX_SETT     16
#define MAX_PALETTE  8

typedef struct {
    uint8_t shaft[WARP_ENDS];               // Threading: shaft assignment per end
} Threading;

typedef struct {
    uint8_t matrix[TREADLES][SHAFTS];       // Tie-up: boolean matrix
} Tieup;

typedef struct {
    uint8_t sequence[MAX_SEQUENCE];         // Treadle indices
    int length;                             // Sequence period
    int direction;                          // +1 or -1 (for reversal)
} Treadling;

typedef struct {
    uint8_t indices[MAX_SETT];              // Palette indices per warp position
    int length;                             // Sett repeat length
} ColorSett;

typedef struct {
    uint8_t r, g, b;
} Color;

typedef struct {
    Threading threading;
    Tieup tieup;
    Treadling treadling;

    ColorSett warp_sett;
    uint8_t weft_color_index;               // Current weft palette index
    Color palette[MAX_PALETTE];
    int palette_size;

    uint32_t pick;                          // Current pick number (monotonic)
    uint32_t next_threading_change;         // Pick number for next threading swap
    uint32_t next_tieup_mutation;           // Pick number for next tie-up bit flip
    uint32_t next_treadling_change;         // Pick number for next treadling evolution
    uint32_t next_weft_color_change;        // Pick number for next weft color step

    uint8_t grid[VISIBLE_ROWS][WARP_ENDS];  // Ring buffer of drawdown rows (palette indices)
    int grid_head;                          // Ring buffer write position
} Loom;
```

### Main Loop

```
initialize loom with random starting state
open SDL2 window

loop:
    if not paused and tick timer elapsed:
        advance_loom(&loom)     // compute one new row
        check_evolutions(&loom) // maybe mutate threading/tieup/treadling/color

    render_grid(&loom, buffer)  // write grid to RGB buffer
    upload texture, present frame
    handle SDL events
```

### Key Functions

```c
// Compute one row of drawdown and append to grid
void advance_loom(Loom *loom);

// Check timers and apply mutations
void check_evolutions(Loom *loom);

// Evaluate one cell of the drawdown
int drawdown_cell(Loom *loom, int x);
// = loom->tieup.matrix[current_treadle][loom->threading.shaft[x]]

// Map drawdown result + colors to a palette index
uint8_t resolve_color(Loom *loom, int x, int drawdown_value);

// Validate a tie-up after mutation
bool validate_tieup(Tieup *tieup);

// Check float constraint for current state
bool validate_floats(Loom *loom, int max_float);

// Select a new threading from the library
void randomize_threading(Loom *loom);

// Toggle one bit in the tie-up
void mutate_tieup(Loom *loom);

// Evolve the treadling (rotate, replace, reverse)
void evolve_treadling(Loom *loom);
```

---

## Randomness and Pacing

All evolution timers use a range, not a fixed interval. When an event fires, the next occurrence is scheduled at `current_pick + random_in_range(min, max)`. This prevents mechanical periodicity.

**Pacing hierarchy (slow to fast):**

| Event                   | Interval (picks) | Character                                         |
| ----------------------- | ---------------- | ------------------------------------------------- |
| Threading change        | 200–600          | Rare, dramatic — new warp pattern                 |
| Warp color sett change  | 200–600          | Aligned with threading — new color stripe pattern |
| Treadling replacement   | 100–300          | Moderate — shifts rhythm and symmetry             |
| Treadling rotation      | 50–150           | Frequent — subtle diagonal drift                  |
| Tie-up mutation (1 bit) | 30–90            | Continuous — gradual texture evolution            |
| Weft color step         | 20–80            | Frequent — horizontal color banding               |

This hierarchy ensures the viewer sees change at multiple timescales simultaneously — slow structural shifts with faster textural and color variations layered on top.

---

## Future Possibilities (Not V1)

These are ideas noted for later, not part of the initial build:

- **8-shaft mode** — deeper pattern space, more complex structures
- **Overshot simulation** — alternating tabby _(plain weave: over one, under one)_ and pattern picks with two weft colors
- **Sound** — map treadling rhythm to audio pulses; each shaft could have a pitch
- **WIF export** — save the generated draft as a standard WIF _(Weaving Information File)_ file that could be woven on a real loom
- **Multi-panel** — several looms running side by side with different parameters
- **Network input** — mutations driven by external data (weather, music, chat)
- **Larger grid** — 128x128 or 256x256 for finer detail
- **History scrub** — scroll back through the generated fabric to see what came before

---

## Build and Run

```sh
# Dependencies
brew install sdl2

# Build
make

# Run
./weave
```

---

## Summary

Weave is a meditation on pattern, constraint, and emergence. By faithfully simulating the mechanics of a 4-shaft loom — and then letting its three independent components drift — it produces an endlessly unfolding textile that is always structured, always surprising, and always recognizably _woven_.
