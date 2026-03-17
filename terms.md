# Weaving Glossary for Non-Weavers

A glossary of weaving terminology organized from first principles. Start at the top and read down -- each concept builds on the ones before it.

---

## Part 1: The Raw Materials

### Yarn / Thread

**Definition:** A continuous strand of fiber (cotton, wool, silk, synthetic) twisted together so it holds its shape. In weaving, "yarn" and "thread" are often used interchangeably, though "thread" sometimes implies finer gauges.

**Why it matters:** Everything in weaving is built from two sets of yarn crossing each other. The properties of the yarn -- its thickness, fiber content, elasticity, color -- determine what the final fabric looks and feels like.

---

## Part 2: The Two Directions

### Warp

**Definition:** The set of parallel yarns held under tension on the loom, running lengthwise (toward you and away from you). These are set up first, before any weaving begins.

**Why it matters:** The warp is the *skeleton* of the fabric. Every pattern begins with deciding how many warp threads there are and in what order they are arranged. In a weaving draft, the warp runs along the horizontal (x) axis.

**Analogy:** Think of the warp as the strings on a guitar -- fixed in place, under tension, waiting to be played.

**Computational connection:** The warp is the x-axis of the fabric grid. Each warp thread has a fixed position (column index). In binary terms, for any given row of fabric, each warp thread is either visible on top (1) or hidden underneath (0).

### Weft

**Definition:** The yarn that is passed back and forth horizontally across the warp, interlacing with it to create fabric. Also historically called the "woof."

**Why it matters:** The weft builds the fabric row by row. Each pass of weft across the warp creates one new row of cloth. Patterns emerge from *which* warp threads the weft goes over vs. under on each pass.

**Analogy:** If the warp is the guitar strings, the weft is the musician's fingers -- it moves across the strings, choosing which to engage.

**Computational connection:** The weft is the y-axis. Each row of weft is like scanning one row of a bitmap image. The weaver builds the fabric line by line, exactly like a printer prints page by page, or like a raster display draws frame by frame.

### End

**Definition:** A single warp thread. If you have 200 warp threads, you have 200 "ends."

**Why it matters:** This is the standard unit of measurement along the x-axis. Draft notations specify patterns in terms of individual ends. "Ends per inch" (EPI) tells you how densely the warp threads are packed.

**Computational connection:** An end is a column in the fabric matrix. The threading draft assigns each end to a shaft: `Threading[x] = shaft_number`.

### Pick

**Definition:** A single pass of the weft yarn across the warp -- one row of fabric. Also called a "shot."

**Why it matters:** This is the standard unit of measurement along the y-axis. Every row you weave is one pick. "Picks per inch" (PPI) tells you the vertical density of the fabric.

**Computational connection:** A pick is a row in the fabric matrix. The treadling sequence specifies what happens at each pick: `Treadling[y] = treadle_number`. In a Jacquard loom, each pick corresponds to one punch card -- one bitmap row.

---

## Part 3: The Loom (The Machine That Holds Everything)

### Loom

**Definition:** A frame or machine that holds the warp threads under tension so you can interlace the weft through them. Looms range from a simple wooden frame to a computerized industrial machine, but they all do the same fundamental job.

**Why it matters:** The type of loom determines the complexity of patterns you can create. A simple loom can only make plain weave; a complex loom can produce any pattern imaginable.

**Analogy:** A loom is to weaving what a printing press is to printing -- it is the machine that turns the *instructions* (the draft) into the *output* (the fabric).

**Computational connection:** The loom is the *hardware*. Different looms have different computational capacity -- from a rigid heddle loom (essentially a 1-bit processor that can only alternate 0 and 1) to a Jacquard loom (a full bitmap display where every pixel is independently addressable).

### Warp Beam

**Definition:** The roller at the back of the loom that holds the full length of warp yarn wound around it. It feeds warp forward as weaving progresses.

**Why it matters:** This is the yarn reservoir. A long project might have yards of warp wound onto this beam, gradually unwinding as you weave.

### Cloth Beam

**Definition:** The roller at the front of the loom that winds up the finished fabric as it is woven.

**Why it matters:** The cloth beam and warp beam work as a pair -- warp feeds off the back, finished cloth winds onto the front, keeping the working area at a consistent tension.

### Shaft (also called Harness or Frame)

**Definition:** A rectangular frame that holds a set of heddles. The shaft moves up or down as a unit, carrying all its heddles (and all the warp threads threaded through those heddles) with it.

