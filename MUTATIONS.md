# Mutations: The Human Touch

Real cloth is never perfect. A handweaver's body is part of the loom — their
rhythm, fatigue, attention, and mood all leave traces in the fabric. These
traces are not defects. They are what separate a living textile from a
printout. The Navajo leave a spirit line so the weaver's soul isn't trapped
in perfection. Persian rug makers introduce deliberate flaws because only God
creates without error. The Japanese concept of wabi-sabi finds beauty in
irregularity and impermanence. In every tradition, the mark of the hand is
what gives cloth its soul.

This document explores ways to bring that humanity into the digital loom.

---

## 1. Mis-threads

A real weaver threads hundreds of heddles by hand before they even start
weaving. Inevitably, a thread ends up on the wrong shaft. Sometimes you
notice immediately. Sometimes you don't see it until twenty rows in, and by
then you either live with it or unweave.

**Implementation.** When a new threading is applied, roll a small chance
(1-3%) per warp end to assign the wrong shaft. Prefer errors that are
off-by-one (shaft 2 instead of shaft 3) since that's how fingers slip in
real life. The error persists until the next threading change — you don't
fix a mis-thread mid-weaving, you live with it.

**Visual effect.** A single column behaves differently from its neighbors.
In a twill, one thread floats where it should sink. The eye catches it
and then forgives it.

---

## 2. Treadling Mistakes

Even an experienced weaver occasionally presses the wrong pedal. It happens
more when tired, more toward the end of a long session, more on complex
sequences. A treadling mistake produces one wrong row — one pick where the
entire shed is wrong.

**Implementation.** On each pick, roll a small chance of substituting a
random treadle for the correct one. The probability could start very low
(0.5%) and increase slightly over time, resetting when the treadling
sequence changes — simulating the way familiarity with a new sequence
starts careful and gradually gets sloppy.

**Variant: sticky pedal.** Instead of a random wrong treadle, repeat the
previous pick's treadle. This mimics the common mistake of not fully
releasing one pedal before pressing the next. It produces a doubled row
that reads as a hesitation in the cloth.

---

## 3. Beaten Unevenly

On a real floor loom, each row of weft is packed into the fell of the cloth
by swinging the beater. The force varies. You beat harder when you're
focused, lighter when you're distracted, unevenly when you're reaching for
your coffee. This produces subtle row-height variation — some picks are
slightly tighter, some slightly looser.

**Implementation.** Vary the visual height of each row by +/- 1 pixel
occasionally, or more subtly, vary the brightness of a row slightly to
simulate tighter/looser packing. A tightly-beaten row shows less weft
(brighter warp color dominance), a loosely-beaten row shows more weft.

TODO: it would be brighter or darker threads, that makes the most sense, though it won't
display well on a low resolution display scenario so it would need to be optional

Could model this as a slow sine wave with noise: `beat_strength = 1.0 +
0.05 * sin(pick * 0.07) + noise * 0.03`. The sine gives a breathing
rhythm. The noise gives human jitter.

---

## 4. Broken Warp Threads

Warp threads break. It's not if, it's when. You stop, tie a new thread
onto the broken end, weight it with a pin at the back of the loom, and
keep going. The repair is visible — a slight irregularity in tension, a
tiny knot that sits differently in the cloth.

**Implementation.** Rare event (much rarer than other mutations). When
triggered, choose a random warp end. For the next N rows (10-30), that
column renders slightly differently — perhaps always showing weft-dominant
(the broken thread sags and doesn't lift cleanly) or displaying a
different brightness. After N rows, the "repair" settles in and the thread
behaves normally again but perhaps on a different shaft than before (you
re-threaded it slightly wrong under pressure).

**Visual effect.** A vertical scar that heals. Brief but visible. Tells a
story.

---

## 6. Shuttle Run-Out

A real shuttle carries a finite amount of weft yarn on its bobbin. When it
runs out mid-row, the weaver has to splice in a new length. The join is
visible — a slight color shift, a thickening where two tails overlap, or a
gap where the old weft ended and the new one began.

**Implementation.** Every M picks (randomized around 40-80, simulating
bobbin capacity), introduce a splice event. For that single row, the weft
color shifts subtly — perhaps interpolating 10% toward a neighboring
palette color, or darkening/lightening slightly. The next row returns to
normal.

**Variant: wrong bobbin.** Occasionally, when "reloading" the shuttle, use
a weft color from the wrong position in the sett. One row of unexpected
color. In real weaving, this happens when you grab the wrong pre-wound
bobbin from your pile. It's always obvious and always too late.

---

## 7. Counting Errors

Complex treadling sequences require counting. The weaver mentally tracks
"1, 2, 3, 4, 3, 2, 1, 2, 3..." and inevitably loses count. The most
common error is repeating a subsequence or skipping a step, not producing
a completely random pick.

**Implementation.** Instead of substituting a random treadle (as in #2),
model counting errors structurally:

- **Lost count:** Reset the treadling sequence position to the beginning
  of the current repeat, causing a visible stutter in the pattern.
- **Skipped step:** Advance the sequence position by 2 instead of 1,
  compressing the pattern for one cycle.
- **Reversed direction (on point sequences):** Accidentally reverse the
  traversal direction one step too early or too late, creating an
  asymmetric point.

These are more realistic than random substitution and produce errors that
look like real weaving mistakes — the pattern is recognizably _almost_
right.

---

## 9. The Spirit Line

The Navajo deliberately introduce a single imperfection — a line of
contrasting color that runs from the interior of the rug to the edge,
providing a path for the weaver's creative spirit to escape. Without it,
the spirit is trapped in the perfection of the work.

**Implementation.** On a very long timer (rarer than any current
evolution), introduce a single row where one warp thread changes color to
a contrasting palette entry, creating a thin line that breaks the pattern.
Or: for a single pick, override the drawdown to produce a straight line
of one color cutting across the full width. It should feel intentional but
unexplained — a mark that clearly came from a decision, not from chance.

**The deeper idea.** The spirit line isn't about randomness. It's about
the weaver asserting their presence in the work. Computationally, this is
the hardest to capture because it requires _intention_ rather than
_probability_. One approach: make the spirit line happen at an interval
that relates to the total pattern evolution (e.g., once per full
threading-treadling-palette cycle), so it marks a structural moment. It
becomes the loom's signature.

---

## Design Principles

**Subtlety over spectacle.** These mutations should be hard to spot
individually but collectively give the cloth a feeling of life. If someone
says "I can see the errors," they're too frequent. If someone says "this
feels more alive than before but I can't say why," they're calibrated
correctly.

**Realistic error distributions.** Real mistakes cluster. They're more
likely at the edges (selvedge), at transitions (new threading), and late
in sessions (fatigue). A uniform random distribution of errors doesn't
feel human. Weight the probabilities toward moments of natural
vulnerability.

**Structural not cosmetic.** Where possible, mutations should operate on
the loom's structural components (shaft assignments, treadle selection,
sequence position) rather than on pixels. An error in the drawdown
propagates through the weave structure and looks like a real weaving
mistake. A randomly darkened pixel looks like a dead LCD.

**Rare and persistent.** Most of these should be uncommon events whose
effects last for multiple rows or permanently. A mis-threaded heddle
affects every row until the next threading change. A broken thread creates
a visible scar across 20 rows. Brief, frequent glitches feel digital.
Slow, lasting imperfections feel handmade.

**Cumulative character.** Over a long viewing session, the cloth should
accumulate a history of small accidents and corrections. No two minutes
of output should feel identical — not because the pattern evolved (it
already does that) but because the _quality of execution_ shifts like a
human body shifts through a long day at the loom.
