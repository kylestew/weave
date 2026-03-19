# Weave Pattern Library

All patterns run on a 4-shaft, 4-treadle floor loom simulation. Shaft and treadle indices are 0-based (0–3).

---

## Threadings (22)

The threading defines which shaft each warp end is tied to. The pattern repeats across all 64 warp ends.

| # | Name | Sequence | Len | Notes |
|---|------|----------|-----|-------|
| 1 | Straight draw | 0,1,2,3 | 4 | Basic sequential threading |
| 2 | Point twill | 0,1,2,3,2,1 | 6 | Ascending then descending — produces chevrons |
| 3 | Rosepath | 0,1,2,3,2,1,0,3 | 8 | Point twill variant with asymmetric turn |
| 4 | Broken twill | 0,1,2,3,1,0,3,2 | 8 | Disrupted diagonal — prevents continuous twill lines |
| 5 | Extended point | 0,1,2,2,3,3,2,2,1,0 | 10 | Point twill with doubled center shafts |
| 6 | Monk's belt | 0,1,0,1,2,3,2,3 | 8 | Two blocks alternating plain weave and pattern |
| 7 | M's and O's | 0,1,0,1,0,1,2,3,2,3,2,3 | 12 | Longer repeat distinguishing it from monk's belt |
| 8 | Overshot | 0,1,0,2,1,2,1,3,0,3,0,2,1,2,1,0 | 16 | Colonial coverlet threading — complex supplementary weft |
| 9 | Huck lace | 0,1,0,2,3,2 | 6 | Lace weave with warp floats creating openwork |
| 10 | Advancing twill | 0,1,2,3,1,2,3,0 | 8 | Twill that shifts start point each repeat |
| 11 | Crepe | 0,2,1,3,2,0,3,1 | 8 | Irregular interlacement for textured surface |
| 12 | Undulating twill | 0,1,2,3,2,3,0,1,0,1,2,3 | 12 | Twill with shifting wave motion |
| 13 | Goose eye | 0,1,2,3,3,2,1,0 | 8 | Point twill with double-hit on the turn — "eye" dots in diamonds |
| 14 | Bird's eye | 0,1,0,3,2,3 | 6 | Broken-twill variant with tiny isolated specks |
| 15 | Summer & winter | 0,2,1,2,0,3,1,3 | 8 | Block weave: shafts 0-1 = binder, 2-3 = pattern. American colonial |
| 16 | Crackle weave | 0,1,2,1,1,2,3,2 | 8 | Scandinavian block weave — threads move one shaft at a time |
| 17 | Dornick twill | 0,1,2,1,0,3,2,3 | 8 | Old Flemish broken twill from Tournai |
| 18 | Canvas / basket | 0,0,1,1,2,2,3,3 | 8 | Paired threads on adjacent shafts — hopsack texture |
| 19 | Cat's paw | 0,1,0,1,1,2,1,2,2,3,2,3,3,0,3,0 | 16 | Colonial coverlet overshot — spiraling 2-thread pairs |
| 20 | Snail trail | 0,1,0,1,0,1,1,2,2,3,2,3,2,3,3,0 | 16 | Companion to cat's paw — traditional American coverlet |
| 21 | Turned twill | 0,1,2,3,0,3,2,1 | 8 | Z-twill and S-twill blocks — checkerboard of twill direction |
| 22 | Zigzag point | 0,0,1,1,2,2,3,3,2,2,1,1 | 12 | Elongated point twill with wider chevrons |

---

## Treadling Sequences (9)

The treadling defines which treadle is pressed for each successive pick (row). Cycles through the sequence, optionally reversed.

| # | Name | Sequence | Len |
|---|------|----------|-----|
| 1 | Straight | 0,1,2,3 | 4 |
| 2 | Reverse | 3,2,1,0 | 4 |
| 3 | Point | 0,1,2,3,2,1 | 6 |
| 4 | Alternating | 0,2,1,3 | 4 |
| 5 | Rose | 0,1,2,3,2,1,0,3 | 8 |
| 6 | Extended | 0,0,1,1,2,2,3,3 | 8 |
| 7 | Syncopated | 0,1,0,2,0,3 | 6 |
| 8 | Doubled | 0,1,2,3,0,1,2,3 | 8 |
| 9 | Offset | 0,2,1,3,2,0,3,1 | 8 |

A special "tromp-as-writ" mode copies the current threading pattern into the treadling for symmetric results.

---

## Tie-ups (6)

The tie-up is a 4x4 matrix connecting treadles (rows) to shafts (columns). 1 = pressing that pedal raises that shaft.