**Why it matters:** Shafts are the primary mechanism for creating patterns. A 4-shaft loom has 4 frames that can be raised or lowered in different combinations. The number of shafts determines the complexity of patterns you can weave: 2 shafts can make plain weave, 4 shafts can make twills and simple patterns, 8+ shafts unlock increasingly complex structures.

**Analogy:** Think of shafts as layers in a drawing program. Each layer holds a subset of the warp threads. By turning layers on and off in different combinations, you create different patterns.

**Computational connection:** With N shafts, you have 2^N - 2 possible shed configurations (excluding all-up and all-down, which are not useful). A 4-shaft loom has 14 distinct sheds; an 8-shaft loom has 254. The shaft system is the loom's *instruction set* -- it defines which operations are possible.

### Heddle

**Definition:** A thin wire or cord with an eye (hole) in its center. Each warp thread passes through the eye of one heddle. The heddle is mounted on a shaft, so when the shaft rises, the heddle pulls its warp thread up with it.

**Why it matters:** The heddle is the connection point between an individual warp thread and the group-control mechanism (the shaft). Threading -- the process of deciding which heddle each warp thread goes through -- is one of the fundamental pattern-defining steps.

**Analogy:** Each heddle is like a tiny handle attached to one string. The shaft is a bar that grabs all the handles at once, lifting a whole group of strings simultaneously.

**Computational connection:** Threading assigns each warp end to a shaft via its heddle. This is the function `Threading[x] = shaft_number`. It is the mapping from individual column (warp thread) to group (shaft). For a straight draw on 4 shafts: `Threading[x] = (x % 4) + 1`, meaning threads cycle through shafts 1, 2, 3, 4, 1, 2, 3, 4...

### Reed

**Definition:** A comb-like frame with many thin, evenly-spaced vertical slits (called "dents"). The warp threads pass through the dents, which space them evenly across the width of the fabric. The reed is mounted in the beater.

**Why it matters:** The reed serves two functions: (1) it spaces the warp threads at the correct density, and (2) when swung forward (as part of the beater), it pushes each new row of weft snugly against the previous rows.

**Analogy:** The reed is like the teeth of a comb, keeping the warp threads organized and evenly spaced, then acting like a squeegee to press each new row into place.

### Dent

**Definition:** One slot (opening) in the reed. You thread one or more warp ends through each dent.

**Why it matters:** The number of dents per inch in the reed, combined with how many threads you put in each dent, determines the sett (thread density).

### Beater

**Definition:** The swinging frame that holds the reed. After each pick of weft is inserted, the weaver swings the beater toward the cloth to pack the new weft row tightly against the previous rows.

**Why it matters:** The beater determines how tightly the weft rows are packed. This is called "beating" -- it is one of the three fundamental motions of weaving (shedding, picking, beating).

### Shuttle

**Definition:** A tool that carries the weft yarn and passes it through the shed (the opening between raised and lowered warp threads). It can be a simple stick with yarn wrapped around it, or a boat-shaped device holding a bobbin.

**Why it matters:** The shuttle is how the weft gets from one side of the loom to the other. In techniques like overshot, you use two shuttles -- one for the structural weft and one for the pattern weft.

### Bobbin

**Definition:** A small spool of weft yarn that sits inside a boat shuttle, allowing the yarn to unwind smoothly as the shuttle passes through the shed.

### Treadle

**Definition:** A foot pedal at the base of the loom. When you press a treadle, it raises (or lowers) one or more shafts, creating a shed.

**Why it matters:** Treadles are the weaver's *input device*. The sequence in which you press treadles determines the pattern. On a 4-shaft loom, you might have 4 to 6 treadles, each wired to a different combination of shafts.

**Analogy:** Treadles are like piano pedals or keyboard shortcuts -- each one triggers a specific pre-configured action (raising a specific combination of shafts).

**Computational connection:** A treadle is an index into the tie-up table. Pressing treadle T looks up `Tieup[T]` to determine which shafts rise. The treadling sequence is the program's main loop: step through picks y = 1, 2, 3, ..., pressing `Treadling[y]` at each step.

### Lamm

**Definition:** A horizontal lever below the shafts that connects treadles to shafts via cords. When you press a treadle, it pulls the lamm, which in turn raises or lowers the shaft.

**Why it matters:** Lamms are the mechanical linkage that translates the weaver's foot action into shaft motion. On a jack loom there is one set of lamms (for raising shafts); on a countermarch loom there are two sets (one for raising, one for lowering).

### Temple (or Stretcher)

**Definition:** A device that holds the woven fabric at its correct width on the loom, preventing the natural draw-in that happens as the weft pulls the edges inward.

---

## Part 4: The Shed (Where the Action Happens)

