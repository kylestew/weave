# Full Design: How a Blanket Gets Made

How a weaver designs a complete piece, how each element in this app maps to
that process, and what's missing.

---

## The Weaver's Workflow

A blanket doesn't start on the loom. It starts with a sequence of decisions,
each constraining the next. This is roughly the order a weaver works through
them, with notes on how our app currently models (or doesn't model) each step.

### 1. Purpose and Fiber

**What the weaver does.** Before anything else: what is this object? A wool
blanket for warmth wants dense twill and fulling. A cotton throw wants plain
weave and absorbency. A silk scarf wants open sett and drape. The fiber choice
constrains sett, structure, finishing, and color behavior. Wool felts and
blooms. Cotton is crisp and stable. Linen softens with washing. These are
material facts that change every downstream decision.

**What we model.** Nothing. The app is fiber-agnostic — it produces pure
structure and color. This is appropriate for a generative art piece, but it
means the app has no concept of cloth *properties* (drape, warmth, weight,
hand). A design tool would need at least a fiber/yarn selector that sets
defaults for sett and finishing behavior.

### 2. Weave Structure (Tie-Up)

**What the weaver does.** Choose a basic cloth structure: plain weave for
durability, twill for drape and diagonal texture, satin for sheen. This is
the foundational decision that determines how warp and weft interlace. On a
4-shaft loom, the tie-up encodes this choice in a 4×4 matrix.

**What we model.** Six tie-up presets (2/2 twill, tabby, point twill, 1/3
twill, 3/1 twill, summer/winter) plus single-bit mutation with validation.
The validation is real — no empty sheds, no all-raised sheds, no duplicate
treadles, no floats over 7. This is one of the strongest parts of the app.
The tie-up is the heart of cloth structure and we treat it as such.

### 3. Threading

**What the weaver does.** Design the threading draft: which shaft each warp
thread is assigned to, repeating across the full width. Threading determines
the *potential* patterns — the tie-up and treadling select *which* patterns
actually appear. A point twill threading can produce diamonds, zigzags, or
straight diagonals depending on how it's treadled.

Critically, the weaver plans **how many repeats** fit across the desired
width. A 12-end rosepath repeat at 20 EPI needs careful math to fill a
48-inch blanket evenly (80 repeats). The last repeat often needs a
**balancing point** — extra threads that complete the symmetry so the pattern
doesn't end mid-motif at the selvedge.

**What we model.** 22 threading patterns drawn from real weaving traditions —
an excellent library spanning simple (straight draw) to complex (overshot,
cat's paw, snail trail). The threading repeats across all 64 warp ends
automatically.

**What's missing.** No concept of balance points. The threading simply wraps
at the array boundary, which can cut a symmetric pattern asymmetrically. A
real draft would calculate the balance and add partial repeats at the edges.
This matters less at 64 ends (small scale) but would matter if the loom
scaled to 256+ ends.

### 4. Treadling

**What the weaver does.** Plan the sequence of treadle presses that builds
the cloth row by row. The treadling determines the vertical rhythm of the
pattern. Straight treadling produces diagonals; point treadling produces
chevrons; tromp-as-writ copies the threading for perfect symmetry.

For a blanket, the treadling sequence defines the **length** of the pattern
repeat and how many repeats fit in the finished piece. A 6-pick point
treadling at 20 PPI creates 0.3-inch chevrons — too small for a bold
blanket but nice for fine cloth. An 8-pick rose treadling gives larger
motifs.

**What we model.** 9 treadling sequences plus tromp-as-writ mode, rotation
(phase shifting), and direction reversal. This is a strong set. The
sequences cover the essential vocabulary of 4-shaft treadling.

### 5. Sett (Thread Density)

**What the weaver does.** Calculate ends per inch (EPI) based on yarn
diameter and structure. The method:

1. Wrap yarn around a ruler → wraps per inch (WPI)
2. Apply a structure factor: plain weave = WPI × 0.50, twill = WPI × 0.67
3. Adjust for desired hand: tighter for firm cloth, looser for drape

Sett determines whether cloth is sleazy (too open) or boardy (too dense).
Getting it wrong ruins the piece. This is why sampling exists.

**What we model.** The concept of sett doesn't apply directly — our "sett"
(`ColorSett`) refers to the color stripe pattern across warp and weft, not
thread density. The physical density is fixed at one pixel per thread. This
is the correct abstraction for a screen-based piece, but it means the app
has no analog for the physical consequences of sett choice.

### 6. Color Planning

**What the weaver does.** This is where weaving gets complex. Color in
weaving is fundamentally optical mixing — where warp and weft cross, the
eye blends the two thread colors. The weave structure controls the ratio:
plain weave shows 50/50 warp/weft, a warp-dominant twill shows mostly warp.

Key color concepts:

- **Value contrast** matters more than hue. Light/dark differences create
  visible patterns; equal-value different-hue threads create subtle shimmer.
- **Color-and-weave effects** exploit the interaction between color order
  and structure. Log cabin, houndstooth, shepherd's check — these patterns
  emerge from specific color sequences in specific structures, not from
  either alone.
- **Color gamps** are sampling tools: stripe the warp with all planned
  colors, cross with the same stripes in weft, and every possible
  combination appears in the resulting matrix.

**What we model.** This is arguably the app's richest area:

- 10 curated palettes of 4 colors each
- 16 warp sett types (solid through complex plaid patterns)
- 3 weft sett modes (mirror/independent/cycling solid)
- Smooth palette crossfading with per-row RGB interpolation

The sett type library directly encodes color-and-weave traditions: tartan,
log cabin, houndstooth, shepherd's check, glen check, gingham, windowpane,
pin stripe, and more. When weft mirrors warp, true plaid emerges. This is
well-designed.

**What's missing.** The app assigns palette indices randomly when generating
a sett. A real weaver chooses colors with intention — this stripe is the
accent, this is the ground. Value relationships (light/dark contrast) drive
visibility of the pattern, but the app doesn't reason about value. A palette
could randomly assign two same-value colors to a houndstooth pattern,
making the check invisible.

### 7. Borders, Selvedges, and Framing

**What the weaver does.** A blanket is not just a field of repeating
pattern. It has spatial structure:

- **Selvedges** (5-10 threads on each side): plain weave edges for
  stability, regardless of body structure. Essential for structures with
  long floats.
- **Borders** (10-15% of width): often a different threading or color
  than the body. A twill blanket might have a plain-weave border in a
  contrasting color. Borders frame the piece and signal intentionality.
- **Headers and footers**: a half-inch of plain weave at the beginning
  and end, stabilizing the fabric before and after the pattern area.
  Provides a foundation for hemstitching.
- **Fringe or hems**: unwoven warp ends finished as twisted fringe,
  knotted fringe, or turned and stitched hems.

**What we model.** None of this. The 64×64 grid is an undifferentiated
field — every cell is body pattern, edge to edge. There are no selvedges,
no borders, no headers, no fringe. The pattern extends uniformly to all
boundaries.

This is perhaps the most significant gap between the app and a real woven
piece. Borders and selvedges are what make a cloth look like a *designed
object* rather than a swatch of repeating pattern. They're the difference
between wallpaper and a painting.

### 8. Dimensional Planning

**What the weaver does.** Calculate the total warp needed:

```
warp_length = finished_length
            + weft_shrinkage (5-20% depending on fiber)
            + take-up (warp consumes length going over/under weft)
            + loom_waste (20-30 inches tied to front/back beams)
            + fringe_allowance (if applicable)

total_ends  = (finished_width + draw-in) × EPI
            + selvedge_ends
```

These calculations prevent the heartbreak of running out of warp 6 inches
before the end of a blanket.

**What we model.** Not applicable — the piece is an infinite scroll with
no target dimensions. This is a feature, not a gap. The generative nature
of the app means it never needs to calculate materials. But a design tool
for real weaving would need a full yarn calculator.

### 9. Sampling

**What the weaver does.** Before committing to a full blanket (~8-12 hours
of weaving, $50-200 of yarn), weave a 6-inch sample and wet-finish it.
The sample reveals:

- Actual shrinkage percentages for this fiber/sett/structure combination
- How colors interact in the real cloth (screen colors lie)
- Whether the hand (feel) is right
- Whether the sett is too open or too dense
- How the cloth responds to finishing (fulling, pressing)

No software can replace this step. The physics of yarn under tension, the
optical properties of dyed fiber, the behavior of wet wool — these are
analog phenomena.

**What we model.** The app *is* a kind of infinite sample — it shows
structure and color interaction in real time. But it can't predict
material behavior. This is an inherent limitation of any screen-based
weaving tool, not a gap to fill.

### 10. Finishing

**What the weaver does.** After cutting the piece off the loom:

1. **Secure the ends** — hemstitch, knot fringe, or machine-stitch
   before handling
2. **Wet finish** — wash the cloth to relax tension, bloom fibers, and
   settle interlacements. This is not optional; unwashed handweaving is
   not finished cloth.
3. **Full** (wool only) — controlled agitation in warm water causes
   fibers to migrate and compact. Transforms open, crunchy weave into
   soft, dense, warm fabric. The degree is controllable: light fulling
   blooms the surface; heavy fulling approaches felt density.
4. **Press** — steam pressing sets the final surface and adds sheen
5. **Trim fringe** — even up the ends

Shrinkage during finishing is typically 5-10% for cotton, 10-20% for
wool (more with heavy fulling). The cloth looks and feels dramatically
different after finishing. This is expected and planned for.

**What we model.** Nothing, nor should it — finishing is purely physical.
But it's worth noting because it means our drawdown output represents
*on-loom appearance*, not finished cloth. Finished cloth has softer edges
between colors (fiber bloom fills gaps), more texture, and different
proportions.

---

## What the App Has: A Summary

| Design Element           | Status    | Notes                                    |
|--------------------------|-----------|------------------------------------------|
| Weave structure (tie-up) | Strong    | 6 presets, validated mutation             |
| Threading                | Strong    | 22 patterns from real traditions          |
| Treadling                | Strong    | 9 sequences + tromp-as-writ + rotation   |
| Color palettes           | Strong    | 10 palettes, smooth crossfading          |
| Color sett patterns      | Strong    | 16 warp types, 3 weft modes              |
| Drawdown formula         | Correct   | Faithful: tie-up[treadling[y]][threading[x]] |
| Float validation         | Correct   | Max 7, real loom constraint              |
| Tie-up validation        | Correct   | No impossible sheds, no duplicates       |
| Human imperfection       | Designed  | Documented in MUTATIONS.md, not yet built|

## What's Missing for a Complete Design System

These are elements that real weavers use to create cohesive finished pieces,
roughly ordered by how much they'd add to the app's output:

### Critical for "finished piece" appearance

1. **Borders and selvedges.** The single biggest gap. A few columns of
   plain weave at each edge, a contrasting border zone, and a header/footer
   area would transform the output from "infinite pattern swatch" to
   "designed textile." Even reserving 4-6 of the 64 columns on each side
   for a border treatment would dramatically change the read.

2. **Balance points in threading.** When a symmetric threading repeat
   doesn't divide evenly into the warp width, the edges look arbitrary.
   Adding a partial repeat to complete the symmetry is standard practice
   and would make the pattern look intentional at the boundaries.

3. **Value-aware color assignment.** When generating sett color assignments,
   consider the light/dark contrast between adjacent stripes. Houndstooth
   needs high contrast to read; tartan needs both bold bands and subtle
   accent lines. Currently, random palette index assignment can produce
   invisible patterns.

### Important for realism and richness

4. **Human imperfections (MUTATIONS.md).** Already designed in detail but
   not implemented. Mis-threads, treadling mistakes, broken warps, and
   spirit lines would add the "handmade" quality that separates compelling
   generative textile art from screensaver-level pattern generation.

5. **Thread-level rendering.** Currently the #1 priority in IDEAS.md. Round
   thread profiles with shading at crossings would be the visual equivalent
   of upgrading from 8-bit graphics to a painting. At 8× scale there's
   room for sub-cell detail — darken edges where a thread passes under,
   brighten the crown of a thread that passes over.

6. **Overshot / supplementary weft.** Overshot is one of the most visually
   rich structures on a 4-shaft loom. It uses two shuttles: a thin tabby
   ground shuttle and a thick pattern shuttle. Every other pick is tabby,
   every other is pattern. The result: bold, textured motifs floating on a
   stable ground cloth. The app has an overshot threading pattern but
   doesn't model the two-shuttle technique. Real overshot would require
   alternating tabby picks between pattern picks.

### Nice-to-have for a full design tool

7. **Yarn/fiber properties.** Even minimal modeling — fiber type sets
   recommended sett range, expected shrinkage, and finishing behavior —
   would make the app useful for planning real pieces.

8. **Dimensional calculator.** Warp length, total ends, yarn quantity.
   Essential for anyone who wants to translate a generated design to a
   real loom.

9. **Draft export.** Output in WIF (Weaving Information File) format, the
   standard interchange format for weaving software. This would let
   weavers import generated designs into tools like Fiberworks, WeaveIt,
   or Handweaving.net's draft editor and actually weave them.

10. **Multi-shuttle support.** Beyond overshot, many traditional structures
    (crackle, summer-and-winter as traditionally woven) use multiple weft
    colors alternating with a tabby binder. The app's single-shuttle model
    can't produce these as traditionally woven.

---

## How the Elements Compose: Building a Blanket

If a weaver sat down to design a wool blanket using only the elements
currently in this app, here's how they'd assemble:

```
1. PALETTE        → Choose "Indigo & Natural" (winter blanket feel)
2. STRUCTURE      → Select 2/2 twill tie-up (warm, drapey, classic)
3. THREADING      → Point twill (chevron/diamond motifs)
4. TREADLING      → Point treadling (diamonds) or tromp-as-writ (symmetry)
5. WARP SETT      → Tartan type (symmetric bands of color)
6. WEFT SETT      → Mirror mode (true plaid — warp and weft match)
```

This produces a classic tartan blanket: diagonal twill texture, diamond
motifs from the point threading, symmetric color bands creating a plaid
grid. It's a complete, coherent design — one of the most traditional
textile forms in the world.

But to make it a *finished* blanket rather than an infinite swatch:

```
7. SELVEDGES      → 6 ends plain weave each side          [NOT IN APP]
8. BORDERS        → 12 ends contrast color each side      [NOT IN APP]
9. HEADER/FOOTER  → 10 picks plain weave at start/end     [NOT IN APP]
10. FRINGE        → 6 inches unwoven warp, twisted         [NOT IN APP]
11. BALANCE       → Partial repeat to complete symmetry    [NOT IN APP]
12. SAMPLING      → Test swatch to verify sett and color   [NOT IN APP]
13. FINISHING     → Wet finish and light fulling           [NOT IN APP]
```

Steps 7-11 are structural elements that could be implemented in the app.
Steps 12-13 are physical processes that belong in the real world.

---

## The Order of Operations

When the app generates a new design (or when a viewer watches it evolve),
the elements activate in this hierarchy, from most stable to most fluid:

```
MOST STABLE (changes rarely)
│
├── Threading    — defines the warp's structural DNA
│                  changes every 200-600 picks
│                  a new threading is like re-warping the loom
│
├── Treadling    — defines the vertical rhythm
│                  changes every 100-300 picks
│                  like the weaver deciding to switch patterns
│
├── Tie-up       — defines the cloth structure
│                  mutates every 30-90 picks (single bit)
│                  gradual structural drift, like wearing in
│
├── Palette      — defines the color world
│                  changes every 300-800 picks
│                  crossfades smooth the transition
│
├── Warp Sett    — defines the color stripe pattern
│                  changes with threading (coupled)
│                  like choosing a new color order
│
├── Weft Sett    — defines weft color behavior
│                  changes every 150-400 picks
│                  switches between mirror/independent/solid
│
└── Weft Color   — in solid mode, cycles through palette
                   changes every 20-80 picks
                   the fastest-moving element
│
MOST FLUID (changes often)
```

This hierarchy mirrors real weaving: the warp is set once and doesn't
change (threading is the most "expensive" change). Treadling can change
whenever the weaver decides. Color is the most fluid — you can change
shuttles on any pick.

The app's evolution timers already encode this hierarchy naturally. The
result is designs that feel like they're being woven by someone who
occasionally changes their mind — which is exactly what real weaving
looks like over the course of a long project.

---

## Conclusion

The app faithfully models the three core components of 4-shaft weaving
(threading, tie-up, treadling) and has a surprisingly rich color system
(palettes, sett patterns, crossfading). The drawdown formula is correct.
The validation constraints are real. The pattern libraries draw from
genuine weaving traditions.

The biggest conceptual gaps are spatial, not structural: the absence of
borders, selvedges, and balance points means the output reads as
*infinite pattern* rather than *designed object*. These are the elements
that frame a piece and declare "this was made with intention."

The second gap is textural: flat pixel rendering vs. thread-level shading.
This is already identified as the top priority in IDEAS.md.

The third gap is human: the imperfection system designed in MUTATIONS.md
but not yet implemented. Real cloth carries the trace of the hand that
made it. The app's output is currently too perfect — structurally correct
but lacking the micro-irregularities that make handweaving feel alive.

Everything else — fiber properties, dimensional calculations, finishing
simulation — belongs in a design *tool*, not a generative art piece.
The app is the latter, and it's good at it. The gaps above are about
making its output feel less like a pattern generator and more like cloth.
