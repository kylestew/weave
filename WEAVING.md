# Weaving Draft Formats and Loom Programming

Existing formal notations for describing woven structures — from shaft looms to Jacquard — and how they relate to Bitshift.

---

## WIF (Weaving Information File)

The de facto standard for exchanging weaving drafts. INI-style text format, version 1.1 (April 1997, MHSoft). Universally supported by weaving software (Fiberworks, WeavePoint, PixeLoom, Handweaving.net, AdaCAD, pyweaving).

### Structure

WIF encodes the four-quadrant draft notation:

| Section | Draft Quadrant | Purpose |
|---------|---------------|---------|
| `[THREADING]` | Top | Which shaft each warp thread is assigned to: `thread=shaft` |
| `[TIEUP]` | Top-right | Which shafts each treadle activates: `treadle=shaft,shaft,...` |
| `[TREADLING]` | Left | Which treadle to press for each pick: `pick=treadle` |
| `[LIFTPLAN]` | (alternative) | Direct shaft lift per pick, replaces TIEUP+TREADLING |

The drawdown (center quadrant — the actual fabric bitmap) is computed, not stored:

```
Drawdown[y][x] = Tieup[Treadling[y]][Threading[x]]
```

Boolean matrix multiply: AND for multiply, OR for addition.

### Example: 2/2 Twill

```ini
[WIF]
Version=1.1
Source Program=bitshift

[CONTENTS]
WEAVING=true
THREADING=true
TIEUP=true
TREADLING=true

[WEAVING]
Rising Shed=true
Shafts=4
Treadles=4

[WARP]
Threads=8

[WEFT]
Threads=8

[THREADING]
1=1
2=2
3=3
4=4
5=1
6=2
7=3
8=4

[TIEUP]
1=1,2
2=2,3
3=3,4
4=4,1

[TREADLING]
1=1
2=2
3=3
4=4
5=1
6=2
7=3
8=4
```

### Color Sections

WIF also supports color assignment — the "sett" in tartan terms:

```ini
[COLOR PALETTE]
Form=RGB
Range=0,255

[COLOR TABLE]
1=0,0,0
2=180,30,30
3=40,80,40

[WARP COLORS]
1=1
2=2
3=2
4=1

[WEFT COLORS]
1=1
2=2
3=2
4=1
```

The color-and-weave interaction: at each pixel, the drawdown selects whether the warp color or weft color is visible.

---

## DTX (Fiberworks)

More compact alternative. Sections use `@@` prefix, metadata uses `%%`. Threading and treadling are space-separated lists. Tie-up is a binary matrix (rows = treadles, columns = shafts).

```
@@StartDTX
%%version 1

@@Info
%%shafts 4
%%treadles 4
%%ends 8
%%picks 8

@@Threading
1  2  3  4  1  2  3  4

@@Tieup
1100
0110
0011
1001

@@Treadling
1  2  3  4  1  2  3  4

@@EndDTX
```

---

## Jacquard: No Language, Just Bitmaps

Jacquard looms have no draft factorization — each thread is controlled independently per pick. The "program" is the drawdown itself.

**Punch cards (1804)**: One card per weft pick. One position per warp thread. Hole = warp raised, no hole = warp down. Cards laced in a chain that loops. A complex pattern can require thousands of cards. The card *is* a bitmap row.

**Modern digital Jacquard (TC2)**: "A pixel is a thread." Designs are standard image files (PNG, BMP) where each pixel maps to a thread crossing. The loom reads the image row by row. No abstraction layer between the image and the fabric — the image literally is the weave instruction.

A Jacquard loom is a 1-bit-per-pixel display. PXL is a programmable Jacquard with a computed card chain.

---

## Other Formats and Tools

**AdaCAD** (adacad.org): Open-source parametric design tool. Node-graph approach — operations (flip, shift, invert, mask) connect to generate drafts algorithmically. JSON-based `.ada` format. Imports/exports WIF.

**pyweaving**: Python library for reading/writing WIF, rendering drafts (PIL/SVG/PDF), computing yarn usage, generating basic structures.

**DrawBoy**: CLI tool that reads WIF/DTX and sends picks to AVL Compu-Dobby looms over serial/ethernet.