### Shed

**Definition:** The V-shaped opening created when some warp threads are raised and others are lowered. The shuttle passes through this opening to lay in the weft.

**Why it matters:** The shed is the *core mechanism* of all weaving. Without a shed, you would have to manually lift each warp thread by hand to pass the weft under it. The entire evolution of loom technology -- from stick looms to Jacquard machines -- is about creating the shed faster, more reliably, and with more complex configurations.

**Analogy:** Imagine holding a deck of cards fanned out on a table. Now imagine lifting every other card while the rest stay down, creating a gap you can slide something through. That gap is the shed.

**Computational connection:** Each shed configuration is a binary state across all warp threads: for each thread, either UP (1) or DOWN (0). A shed is a single row of the output bitmap. With N shafts, the number of distinct sheds is 2^N - 2.

### Shedding

**Definition:** The act of creating a shed -- raising some shafts and lowering others to separate the warp threads into two groups.

**Why it matters:** Shedding is the first of the three fundamental motions of weaving. The sequence is: shed, pick (pass the weft through), beat (pack it in). Repeat.

### Rising Shed

**Definition:** A shed created by lifting certain shafts up from a neutral position while the rest stay in place. Jack looms are rising-shed looms.

**Why it matters:** The tie-up notation differs between rising-shed and sinking-shed looms. A filled square in a rising-shed tie-up means "this shaft goes UP when this treadle is pressed." Most modern handweaving drafts and software assume rising shed.

**Computational connection:** In a rising-shed system, a filled square (1) in the tie-up matrix means "warp thread is raised, so it will be on top of the weft and visible on the face." This is the convention used in WIF files when `Rising Shed=true`.

### Sinking Shed

**Definition:** A shed created by pulling certain shafts down while the rest stay in place. Counterbalance and some countermarch looms work this way.

**Why it matters:** The tie-up is the inverse of a rising-shed tie-up. To convert between them, you swap every filled square for an empty one and vice versa (bitwise NOT). If you use a rising-shed draft on a sinking-shed loom without converting, the pattern will be inverted -- warp-dominant areas become weft-dominant and vice versa.

**Computational connection:** Sinking-shed is the bitwise complement of rising-shed. If rising-shed tie-up is matrix T, sinking-shed is ~T (NOT T). The drawdown result is also inverted: what was warp-on-top becomes weft-on-top.

---

## Part 5: The Draft (The Pattern Blueprint)

### Draft (Weaving Draft)

**Definition:** A visual diagram that encodes all the instructions needed to weave a specific pattern. It is a set of grids on paper (or screen) that tells the weaver how to thread the loom, how to connect the treadles to the shafts, and what sequence to press the treadles in.

**Why it matters:** The draft is the *source code* of the fabric. It is a complete, reproducible specification -- any weaver with the right loom can take a draft and produce the same fabric from it.

**Analogy:** A draft is to weaving what sheet music is to music performance, or what a blueprint is to building construction. It is not the fabric itself -- it is the instructions for making it.

**Computational connection:** A draft is a factored representation of a binary matrix. Instead of storing every pixel of an M x N fabric bitmap, the draft stores three smaller structures (threading, tie-up, treadling) that, when composed, *generate* the full bitmap. The formula: `Drawdown[y][x] = Tieup[Treadling[y]][Threading[x]]`. This is compression by construction -- the same principle as expressing an image as a short program rather than a raw bitmap.

A draft has four quadrants:

### Threading

**Definition:** The grid at the top of the draft. It specifies which shaft each warp end is assigned to. Each column represents one warp thread; each row represents one shaft. A filled square at column X, row S means "thread X goes through a heddle on shaft S."

**Why it matters:** Threading is the first creative decision. It determines the set of patterns that are *possible* with this warp setup. Once the loom is threaded, you can change the treadling sequence to get different patterns, but you cannot change which threads are on which shafts without re-threading.

**Analogy:** Threading is like wiring a switchboard. You are deciding which output (warp thread) is connected to which control group (shaft). Once wired, you can flip the switches (treadles) in any order, but the wiring is fixed.

**Computational connection:** Threading is a function from column index to shaft number: `Threading: x -> shaft`. For a straight draw, `Threading[x] = (x mod N) + 1`. For a point draw (herringbone), the function reverses direction at the midpoint. This is the x-axis component of the factored representation.

### Tie-Up

**Definition:** The small grid in the corner of the draft (between threading and treadling). It specifies which shafts each treadle activates. Rows represent shafts; columns represent treadles. A filled square at row S, column T means "when treadle T is pressed, shaft S will rise."

