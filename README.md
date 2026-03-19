# Weave

A generative loom simulator that produces endlessly evolving woven fabric. Three components of a weaving draft — threading, tie-up, and treadling — independently mutate over time, creating a continuously scrolling 64x64 textile that never repeats.

Built with C99 and SDL2. Zero heap allocation.

## How It Works

The program simulates a 4-shaft floor loom with 64 warp ends. Each tick produces one new row of fabric using the standard drawdown formula:

```
pixel = Tieup[ Treadling[pick] ][ Threading[column] ]
```

- **Threading** maps each of the 64 columns to one of 4 shafts
- **Treadling** maps each row (pick) to one of 4 treadles (foot pedals)
- **Tie-up** is a 4x4 matrix connecting treadles to shafts

The result is binary per cell — warp thread on top or weft thread on top — then colored by a warp color sett and weft color to produce the final pixel.

### Evolution

The loom's three draft components mutate independently at different rates:

| Event | Interval (picks) | Effect |
|---|---|---|
| Weft color step | 20–80 | Horizontal color banding |
| Tie-up mutation | 30–90 | Single bit flip; gradual texture drift |
| Treadling rotation | 50–150 | Diagonal shift of the pattern |
| Treadling replacement | 100–300 | New pedal sequence or reversal |
| Threading change | 200–600 | Dramatic new warp pattern + color sett |

Mutations are validated against real weaving constraints: no impossible sheds (all-up/all-down), no duplicate treadle rows, and a maximum float length of 7.

### Pattern Libraries

12 classic threadings (straight draw, point twill, rosepath, overshot, etc.), 9 treadling sequences, 6 starting tie-ups (2/2 twill, tabby, point twill, 1/3 twill, 3/1 twill, summer/winter), and 10 color palettes are built in as static data.

7 warp sett types — alternating, tartan, gradient, herringbone, windowpane, district check, and solid — use randomized color indices so the same sett shape reads differently each time it appears.

## Build

Requires SDL2.

```sh
# Install SDL2
# Arch: pacman -S sdl2
# macOS: brew install sdl2
# Debian/Ubuntu: apt install libsdl2-dev

make
./weave
```

## Controls

| Key | Action |
|---|---|
| Space | Pause / resume |
| Up / Down | Speed up / slow down |
| T | Force threading change |
| U | Force tie-up mutation |
| R | Force treadling change |
| C | Cycle color palette |
| D | Toggle debug overlay |
| Esc / Q | Quit |

The debug overlay shows traditional draft notation in the margins: threading across the top, tie-up in the top-left corner, and treadling down the left side.

## Project Structure

```
main.c    SDL2 window, rendering, input handling, main loop
weave.h   All data structures, constants, and function declarations
weave.c   Loom core: pattern libraries, RNG, initialization,
          drawdown computation, evolution logic
Makefile  Single-target build
```

## Architecture

The `Loom` struct holds the complete simulation state: threading, tie-up, treadling, color sett, palette, evolution timers, and a ring buffer of 64 rendered rows. The main loop calls `advance_loom()` to compute one new row, then `check_evolutions()` to fire any pending mutations. Rendering writes into an RGB24 pixel buffer uploaded as an SDL streaming texture with nearest-neighbor scaling (8x).
