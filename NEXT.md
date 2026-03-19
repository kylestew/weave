# Weave — Next Steps: Color & Visual Variety

## The core problem

Every cell in the drawdown is binary: warp on top or weft on top. That means a single row can show at most **the weft color + however many distinct warp sett colors are in that row's span**. Right now:

- The **palette never changes automatically** — only via the `C` key. One palette per run.
- The **weft is always a single color**. Real weaving often alternates shuttle colors.

## Changes, ranked by impact

### Tier 1 — Fixes that immediately break the monochrome feel

**2. Automatic palette evolution.** Add a `next_palette_change` timer (every 300–800 picks). When it fires, switch to a new random palette. This is the single biggest variety lever — it completely changes the mood without touching structure.

**4. Weight sett types away from solid.** Make solid ~10% probability instead of 25%. Multi-color setts should be the norm, not the exception.

### Tier 2 — New color mechanics

**5. Weft color sett (multi-shuttle).** Instead of a single `weft_color_index`, add a `weft_sett` — a repeating sequence of 2–3 weft colors that cycles per pick. `{0, 2}` gives every-other-row striping. `{1, 1, 3}` gives thick/thin bands. Combined with a warp sett, you get plaid/tartan effects with all 4 palette colors visible simultaneously.

**7. Palette crossfade.** On palette change, lerp each color channel from old→new over ~30 picks. Creates a beautiful gradient transition zone in the fabric rather than a hard line.

### Tier 3 — Richer sett patterns

**8. Asymmetric / tartan setts.** Real tartans have uneven band widths. Generate setts like `{0,0,0,0,1,1,0,0,0,0,2,3,2}` with randomized band widths (2–6 threads per band). Much more organic than the current equal-width patterns.

### Tier 4 — Structural additions

**11. Palette hue drift.** Slowly rotate all palette colors' hue by ~1 degree every 10 picks. Over 3600 picks the entire palette cycles through the color wheel. Creates an ambient, never-repeating quality. (Was in PLAN.md step 11 but skipped.)

**12. Coordinated phase transitions.** Occasionally fire 2–3 evolution events together (threading + palette + treadling). Creates dramatic "the whole thing just changed" moments instead of only incremental drift.

## Suggested implementation order

Do 2 and 4 first — they're small changes to existing code. Then 5 for the biggest experiential leap (multi-shuttle weaving). Then 7–12 as taste dictates.