**Why it matters:** The tie-up is the *rule set* that defines how foot actions translate into shed configurations. Two different tie-ups with the same threading and treadling will produce two entirely different fabrics.

**Analogy:** The tie-up is like a keyboard mapping or macro definition. It says "when I press this key (treadle), execute this combination of actions (raise these shafts)."

**Computational connection:** The tie-up is a boolean matrix `T[shaft][treadle]`. It is the lookup table at the heart of the draft computation. In the formula `Drawdown[y][x] = Tieup[Treadling[y]][Threading[x]]`, the tie-up is the function that maps a (treadle, shaft) pair to a binary output (warp up or warp down). For simple structures like twills, the tie-up can be expressed as a threshold comparison. For complex structures, it requires explicit encoding.

### Treadling

**Definition:** The grid running vertically along one side of the draft. It specifies the order in which the treadles are pressed. Each row represents one pick (one row of weft); a filled square indicates which treadle is pressed for that pick. You read it from top to bottom.

**Why it matters:** The treadling is the *sequence of actions* the weaver performs. It is the temporal, step-by-step part of the draft -- the "do this, then this, then this" instruction list.

**Analogy:** If threading is wiring a piano's keys, and the tie-up is tuning each key, then the treadling is the musical score -- it tells you which keys to press, in what order, to play the piece.

**Computational connection:** Treadling is a function from row index to treadle number: `Treadling: y -> treadle`. For a straight treadling on 4 treadles: `Treadling[y] = (y mod 4) + 1`. This is the y-axis component of the factored representation. The treadling sequence is the program's main loop.

### Drawdown

**Definition:** The grid occupying the main body of the draft. It shows the resulting fabric -- which thread (warp or weft) is visible at each intersection point. A filled square typically means the warp thread is on top; an empty square means the weft is on top.

**Why it matters:** The drawdown is the *preview* of the fabric. It is what you actually see when you look at the woven cloth. But crucially, the drawdown is *computed*, not specified directly -- it is the output of combining threading + tie-up + treadling.

**Analogy:** The drawdown is like the rendered image on screen -- it is the result of running the program, not the program itself.

**Computational connection:** `Drawdown[y][x] = Tieup[Treadling[y]][Threading[x]]`. This is a boolean matrix multiplication: AND for multiply, OR for addition. The drawdown is the uncompressed bitmap; the three smaller grids are the compressed representation. For a WIF file, the drawdown is never stored -- it is always recomputed.

---

## Part 6: Physical Properties of Cloth

### Sett

**Definition:** The density of warp threads, measured as the number of ends per inch (EPI). A sett of 20 means there are 20 warp threads in every inch of width.

**Why it matters:** Sett determines the character of the fabric. Too tight (too many threads per inch) produces cardboard-stiff cloth. Too loose (too few) produces limp, unstable fabric with threads that shift around. The right sett depends on the yarn thickness and the weave structure.

**How to determine sett:** Wrap the yarn tightly around a ruler for one inch, count the wraps. For plain weave, divide by 2. For twill, multiply by 0.67. This accounts for the fact that different weave structures need different densities because the weft travels differently through the warp.

**Computational connection:** Sett defines the resolution of the x-axis -- it is the DPI (dots per inch) of the fabric in the horizontal direction. Combined with picks per inch (PPI), it defines the aspect ratio of each "pixel" in the cloth.

### Picks Per Inch (PPI)

**Definition:** The density of weft rows, measured as the number of picks in one inch of fabric height.

**Why it matters:** Together with sett (EPI), PPI defines the overall density and drape of the fabric. When EPI and PPI are equal, the fabric is "square" -- each crossing point is roughly square-shaped.

### Selvedge (also Selvage)

**Definition:** The finished edge on both sides of a woven fabric, formed naturally as the weft yarn reverses direction at the end of each row.

**Why it matters:** The selvedge prevents the fabric from unraveling at the edges. In handweaving, getting clean, even selvedges is a skill -- the weft must be inserted at a slight angle and with consistent tension to avoid pulling the edges inward (draw-in) or leaving loose loops.

**Practical note:** "Floating selvedges" are extra warp threads at each edge that are not threaded through any heddle. They float freely, ensuring the weft always has something to wrap around regardless of which shed is open.

### Float

**Definition:** A length of yarn that passes over (or under) two or more adjacent threads without interlacing. A warp float is a warp thread riding on top of multiple weft rows; a weft float is a weft thread crossing over multiple warp ends.

