# Alternative (Non-Screen) Displays for Weave Patterns

The drawdown is a binary grid (warp-on-top / weft-on-top) mapped to colors from a small palette. The viewer should see the process in action — not a final artifact.

---

## Kept from original list

### 0. LED Matrix Under Frosted Glass
A dense LED matrix (P2.5 or finer) mounted behind a sheet of frosted glass or acrylic. The glass diffuses each pixel into a soft glow — no sharp pixel grid visible. The drawdown becomes a luminous field of blurred color, like stained glass lit from behind. Evolution events are visible as slow color shifts rippling across the surface. At a distance it reads as abstract painting. Up close you see the glow of individual cells bleeding into their neighbors. The frost *is* the weave — it interlaces the light.

> **[comment]** This is essentially what the screen version already does, but with better materiality. The honesty problem: is frosted glass over LEDs meaningfully different from a monitor? I think it is, but only if the physical object asserts itself — the thickness of the glass, the warmth of the glow, the absence of a bezel or UI. Consider: mount it in a frame, hang it like a painting, and never show any interface. It's just a glowing rectangle that slowly changes. No one needs to know it's a loom simulation. The "frost is the weave" line does real work.

### 1. Thermal Printer on Continuous Roll
A receipt printer produces a dithered strip of fabric pattern in real time. Cheap, lo-fi, infinite length. Mount the printer high on a wall and let the paper curl down to the floor. The evolution unfolds physically as the roll grows. Black-and-white only, but the drawdown *is* binary.

> **[comment]** Strongest first-prototype candidate. A serial-connected thermal printer (Adafruit Mini Thermal, ~$50) can receive raster data directly from the C program over UART. You could have this running in days, not weeks. Width is narrow (~384 dots) but that's enough for a scaled drawdown. The paper curling to the floor is a great image — consider the total run length as part of the piece (how many generations before it touches the ground?).

### 2. Embroidery Machine
Feed the grid to a computerized embroidery machine (Brother PE800, etc.). Each cell = one stitch block in the corresponding palette color. The output is actual thread on actual fabric. Frame it. The thread texture is the point — you can feel the warp/weft structure. In gallery: the machine runs live, hoop visible, needle punching in real time. The growing field of stitches *is* the piece.

> **[comment]** The output format matters here — most embroidery machines expect .PES or .DST files, not raw bitmaps. You'd need a conversion step (probably ink/stitch or pyembroidery). Hoop size limits you to ~130x180mm per frame on consumer machines, which caps the grid resolution per piece. The "live in gallery" angle is strong but the machine pauses for thread color changes — with 4 palette colors that's 3 pauses per piece. Could lean into those pauses as breath marks.

### 3. Modular Pin Board
A pegboard where each hole holds a colored wooden dowel or peg (like a Lite-Brite without the light). 64x64 holes, 4 colors of pegs. Swap pegs to update the pattern. A physical, low-tech display you maintain by hand — the act of placing each peg mirrors the act of threading each warp end. Evolution becomes a ritual of rearrangement.

> **[comment]** 64x64 = 4,096 pegs. Even if only 10% change per evolution event, that's ~400 swaps. This is either a beautiful durational performance or a logistical nightmare — depends on whether the labor is the point. If it is: consider printing a "diff sheet" (thermal printer again?) that tells you which pegs changed and to what color. The program could output that. The weaving-as-ritual framing is honest and good.

### 4. Flip-Dot Display (Live Clatter)
Electromechanical flip-dot panels where physical discs clatter between black and yellow. On each new pick, an entire row of discs flips with an audible snap. The oldest row scrolls off the top and the newest appears at the bottom. The binary drawdown maps perfectly. The sound fills the room — you hear the pattern being woven.

> **[comment]** Alfa-Zeta and Hanover sell panels that tile. A 64-wide row is ~2 panels. The RS-485 protocol is well-documented and people have driven them from Raspberry Pi. The sonic quality is genuinely compelling — the density of simultaneous flips varies with the pattern, so you'd actually *hear* the difference between a plain weave (half the dots flip every row) and a satin (sparse changes). The binary constraint is a feature, not a limitation. This and #1 are the two most buildable entries on this list.

### 5. Split-Flap Display
TODO: write description for this

https://github.com/scottbez1/splitflap

