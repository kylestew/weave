# Library Update: New Threadings & Warp Sett Types

Current state: 12 threadings, 9 treadling sequences, 6 tie-ups, 10 palettes, 7 warp sett types.

---

## New Threadings to Add

10 historically named patterns, all on 4 shafts (0-indexed), all fit within MAX_THREADING_REPEAT=16.

| Name | Sequence | Length | Origin / Notes |
|------|----------|--------|----------------|
| Goose eye | 0,1,2,3,3,2,1,0 | 8 | Point twill with double-hit on the turn — creates a dot/"eye" in each diamond |
| Bird's eye | 0,1,0,3,2,3 | 6 | Broken-twill variant producing tiny isolated specks. Classic toweling pattern |
| Summer & winter | 0,2,1,2,0,3,1,3 | 8 | Block weave. Shafts 0-1 = plain weave binder, 2-3 = pattern blocks A/B. American colonial |
| Crackle weave | 0,1,2,1,1,2,3,2 | 8 | Scandinavian block weave ("jamslandsvaev"). Threads move only one shaft at a time |
| Dornick twill | 0,1,2,1,0,3,2,3 | 8 | Old Flemish broken twill from Tournai. Compact zigzag, very different texture from straight twill |
| Canvas / basket | 0,0,1,1,2,2,3,3 | 8 | Paired threads on adjacent shafts. Creates hopsack/basket texture |
| Cat's paw | 0,1,0,1,1,2,1,2,2,3,2,3,3,0,3,0 | 16 | Colonial coverlet overshot — spiraling 2-thread pairs |
| Snail trail | 0,1,0,1,0,1,1,2,2,3,2,3,2,3,3,0 | 16 | Companion to cat's paw. Traditional American coverlet draft |
| Turned twill | 0,1,2,3,0,3,2,1 | 8 | One block Z-twill, one block S-twill — checkerboard of twill direction |
| Zigzag point | 0,0,1,1,2,2,3,3,2,2,1,1 | 12 | Elongated point twill with wider chevrons than standard point |

**Note on existing duplicates:** Current "monk's belt" and "M's and O's" share the same sequence {0,1,0,1,2,3,2,3}. Traditional M's and O's uses a longer repeat: {0,1,0,1,0,1,2,3,2,3,2,3} (length 12). Worth differentiating.

---

## New Warp Sett Types to Add

9 historically named color arrangements. All fit within MAX_SETT=16. Palette indices should be randomized at generation time (like existing setts).

| Name | Template | Length | Origin / Notes |
|------|----------|--------|----------------|
| Log cabin | A,B,A,B,B,A,B,A | 8 | Block reversal at the double-thread boundary. When weft matches, gives characteristic light/dark squares |
| Houndstooth | A,A,A,A,B,B,B,B | 8 | 4-and-4. The most iconic color-and-weave effect. With 2/2 twill creates the jagged "tooth" shape |
| Shepherd's check | A,A,B,B | 4 | 2-and-2. Smaller version of houndstooth. Also "puppytooth." Traditional Scottish |
| Glen check | A,A,B,B,A,A,B,B,A,A,A,A,B,B,B,B | 16 | Combines shepherd's check (2+2) and houndstooth (4+4) sections. Glen Urquhart |
| Gingham | A,A,A,B,B,B | 6 | Equal-width stripes. Traditional plain-weave check |
| Tattersall | A,A,A,B,A,A,A,C | 8 | Two thin accent stripes on wide ground. Named from Tattersall's horse market, London |
| Gun club | A,A,B,B,A,A,C,C | 8 | Two contrasting dark colors on light ground. Originally "Coigach" from a Scottish estate |
| Madras | A,A,B,B,B,C,A,A,D,D,C,B | 12 | Asymmetric bright bands. Madras is inherently informal — no mirror symmetry, uses all 4 colors |
| Pin stripe | A,A,A,A,A,A,A,B | 8 | Single contrasting thread on wide ground. Narrower than windowpane |

### Implementation notes

- **Log cabin** is the most important addition — it's a whole family of weaving (log cabin weave) and the block-reversal trick is unique among the current setts.
- **Houndstooth** and **shepherd's check** are the two most historically significant color-and-weave effects. They're simple sequences but produce dramatically different results depending on the tie-up.
- **Glen check** uses the full MAX_SETT=16. It's the richest single sett — combines two different check scales in one repeat.
- **Tattersall** and **gun club** both use 3 colors (like the existing tartan/district check).
- **Madras** is the only sett using all 4 palette colors and having no symmetry.
- **Pin stripe** is very close to windowpane but narrower (1-thread accent vs windowpane's variable width). Could arguably fold into windowpane with a width=1 option instead of a separate type.

### Randomization strategy

For 2-color setts (log cabin, houndstooth, shepherd's check, glen check, gingham, pin stripe): pick two distinct random palette indices, same as alternating.

For 3-color setts (tattersall, gun club): pick three distinct random palette indices, same as tartan.

For 4-color setts (madras): pick four distinct random palette indices (full palette shuffle, take first 4).

---

## Sources

- Gist Yarn: 4-Shaft Weaving Patterns, Overshot, Monk's Belt/Crackle/Summer&Winter, Bronson/Swedish Lace, Tartans/Tattersalls/Checks
- Weaving Space: Twills on 4 Shafts, Goose Eye, Undulating Twill, Houndstooth
- Schacht Spindle: Twill Sampler, Summer and Winter, Monk's Belt
- Handwoven Magazine: Twill Glossary, Overshot Threading, Shadow Weave
- Cotton Clouds: Cat Track & Snail Trail, Log Cabin
- Wikipedia: Houndstooth, Glen Plaid, Tattersall
- Permanent Style: Tweed Patterns and District Checks