**Why it matters:** Floats are both a design tool and a structural concern. Short floats (over 2-3 threads) create textures like twill diagonals. Longer floats create bold pattern effects (as in satin weave or overshot), but they are less structurally secure -- they can snag, shift, or look loose.

**Analogy:** In knitting, a float is when yarn stretches across the back of the work between stitches. In weaving, it is the same concept -- a thread traveling freely without being locked in place by interlacement.

**Computational connection:** In the binary drawdown grid, a float is a consecutive run of the same value (1s or 0s) in a row or column. A warp float of length 3 means three consecutive 1s in a column; a weft float of length 3 means three consecutive 0s in a row. Float length is a key constraint in pattern design -- most practical fabrics limit floats to prevent structural problems.

### Balanced Weave

**Definition:** A fabric in which the warp and weft are equally visible -- same yarn size in both directions, same density (EPI equals PPI), and both sets of threads share the fabric surface equally.

**Why it matters:** Plain weave woven with the same yarn at the same density in both directions is the archetypal balanced weave -- you see equal amounts of warp and weft on the surface. Many weaving sett calculations aim for a balanced cloth as the default.

**Counterpart -- Unbalanced Weave:** A fabric where one direction dominates. *Warp-faced* fabrics have the warp packed so densely that the weft is hidden (e.g., inkle bands). *Weft-faced* fabrics have the weft beaten so tightly that the warp is hidden (e.g., tapestry). Unbalanced weaves are intentional design choices.

**Computational connection:** In a balanced plain weave, EPI = PPI, so each pixel in the drawdown is roughly square. In a warp-faced fabric, the x-resolution is much higher than the y-resolution -- pixels are tall and narrow. The aspect ratio of the cloth pixels matters when translating between screen previews and physical fabric.

---

## Part 7: Basic Weave Structures

### Plain Weave (also Tabby)

**Definition:** The simplest possible weave structure. The weft goes over one warp thread, under the next, over the next, under the next, across the entire row. On the next row, the pattern reverses -- over becomes under and vice versa.

**Why it matters:** Plain weave is the foundation. It is the most stable, most balanced, and most common weave structure. It requires only 2 shafts. Many more complex structures (like overshot) use plain weave as their ground cloth.

**Computational connection:** `(x + y) % 2`. A checkerboard. The simplest possible binary pattern -- it can be expressed in 5 instructions in PXL: `x y + 2 %`.

### Twill

**Definition:** A weave structure where the weft floats over two or more warp threads, with the pattern shifting one position on each successive row, creating a diagonal line. Denim is the most familiar example of twill.

**Why it matters:** Twill is the next step up in complexity from plain weave and introduces the concept of floats, pattern repeats, and diagonal movement. A "2/2 twill" means the weft goes over 2 threads, under 2 threads, and shifts by one each row -- the classic diagonal. Requires 4 shafts.

**Computational connection:** `(x + y) % N < threshold`. A 2/2 twill on 4 shafts: `(x + y) % 4 < 2`. This is a diagonal stripe in modular arithmetic. In PXL: `x y + 4 % 2 <`. Changing the threshold or modulus changes the twill ratio.

### Satin

**Definition:** A weave structure where floats are spaced out so that no two interlacement points are adjacent. This creates a smooth, lustrous surface because the long floats reflect light evenly.

**Why it matters:** Satin demonstrates how float placement -- not just float length -- affects fabric character. The mathematical spacing rule (typically using a coprime increment) ensures the interlacement points are maximally dispersed.

**Computational connection:** A 5-end satin: `(x * 3 + y) % 5 == 0` (where 3 is coprime to 5). The multiplication by a coprime number scatters the interlacement points evenly across the repeat unit. In PXL: `x 3 * y + 5 % 0 ==`.

### Basket Weave

**Definition:** Like plain weave, but with pairs of threads treated as one. The weft goes over 2, under 2, and on the next row, it does the same (not offset). The third row shifts to mirror the first pair of rows.

**Computational connection:** `(floor(x/2) + floor(y/2)) % 2`. The same as plain weave but with each coordinate integer-divided by 2 first -- a 2x scale.

---

## Part 8: Advanced Structures and Concepts

### Block

**Definition:** A group of warp threads (and corresponding weft rows) that all interlace the same way, functioning as a unit. Instead of thinking thread-by-thread, you think in blocks -- each block can be either "pattern" or "background."

**Why it matters:** Block-based thinking is the key to designing complex patterns on shaft looms. In a block weave, you can create large-scale geometric patterns (diamonds, squares, curves) even though the underlying loom only has a limited number of shafts. Different block weave structures (overshot, Summer & Winter, Ms & Os) define the blocks differently, but the design principle is the same: assign groups of threads to blocks, then decide which blocks show pattern and which show background on each row.