> **[comment]** On the TODO — the scottbez1 design is single-character per module (airport departure board style). Each module cycles through a fixed set of printed flaps. For a drawdown you'd want two flap faces: one warp-color, one weft-color. That's a custom flap set, which the project supports. The key difference from flip-dot (#4) is that split-flap has a *sweep* — each module cascades through intermediate states before landing. So an evolution event wouldn't snap into place, it would ripple and chatter as every module hunts for its target. That cascade *is* the description. The sound is louder and more chaotic than flip-dot — think train station, not typewriter. Cost is high: 64x64 = 4,096 modules. Even at a reduced grid (16x16?) this is an expensive build. Maybe this is better as a single-row display like the solenoid loom — one row of 64 modules showing the current pick, cycling on each step.

### 6. Solenoid Loom (Audible Weave)
An array of 64 solenoids mounted in a row, each holding a metal pin. On each pick the active solenoids fire — a burst of clicks that encodes the shed pattern as sound. A contact microphone amplifies the array into the room. No visual output at all — the drawdown becomes a rhythmic, percussive score. Each evolution event changes the rhythm. Visitors hear the structure of cloth.

AMAZING IDEA, sonify the pattern

TODO: explain how the visual state differs from non-visual (off state)

> **[comment]** On the TODO — I think what you're getting at is: when the display is purely auditory, what does "off" or "idle" mean? On a screen, no evolution = static image. On the solenoid array, no evolution = silence. That's a meaningful difference — silence reads as "dead," not "stable." One answer: in the off/idle state, keep the solenoids firing the *current* pattern on a slow loop, so stability sounds like repetition rather than absence. The pattern drones. Only evolution events break the rhythm. Another answer: lean into the silence. The piece is only alive when it's changing. The gaps between evolution events are the rests in the score.
>
> This connects strongly to #8, #9, #10, #12, #13 — you have a whole cluster of sonification ideas. Worth thinking about whether these are separate pieces or one configurable audio engine with different physical front-ends.

### 7. Weaving Notation Typewriter
A modified electric typewriter with a custom typeball or daisy wheel carrying block characters (full block, half block, etc.) in 2 strike colors (black ribbon + red ribbon). The machine types the drawdown row by row onto continuous-feed paper, advancing one line per pick. The carriage return, the keystrike, the line feed — all audible, all rhythmic. The paper feeds off the platen and down the wall. A textile pattern rendered as typewritten text.

### 8. Music Box Cylinder
Map the drawdown onto a rotating brass cylinder with raised pins. Each column = one tuned tine. Warp-up cells get a pin. As the cylinder turns, tines pluck — the pattern becomes a melody. A plain weave alternates every other note. A twill walks up the scale. An evolution event rewrites the pins and the tune mutates mid-phrase. Build it at 64 tines and the harmonic density of a satin weave becomes a shimmering cluster chord.

TODO: can the brass cylinder with raised pins be re-written on the fly by the algorithm?

> **[comment]** On the TODO — not with a traditional brass cylinder, no. The pins are punched or soldered permanently. But there are workarounds: (a) a MIDI-driven solenoid array behind a comb — essentially a programmable music box where solenoid plungers replace fixed pins. RMIF and others have built these. (b) a DIY approach with a rotating drum of holes and retractable pins (push a pin up = active, retract = inactive), driven by a shift register per row. This is mechanically complex but not impossible. (c) Skip the physical cylinder entirely and use the music box as a *metaphor* — drive a kalimba/mbira tine array with solenoids, one per tine. The sound is authentic, the mechanism is programmable. Option (c) is probably the right move. It's also basically a physical instantiation of #10 (modular synth sequencer) with acoustic rather than electronic voices.

### 9. Relay Matrix as Audio Router
A wall of 64 electromagnetic relays wired as a switching matrix. Input: a live audio feed (radio, field recording, someone playing guitar in the next room). Each relay connects or disconnects one band-pass channel. The drawdown row determines which frequency bands pass through at any moment. The weave structure literally *filters* sound — plain weave chops the signal into a stuttering comb filter, satin lets wide bands through smoothly, and every evolution event reshapes what you hear. The relays click as they switch. The click *and* the filtered signal both fill the room.

### 10. Modular Synth Sequencer
Each row of the drawdown is one step in a 64-channel gate sequencer. Warp-up = gate high, triggering the corresponding oscillator or voice. The treadling order determines step sequence. Tie-up changes = patch reconfiguration. As the pattern evolves, the sequence audibly morphs — a new threading event rewrites which voices fire, a treadling mutation reorders the rhythm. Patch it into a rack and the loom *is* the composition engine. Output CV too: let the drawdown control voltage for external gear.

TODO: what if instead of the drawdown line output, its the inputs

> **[comment]** On the TODO — this is an interesting inversion. Instead of the drawdown *controlling* the sequencer, let the sequencer (or any external CV/gate source) control the *loom inputs* — threading, treadling, tie-up. A musician plays the loom. Gate signals set which shafts rise. A knob sweeps the treadling order. The drawdown becomes a visualization of the performance. This turns the whole project inside out: the loom is an instrument, the weave pattern is the score that the performer writes in real time. You could do this bidirectionally too — the loom drives audio, the audio feeds back into evolution parameters. A cybernetic loop.

### 11. Thread Based Display

https://theartistbreakfast.com/works/thread-screen

> **[comment]** The Thread Screen by Griegst uses stepper-driven spools that twist bicolor thread (black/white) to show one face or the other. It's essentially a flip-dot display made of thread — which is conceptually perfect for this project. The mechanical complexity is extreme (one stepper per cell). At 64x64 that's 4,096 steppers. The original was ~20x20. This might be more of a reference/inspiration than a build target, unless you drastically reduce the grid. But the concept of the display medium *being* thread is too good to ignore. Could a simpler version work — maybe just one row of 64 bicolor threads, showing the current pick?

### 12. Granular Synthesis Cloud
Each warp-up cell spawns one audio grain — a tiny burst of sound (5–50 ms). Column position maps to pitch. Row position maps to time. Grain density follows pattern density: a plain weave is a thick, even cloud; a sparse lace structure is a delicate scatter. The tie-up defines the grain's waveshape. Treadling sets the rhythm of grain emission. Evolution events cause the cloud to mutate in real time — new structures emerge as timbral shifts, not discrete events. Play it through a subwoofer and the loom structure vibrates your chest.

### 13. Telegraph Chorale
One telegraph sounder per treadle (4–8 sounders). The treadling sequence drives them: each pick activates the sounders corresponding to the active treadles. The tie-up determines the mapping from treadle to shaft, so a tie-up change rewires which sounders fire together. Plain weave: two sounders alternating, a steady back-and-forth tick. Complex twill: overlapping polyrhythmic clicking. Mount the sounders on resonant wooden boxes tuned to different pitches. The weave becomes a rhythm ensemble. Speed it up and the clicks fuse into pitched drones — a telegraph choir.

> **[comment]** On the TODO — yes, a telegraph sounder is exactly the device that clicks out Morse code. It's an electromagnet that pulls a metal bar down with an audible clack when current flows. The classic American Morse sounder (J.H. Bunnell & Co. type) is the iconic version. You can still buy originals on eBay or get reproductions. The resonant box idea is great — traditionally they were mounted on "sounders on resonators" (wooden boxes) for exactly this reason, to amplify and give tonal character. So you'd be using them in a historically authentic way, just driven by loom data instead of an operator's key. 4-8 sounders is very doable. This is one of the more buildable audio pieces.

### 14. Shadow Loom
A flat grid of vertical pins (like a bed of nails) mounted on a wall. Servo motors push selected pins outward by 2–3 cm — warp-up cells protrude, weft-up cells stay flush. A single raking light source from one side casts long shadows from the protruding pins across the flat ones. The pattern is visible only as shadow. Walk around it and the shadow geometry shifts — the same drawdown looks different from every angle. At noon (light straight on) the pattern vanishes entirely.

> **[comment]** The viewing-angle dependency is the strongest part of this. It means the piece has a temporal quality even when the pattern is static — the sun moves, the shadows shift, the drawdown transforms throughout the day. In a gallery with controlled lighting, you could motorize the light source instead and sweep it slowly. The pin actuation is the hard part: 4,096 servos is impractical, but a smaller grid (16x16 = 256) with larger pins could work. Or consider solenoids with a bistable mechanism (push-push) so they hold position without power.

### 15. CNC Sand Garden
A flat tray of fine sand with a CNC gantry carrying a stylus. For each generation, the stylus traces the drawdown: pressing deep furrows for warp-up cells, leaving the surface smooth for weft-up. Raking light reveals the pattern as texture and shadow. Before each new generation, the gantry makes one flat pass to erase the previous pattern — a meditative reset. The act of erasure is as visible as the act of drawing. Zen garden meets jacquard.

### 16. Projected Onto Moving Fabric
A projector aimed downward at a length of white fabric being slowly pulled through the beam by a motorized roller. The drawdown is projected at the speed the fabric moves — each row lands on a fresh section of cloth. From above you see only the current few rows. But the fabric accumulates the ghost of the projection as it winds onto a take-up roll below. Unroll it later and the pattern is there as a faded UV memory in the fibers if you use UV-reactive dye in the projector's path.

> **[comment]** The UV-reactive angle is speculative — the exposure time from a standard projector passing over fabric would be very short. Cyanotype-coated fabric (sun-print) would actually work: it's UV-sensitive, cheap, and the exposure is permanent once washed. You'd need a UV light source instead of a visible projector, and the fabric speed would need to be slow enough for exposure (~5-15 minutes per section in sunlight, faster with a strong UV source). The concept of the take-up roll accumulating the entire history is shared with #1 (thermal printer) and #37 (knitting machine) — the roll/spool as archive is a recurring motif in this doc. Worth naming that explicitly.

### 17. Electrochromic Glass Wall
A wall of electrochromic glass tiles (like smart glass) in a grid. Apply voltage and the tile goes opaque/dark; remove it and the tile clears. The drawdown controls the voltage map. Behind the glass wall: a view — a window, a garden, another room. The weave pattern selectively reveals and hides the world behind it. Visitors see *through* the pattern. Dense weaves obscure. Lace structures frame. The pattern is architecture.

> **[comment]** "The pattern is architecture" — that's the whole thesis of this entry and it's strong. The cost is prohibitive for a full 64x64 grid (electrochromic glass is ~$50-100/sq ft for commercial panels). But a smaller installation — say an 8x8 grid of 6" tiles as a room divider or window treatment — is feasible and would read clearly. The slow switching speed of electrochromic glass (~1-5 seconds to full transition) actually helps here: evolution events would ripple across the wall as a slow dissolve, not a snap cut.