| # | Name | Row 0 | Row 1 | Row 2 | Row 3 | Notes |
|---|------|-------|-------|-------|-------|-------|
| 1 | 2/2 Twill | 1,1,0,0 | 0,1,1,0 | 0,0,1,1 | 1,0,0,1 | Diagonal lines — two up, two down |
| 2 | Tabby | 1,0,1,0 | 0,1,0,1 | 1,0,1,0 | 0,1,0,1 | Plain weave — over one, under one |
| 3 | Point Twill | 1,1,0,0 | 1,1,1,0 | 0,1,1,1 | 0,0,1,1 | Twill that reverses — chevrons/diamonds |
| 4 | 1/3 Twill | 1,0,0,0 | 0,1,0,0 | 0,0,1,0 | 0,0,0,1 | Very open, warp-dominant |
| 5 | 3/1 Twill | 1,1,1,0 | 0,1,1,1 | 1,0,1,1 | 1,1,0,1 | Weft-dominant, heavy coverage |
| 6 | Summer/Winter | 1,1,0,0 | 1,0,1,0 | 1,0,0,1 | 0,1,0,1 | Block weave patterns |

Tie-ups evolve during runtime via single-bit mutations (validated against float limits and structural rules).

---

## Color Palettes (10)

Each palette has 4 colors used for warp and weft threads.

| # | Name | Color 1 | Color 2 | Color 3 | Color 4 |
|---|------|---------|---------|---------|---------|
| 1 | Indigo & Natural | rgb(45,52,94) | rgb(210,195,170) | rgb(80,85,128) | rgb(180,165,140) |
| 2 | Earth | rgb(101,67,33) | rgb(181,137,85) | rgb(139,90,43) | rgb(222,199,163) |
| 3 | High Contrast | rgb(20,20,20) | rgb(235,235,230) | rgb(180,40,30) | rgb(50,50,55) |
| 4 | Ocean | rgb(15,30,60) | rgb(40,80,120) | rgb(100,160,180) | rgb(200,220,225) |
| 5 | Forest | rgb(25,50,25) | rgb(60,100,45) | rgb(140,160,80) | rgb(200,190,140) |
| 6 | Berry & Sage | rgb(140,30,80) | rgb(85,45,90) | rgb(120,150,95) | rgb(230,220,195) |
| 7 | Sunset | rgb(210,95,75) | rgb(200,155,50) | rgb(75,40,100) | rgb(230,200,130) |
| 8 | Slate & Rust | rgb(100,105,110) | rgb(70,75,80) | rgb(185,95,45) | rgb(160,80,35) |
| 9 | Nordic | rgb(20,30,70) | rgb(190,35,40) | rgb(235,235,235) | rgb(60,90,150) |
| 10 | Jewel | rgb(15,110,60) | rgb(170,25,40) | rgb(30,55,140) | rgb(200,165,40) |

---

## Warp Sett Types (16)

The warp sett defines the repeating color-stripe pattern across the warp threads. Palette indices are randomized at generation time.

| # | Name | Template | Len | Colors | Notes |
|---|------|----------|-----|--------|-------|
| 0 | Solid | A | 1 | 1 | Single color across all warp ends |
| 1 | Alternating | A,B | 2 | 2 | Simple two-color stripe |
| 2 | Tartan | A,A,B,B,A,A,C,C | 8 | 3 | Symmetric banded stripes |
| 3 | Gradient | shuffled palette | varies | all | All palette colors in random order |
| 4 | Herringbone | A,A,B,B,C,C,B,B | 8 | 3 | Mirrored color bands |
| 5 | Windowpane | A...A,B | 5–7 | 2 | Wide ground with thin accent stripe |
| 6 | District check | A...,B...,C... | 5–11 | 3 | Unequal-width color bands |
| 7 | Log cabin | A,B,A,B,B,A,B,A | 8 | 2 | Block reversal at double-thread boundary |
| 8 | Houndstooth | A,A,A,A,B,B,B,B | 8 | 2 | 4-and-4. Classic jagged "tooth" effect with twill |
| 9 | Shepherd's check | A,A,B,B | 4 | 2 | 2-and-2. Smaller houndstooth / "puppytooth" |
| 10 | Glen check | A,A,B,B,A,A,B,B,A,A,A,A,B,B,B,B | 16 | 2 | Combines shepherd's check + houndstooth sections |
| 11 | Gingham | A,A,A,B,B,B | 6 | 2 | Equal-width stripes |
| 12 | Tattersall | A,A,A,B,A,A,A,C | 8 | 3 | Two thin accent stripes on wide ground |
| 13 | Gun club | A,A,B,B,A,A,C,C | 8 | 3 | Two contrasting darks on light ground |
| 14 | Madras | A,A,B,B,B,C,A,A,D,D,C,B | 12 | 4 | Asymmetric bright bands — no mirror symmetry |
| 15 | Pin stripe | A,A,A,A,A,A,A,B | 8 | 2 | Single contrasting thread on wide ground |