**Computational connection:** Blocks are an abstraction layer. Instead of specifying every thread crossing individually, you specify at the block level and let the weave structure expand each block into the actual thread-by-thread interlacement. This is hierarchical compression -- the same principle as vector graphics (describe shapes) vs. raster graphics (describe pixels).

### Overshot

**Definition:** A weave structure that combines a plain-weave ground cloth with a separate pattern layer made of thicker yarn that "overshoots" (floats over) groups of warp threads. Two shuttles are used -- one for the thin tabby (ground) weft, one for the thick pattern weft -- and they alternate: tabby pick, pattern pick, tabby pick, pattern pick.

**Why it matters:** Overshot is historically one of the most important American handweaving traditions and produces bold, geometric patterns. It demonstrates a fundamental principle: a pattern layer and a structural layer can be independent. The tabby holds the fabric together; the pattern weft creates the visual design. If you pulled out all the pattern weft, you would be left with a complete plain-weave cloth.

**Analogy:** Overshot is like writing on lined paper. The lines (tabby) provide structure, and the ink (pattern weft) provides the design. The lines exist independently of whatever you draw on top of them.

**Computational connection:** Overshot is a two-layer system: a base layer (plain weave) and a mask layer (pattern floats). In Bitshift terms, this maps naturally to the layer architecture -- a base tabby layer plus a pattern layer with a float mask. The pattern weft's float behavior at any point is determined by the block structure of the threading.

### Profile Draft

**Definition:** A simplified, condensed version of a draft that shows the design at the *block* level rather than the thread level. Each square in a profile draft represents an entire block of threads, not a single thread. A profile draft is shorthand -- you cannot weave directly from it. You must first "expand" it by substituting each block with the actual threading/treadling for your chosen weave structure.

**Why it matters:** Profile drafts separate *design* (the pattern layout) from *implementation* (the specific weave structure). The same profile draft can be woven as overshot, Summer & Winter, or any other block weave structure -- you just change the expansion rules. This is a powerful design abstraction.

**Analogy:** A profile draft is like a wireframe mockup of a website. It shows the layout and proportions but not the final visual details. The same wireframe can be implemented with different CSS, just as the same profile draft can be woven with different structures.

**Computational connection:** Profile drafts are parametric design at its most explicit: the pattern (profile) is a variable; the weave structure (expansion rules) is a separate variable. This is a clean separation of concerns -- the two can be varied independently. The profile is a low-resolution design; the expansion is an upsampling function that maps each block to a specific thread-level pattern.

### Color-and-Weave