**JacqCAD Master**: Professional Jacquard software. Supports Bonas `.EP`, AVL `.J1P`, Staubli, and Nedgraphics formats. Designs up to 16,000 ends by 32,000 picks.

**PENELOPE / TidalCycles**: Research project using TidalCycles (Haskell live-coding language for music) to drive a TC-1 loom. Weave structures described from a single thread's behavioral perspective. Experimental.

---

## Mapping to Bitshift

### The Factored Representation

WIF's three-component factorization (threading, tie-up, treadling) is compression by construction — the same principle as PXL expressing a 128x128 image as a short expression. But the factorization is more structured than a flat PXL expression: it separates the x-axis rule from the y-axis rule from the interaction rule.

| Draft Component | What It Does | PXL Equivalent |
|----------------|-------------|----------------|
| Threading | `shaft = x % N_shafts` | `x N %` |
| Treadling | `treadle = y % N_treadles` | `y N %` |
| Tie-up | `raised = tieup[treadle][shaft]` | Threshold or lookup |
| Color sett | `color = sett[x % period]` | Palette index |

For a straight draw (threading = `x % N`, treadling = `y % N`), the drawdown reduces to a pure function of `(x + y) % N` — which is already how PXL expresses twills.

### What PXL Handles

Basic binary structures are one-liners:

```
Plain weave:    x y + 2 %
2/2 twill:      x y + 4 % 2 <
3/1 twill:      x y + 4 % 3 <
Basket weave:   x 2 / y 2 / + 2 %
5-end satin:    x 3 y * + 5 % 0 ==
```

### Where PXL Gets Strained

- **Herringbone** requires threading reversal (macros + IF/ELSE — works but verbose)
- **Color-and-weave** (tartans) needs conditional color selection from a sett sequence — burns instructions on the lookup table
- **Arbitrary tie-ups** that don't reduce to a simple threshold need explicit encoding
- **Non-straight threading** (point draw, broken draw) adds more conditional logic

### What the Layer System Adds

- **Overshot** (ground + pattern): two layers — base tabby, masked pattern float layer
- **Color**: `lingrad` or `fill` layers provide RGB; PXL selects via mask
- **Temporal mutation**: channels drive parameter drift; `shift` renderer + frame persistence enable accumulation
- **Post-processing**: `dither`, `mirror`, `adjust` as POST layers

### Implementation Options

**1. A `weave` renderer** — threading pattern, tie-up matrix, treadling sequence, and optional color sett as `.bs` parameters. The renderer does the boolean matrix multiply. Could import `.wif` files directly.

**2. PXL macro library** — prebuilt macros for common structures. Threading and treadling are already `x N %` / `y N %`; macros encode the tie-up lookup.

**3. Separate notation** — a `.weave` or `[weave]` section in `.bs` files with WIF-like syntax. Factored form is more expressive for exploring the design space (swap tie-ups independently), but adds parsing infrastructure.

---

## Loom Complexity Hierarchy

| Loom | Shafts | Possible Sheds | Computational Equivalent |
|------|--------|----------------|--------------------------|
| Rigid heddle | 2 | 2 | `x y + 2 %` (plain weave only) |
| 4-shaft treadle | 4 | 14 | Modular expressions mod 4 |
| 8-shaft treadle | 8 | 254 | Modular expressions mod 8 |
| 24-shaft dobby | 24 | ~16.7 million | Arbitrary modular expressions |
| Jacquard | per-thread | 2^(thread count) | Arbitrary per-pixel expressions (PXL) |

A Jacquard loom controls each warp thread independently — it is a 1-bit-per-pixel display driven by punch cards. PXL is a Jacquard with a programmable card chain.

---

## References

- WIF 1.1 specification (April 1997, MHSoft)
- Fiberworks PCW DTX format
- AdaCAD: https://adacad.org (UnstableDesign, GitHub)
- pyweaving: https://github.com/storborg/pyweaving
- DrawBoy: https://github.com/MtnViewJohn/DrawBoy
- Shelley Rasmussen, "Factoring Binary Matrices: A Weaver's Approach"
- PENELOPE project — TidalCycles driving TC-1 looms (Alex McLean)
- Handweaving.net Draft Library
