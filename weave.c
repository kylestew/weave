#include "weave.h"

void loom_init(Loom *loom, uint32_t seed) {
    (void)seed; // will be used for RNG in Step 5
    memset(loom, 0, sizeof(Loom));

    // Straight draw threading: each warp end cycles through shafts 0,1,2,3
    for (int i = 0; i < WARP_ENDS; i++)
        loom->threading.shaft[i] = i % SHAFTS;

    // 2/2 Twill tie-up: two shafts raised per pedal, shifting diagonally
    uint8_t twill[4][4] = {
        {1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1}
    };
    memcpy(loom->tieup.matrix, twill, sizeof(twill));

    // Straight treadling: press pedals 0,1,2,3 in order
    loom->treadling.sequence[0] = 0;
    loom->treadling.sequence[1] = 1;
    loom->treadling.sequence[2] = 2;
    loom->treadling.sequence[3] = 3;
    loom->treadling.length = 4;
    loom->treadling.direction = 1;

    // Palette: indigo + natural (undyed cotton)
    loom->palette[0] = (Color){45, 52, 94};    // indigo
    loom->palette[1] = (Color){210, 195, 170};  // natural
    loom->palette_size = 2;

    // Solid warp sett (all indigo), weft is natural
    loom->warp_sett.indices[0] = 0;
    loom->warp_sett.length = 1;
    loom->weft_color_index = 1;

    loom->grid_head = 0;
    loom->pick = 0;

    // Schedule first evolution events (hardcoded for now, randomized in Step 5)
    loom->next_threading_change   = 200;
    loom->next_tieup_mutation     = 30;
    loom->next_treadling_change   = 100;
    loom->next_treadling_rotation = 50;
    loom->next_weft_color_change  = 20;

    loom->paused = 0;
}

// Core drawdown formula: Tieup[Treadling[y]][Threading[x]]
// For the current pick, which treadle is pressed? Then check if that
// treadle raises the shaft this warp end is on.
int drawdown_cell(const Loom *loom, int x) {
    // Which treadle (foot pedal) is active for the current pick?
    int seq_pos = (int)(loom->pick % (uint32_t)loom->treadling.length);
    if (loom->treadling.direction < 0)
        seq_pos = loom->treadling.length - 1 - seq_pos;
    int treadle = loom->treadling.sequence[seq_pos];

    // Which shaft (frame) is this warp end threaded on?
    int shaft = loom->threading.shaft[x];

    // Tie-up lookup: does this pedal raise this frame?
    return loom->tieup.matrix[treadle][shaft];
}

// Map a drawdown result to a color palette index.
// If warp is on top (drawdown=1), use the warp sett color for this column.
// If weft is on top (drawdown=0), use the current weft color.
uint8_t resolve_color(const Loom *loom, int x, int drawdown) {
    if (drawdown) {
        return loom->warp_sett.indices[x % loom->warp_sett.length];
    } else {
        return loom->weft_color_index;
    }
}

// Compute one new row of fabric and write it into the ring buffer.
void advance_loom(Loom *loom) {
    int row = loom->grid_head;
    for (int x = 0; x < WARP_ENDS; x++) {
        int dd = drawdown_cell(loom, x);
        loom->grid[row][x] = resolve_color(loom, x, dd);
    }
    loom->grid_head = (loom->grid_head + 1) % VISIBLE_ROWS;
    loom->pick++;
}