**Definition:** Patterns created by the interaction between the color order of the yarns and the weave structure. Even a simple plain weave can produce complex visual patterns (houndstooth, shepherd's check, log cabin) when the warp and weft use specific color sequences.

**Why it matters:** Color-and-weave reveals that the visual pattern of a fabric is not just the weave structure alone -- it is the weave structure *multiplied by* the color arrangement. The same weave structure with different color orders produces completely different visual results.

**Computational connection:** At each pixel, the drawdown determines whether the warp thread or weft thread is visible. The color you see is `if drawdown[y][x] == 1 then warp_color[x] else weft_color[y]`. The color sett (color order) is an additional data layer: `warp_color: x -> color`, `weft_color: y -> color`. The visual output is the product of two independent systems (structure and color).

---

## Part 9: Types of Looms (A Complexity Hierarchy)

### Rigid Heddle Loom

**Definition:** The simplest shaft-like loom. It has a single rigid heddle -- a frame with alternating slots and holes. Threads in the holes move with the heddle; threads in the slots stay put. This gives you two sheds: heddle up and heddle down.

**Why it matters:** A rigid heddle loom can only produce plain weave (and minor variations with pick-up techniques). It is the beginner's loom, but it clearly demonstrates the fundamental mechanism: separating threads into two groups.

**Computational connection:** A 1-bit system. The only possible output is `(x + y) % 2` -- plain weave. Two possible sheds.

### Floor Loom (Multi-Shaft, Treadle Loom)

**Definition:** A loom with multiple shafts (typically 4 to 16) controlled by foot treadles. The weaver presses treadles to create different sheds, allowing pattern weaving without manually picking up individual threads.

**Why it matters:** This is the "standard" handweaving loom. Four shafts with a well-designed tie-up and treadling can produce an enormous variety of patterns: twills, satins, overshot, lace, and more.

**Computational connection:** A modular system. With N shafts, the pattern space is defined by operations modulo N. The threading, tie-up, and treadling form a factored representation of the output. The three-component factorization (threading x tie-up x treadling) is a form of data compression -- it represents a large bitmap as three small lookup tables.

#### Jack Loom

A floor loom where shafts are pushed *up* from below. Rising-shed loom. When no treadles are pressed, all shafts are in the resting (down) position. Simple mechanism, easy to tie up, but may produce uneven sheds on complex threadings.

#### Counterbalance Loom

A floor loom where shafts are connected in pairs on a pulley system -- when one goes up, its partner goes down. Produces excellent sheds for balanced structures (half the shafts up, half down) but struggles with unbalanced lifts. Sinking-shed behavior.

#### Countermarch Loom

A floor loom with two sets of lamms (upper and lower) that allow each shaft to be independently raised OR lowered on every pick. Every shaft is either actively pulled up or actively pulled down -- nothing floats passively. Produces the cleanest, most even sheds across all structures. Common in Scandinavian weaving traditions.

### Dobby Loom

**Definition:** A loom where the shaft selection for each pick is controlled by a mechanical or electronic pattern device rather than direct treadle-to-shaft tie-ups. Originally used a chain of pegged bars or punched cards; modern dobby looms use a computer interface.

**Why it matters:** A dobby loom removes the physical limit of how many treadles your feet can reach. A 24-shaft dobby loom can use any combination of its 24 shafts on every pick -- the weaver only needs to advance the pattern chain (or the computer advances it automatically). This dramatically expands the pattern space.

**Computational connection:** A dobby loom replaces the tie-up + treadling factorization with a direct lift plan -- a list of shaft combinations, one per pick. The lift plan is like a program listing: "Pick 1: raise shafts 1,3,5. Pick 2: raise shafts 2,4,6..." A 24-shaft dobby has approximately 16.7 million possible shed configurations per pick.

### Jacquard Loom

**Definition:** A loom that controls each warp thread individually rather than in groups (shafts). Originally invented by Joseph Marie Jacquard in 1804 using a chain of punched cards, where each card has one position per warp thread: hole = raise thread, no hole = leave thread down.

**Why it matters:** A Jacquard loom can weave *any* pattern -- photographic images, curved lines, intricate figures, text. There is no restriction to the geometric patterns imposed by shaft groupings. This is the fully general-purpose weaving machine.

**Historical significance:** The Jacquard punch card is one of the most important inventions in the history of computing. Charles Babbage planned to use punch cards for his Analytical Engine. Ada Lovelace wrote: "The Analytical Engine weaves algebraic patterns, just as the Jacquard loom weaves flowers and leaves." Herman Hollerith adapted the concept for the 1890 U.S. Census, founding the company that became IBM.

**Computational connection:** A Jacquard loom is a 1-bit-per-pixel display. Each punch card is one row of a bitmap. The card chain is the full bitmap, read row by row. There is no factorization, no compression, no abstraction layer -- the instruction *is* the output, pixel by pixel. In Bitshift terms: PXL is a programmable Jacquard with a computed card chain.

---

## Part 10: How It All Connects

### The Three Fundamental Motions of Weaving

Every loom, from the simplest to the most complex, performs the same three actions in a cycle:

1. **Shedding** -- Separate the warp threads into two groups (raised and lowered) to create an opening.
2. **Picking** -- Pass the weft through the shed.
3. **Beating** -- Push the new weft row snugly against the previous rows.

Repeat. Every row of fabric is one iteration of this cycle.

### The Grammar of Drafts

The four-quadrant draft system can be understood as a grammar:

| Component | Grammatical Role | What It Specifies |
|-----------|-----------------|-------------------|
| Threading | Vocabulary | What elements (shaft assignments) are available |
| Tie-up | Grammar rules | How elements combine (which shafts group together) |
| Treadling | Syntax / sequence | The order of operations |
| Drawdown | Meaning / output | The resulting pattern |

The power of this system is that you can change any component independently. Same threading with a different tie-up gives a different fabric. Same tie-up with a different treadling gives yet another. This combinatorial independence is what makes the draft system so expressive -- and what makes it a natural fit for computational exploration.

### The Binary Nature of Weaving

At every intersection of warp and weft, there are exactly two possibilities: the warp thread is on top, or the weft thread is on top. There is no third option. This makes weaving inherently binary -- every woven fabric is a bitmap, a grid of 1s and 0s, where 1 means "warp on top" and 0 means "weft on top."

This binary nature means:
- Every weave structure is a boolean matrix
- The draft computation is boolean matrix multiplication
- The Jacquard punch card is literally a bitmap row
- The entire history of looms is a history of increasingly powerful mechanisms for generating binary sequences

This is why weaving is often called one of the earliest forms of computation -- and why the loom is a direct ancestor of the computer.

---

## Quick Reference: The Loom Complexity Hierarchy

| Loom Type | Shafts | Possible Sheds per Pick | Computational Equivalent |
|-----------|--------|------------------------|--------------------------|
| Rigid heddle | 2 | 2 | 1-bit: plain weave only |
| 4-shaft treadle | 4 | 14 | Modular arithmetic mod 4 |
| 8-shaft treadle | 8 | 254 | Modular arithmetic mod 8 |
| 24-shaft dobby | 24 | ~16.7 million | Arbitrary modular expressions |
| Jacquard | per thread | 2^(thread count) | Arbitrary per-pixel (full bitmap) |

---

## Sources

- [Gist Yarn: Weaving Glossary](https://www.gistyarn.com/blogs/how-to-weave/weaving-glossary)
- [Gist Yarn: How to Read a Weaving Draft](https://www.gistyarn.com/blogs/how-to-weave/how-to-read-a-weaving-draft)
- [Gist Yarn: Reading and Understanding Profile Drafts](https://www.gistyarn.com/blogs/how-to-weave/reading-and-understanding-profile-drafts)
- [Gist Yarn: Weave Structures: Overshot](https://www.gistyarn.com/blogs/how-to-weave/weave-structures-overshot)
- [Gist Yarn: Understanding Block Weaves vs. Unit Weaves](https://www.gistyarn.com/blogs/how-to-weave/understanding-block-weaves-vs-unit-weaves)
- [Glossary of Weaving Terms (U.S. National Park Service)](https://www.nps.gov/articles/000/glossary-of-weaving-terms.htm)
- [SweetGeorgia Yarns: Weaving Words Glossary](https://sweetgeorgiayarns.com/weaving-words-a-glossary-of-weaving-terms/)
- [Schacht Spindle Company: Weaving Terms](https://schachtspindle.com/blogs/faqs/weaving-terms)
- [Kelly Casanova: What is the Sett in Weaving](https://kellycasanovaweavinglessons.com/2019/05/what-is-the-sett-in-weaving.html)
- [Joy of Weaving: How to Read a Weaving Draft](https://joyofweaving.com/articles/how-to-read-a-weaving-draft/)
- [Weaving Space: Reading a Tie-Up](https://weavingspace.co.uk/reading-a-tie-up/)
- [Comfortcloth Weaving: The Grammar of Drafting](https://comfortclothweaving.com/article/the-grammar-of-drafting)
- [Comfortcloth Weaving: Overshot Basics](https://comfortclothweaving.com/article/overshot-basics)
- [Roots of Progress: Learning the Loom](https://blog.rootsofprogress.org/learning-the-loom)
- [Handwoven Magazine: Profile Drafts](https://handwovenmagazine.com/how-to-weave-profile-drafts/)
- [Handwoven Magazine: What's a Block Weave?](https://handwovenmagazine.com/ask-madelyn-whats-a-block-weave/)
- [Handwoven Magazine: Jack Looms and Counterbalance Looms](https://handwovenmagazine.com/looms/)
- [Jane Stafford Textiles: Tie-ups on Sinking vs Rising Shed](https://janestaffordtextiles.com/knowledge-base/tie-ups-on-sinking-shed-versus-rising-shed-looms/)
- [Eugene Textile Center: How Looms Create a Shed](https://www.eugenetextilecenter.com/how-looms-create-a-shed)
- [Berliner Webstuehle: Selvedge Explained Simply](https://weavingloomsdirect.com/blogs/news/selvedge-how-to-weave-clean-edges)
- [Wikipedia: Warp and Weft](https://en.wikipedia.org/wiki/Warp_and_weft)
- [Wikipedia: Shed (Weaving)](https://en.wikipedia.org/wiki/Shed_(weaving))
- [Wikipedia: Dobby Loom](https://en.wikipedia.org/wiki/Dobby_loom)
- [Fiberart.com: Dobby vs Jacquard Loom](https://www.fiberart.com/post/what-is-the-difference-between-a-dobby-and-jacquard-loom)
- [Loomery: The Loom -- Programming Patterns on a Path to Computing](https://www.loomery.com/insights/the-loom)
- [TEXTILE Journal: Weaving as Binary Art and the Algebra of Patterns](https://www.tandfonline.com/doi/abs/10.1080/14759756.2017.1298239)
- [Warped Fibers: Overshot Weaving](https://warpedfibers.com/overshot-weaving/)
