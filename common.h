// Clear sprites
void clear_sprites(void) {
    SMS_initSprites();
    UNSAFE_SMS_copySpritestoSAT();
}

void clear_VRAM(void) {
    // Clear all tile data
    SMS_VRAMmemsetW(0,0, TILE_MAP_0);
    clear_sprites();
}

void clear_screen(unsigned short tileId) {
    SMS_VRAMmemsetW(XYtoADDR(0,0), tileId, 2048);
}

void reset_global_variables(void) {
    clear_sprites();
    SMS_setBGScrollX(0);
    SMS_setBGScrollY(0);
    SMS_setLineCounter(0);
    SMS_disableLineInterrupt();
    SubState = 0;
    FrameCounter = 0;
    Timer = 0;
    Accumulator = 0;
    ScrollYAmount = 0;
    ScrollY = 0;
    ScrollLock = 0;
    Track = AUDIO_NONE;
    PrevTrack = AUDIO_NONE;
    Paused = 0;
    ActClackerFrame = 0;
    AutoScrollDir = 0;
    Sfx = SFX_NONE;
}

void reset_scores(void) {
    // Clear 1 player
    for (unsigned char i = 3; i < 9; i++) {
        Scores[i] = 259;
    }
    // Clear 2 player
    for (unsigned char i = 23; i < 29; i++) {
        Scores[i] = 259;
    }
}

// Increments general use timer to the given threshold, then resets it
// Return true, if the timer hits the threshold
unsigned char increment_timer(unsigned short threshold) {
    Timer++;
    if (Timer >= threshold) {
        Timer = 0;
        return 1;
    }
    return 0;
}

// Sets RNG based on frame counter and user input
void set_rng(unsigned char seed) {
    Rng = (FrameCounter + seed);
}

// Get random runmer, using power of two division
unsigned char get_rng(unsigned char powerOfTwo, unsigned char offset) {
    Rng += offset;
    return Rng >> powerOfTwo;
}

// // Get pseudo-random number (8-bit LFSR). 'powerOfTwo' limits output by shifting.
// // 'offset' is added to the internal seed to vary the sequence.
// unsigned char get_rng(unsigned char powerOfTwo, unsigned char offset) {
//     // Keep the old offset behaviour (adds small entropy/change each call)
//     Rng = (unsigned char)(Rng + offset);

//     // Ensure seed is non-zero (a zero LFSR stays zero)
//     if (Rng == 0) Rng = 0xA5; // arbitrary non-zero seed

//     // 8-bit Galois LFSR step (polynomial x^8 + x^6 + x^5 + x^4 + 1)
//     unsigned char lsb = Rng & 1;
//     Rng >>= 1;
//     if (lsb) Rng ^= 0xB8;

//     // Safety: avoid shifting by >= 8 (undefined); clamp to 0..7
//     if (powerOfTwo >= 8) powerOfTwo = 7;

//     return (unsigned char)(Rng >> powerOfTwo);
// }

// Polls current input state
void poll_input(void) {
    KeysPressed = SMS_getKeysPressed();
    KeysHeld = SMS_getKeysHeld();
    if (KeysHeld != 0) {
        set_rng(KeysHeld << 3);
    }
}

// Gets row number when scrolled past 28 rows
unsigned char get_row(unsigned char row) {
    return row >= 28 ? row - 28 : row;
}

unsigned short get_row_offset(unsigned char row) {
    return row >= 28 ? 1792 : 0;
}

// Gets a map tile id based on the given index
// Info: https://www.smspower.org/maxim/HowToProgram/Tilemap
unsigned short get_map_tile_id(unsigned short index) {
    if (CurrentPlayer == INDEX_PLAYER_01) {
        return P1Map[index] & 511; // Mask out attributes, we just need the 9 bits
    }
    return P2Map[index] & 511; // Mask out attributes, we just need the 9 bits
}

// Sets a map and vram tile at the given entity's tile index with the given value
void set_map_tile(unsigned char index, unsigned short value) {
    if (CurrentPlayer == INDEX_PLAYER_01) {
        P1Map[EntityTile[index]] = value;
        // Is the player in view
        if (EntityVisibility[index] == IN_VIEW) {
            SMS_loadTileMapAreaatAddr(TILE_MAP_0 + ((EntityTile[index] << 1) - get_row_offset(EntityY[index] >> 3)), &P1Map[EntityTile[index]], 1, 1);
        }
    } else {
        P2Map[EntityTile[index]] = value;
        SMS_loadTileMapAreaatAddr(TILE_MAP_0 + ((EntityTile[index] << 1) - get_row_offset(EntityY[index] >> 3)), &P2Map[EntityTile[index]], 1, 1);
    }
}

// Updates view rows in the given direction and amount
void update_row(unsigned char dir, unsigned char amount) {
    // If facing down
	if (dir == DIRECTION_DOWN) {
        // If beyond or at the position threshold
		if (ScrollY + amount >= 96) {
            ScrollY = 96;
            Scroller = INDEX_NONE;
            ScrollYAmount = 0;
            SMS_setBGScrollY(ScrollY);
        // Else, do scroll row update
		} else {
            // Set scroll amount, and the amount that entites will be affected by this scroll
            ScrollY += amount;
            ScrollYAmount = -amount;
			SMS_setBGScrollY(ScrollY);
            // Get row to update, 1 row after row 24
            unsigned char row = ((ScrollY + 200) >> 3);
            // Load map based on current player
            if (CurrentPlayer == INDEX_PLAYER_01) {
                UNSAFE_SMS_VRAMmemcpy64(TILE_MAP_0 + (get_row(row) << 6), &P1Map[row << 5]);
            } else {
                UNSAFE_SMS_VRAMmemcpy64(TILE_MAP_0 + (get_row(row) << 6), &P2Map[row << 5]);
            }
		}
	} else if (dir == DIRECTION_UP) {
        // If beyond or at the position threshold
        if (ScrollY - amount <= 0) {
            ScrollY = 0;
            Scroller = INDEX_NONE;
            ScrollYAmount = 0;
            SMS_setBGScrollY(ScrollY);
        } else {
            // Set scroll amount, and the amount that entites will be affected by this scroll
            ScrollY -= amount;
            ScrollYAmount = amount;
            SMS_setBGScrollY(ScrollY);
            // Get row to update
            unsigned char row = ScrollY >> 3;
            // Load map based on current player
            if (CurrentPlayer == INDEX_PLAYER_01) {
                UNSAFE_SMS_VRAMmemcpy64(TILE_MAP_0 + (row << 6), &P1Map[row << 5]);
            } else {
                UNSAFE_SMS_VRAMmemcpy64(TILE_MAP_0 + (row << 6), &P2Map[row << 5]);
            }
        }
    }
}

// Updates view rows in the given direction and amount, using VDP safe calls
void update_row_safe(unsigned char dir, unsigned char amount) {
    // If facing down
	if (dir == DIRECTION_DOWN) {
        // Set scroll amount, and the amount that entites will be affected by this scroll
        ScrollY += amount;
        ScrollYAmount = -amount;
        SMS_setBGScrollY(ScrollY);
        // Get row to update, 1 row after row 24
        unsigned short row = ((ScrollY + 200) >> 3);
        // Load map based on current player
        if (CurrentPlayer == INDEX_PLAYER_01) {
            SMS_loadTileMapArea(0, get_row(row), &P1Map[row << 5], 32, 1);
        } else {
            SMS_loadTileMapArea(0, get_row(row), &P2Map[row << 5], 32, 1);
        }
	} else if (dir == DIRECTION_UP) {
        // Set scroll amount, and the amount that entites will be affected by this scroll
        ScrollY -= amount;
        ScrollYAmount = amount;
        SMS_setBGScrollY(ScrollY);
        // Get row to update, 1 row after row 24
        unsigned short row = ScrollY >> 3;
        // Load map based on current player
        if (CurrentPlayer == INDEX_PLAYER_01) {
            SMS_loadTileMapArea(0, row, &P1Map[row << 5], 32, 1);
        } else {
            SMS_loadTileMapArea(0, row, &P2Map[row << 5], 32, 1);
        }
    }
}

// Update act number sprite
void update_act_number(unsigned char x, unsigned char y) {
    unsigned char number[] = { 0, 0, 164, METASPRITE_END };
    SMS_addMetaSprite(x, y, number);
}

// Updates clacker animation
void update_clacker(unsigned char x, unsigned char y) {
    // If initial setup
    if (ActClackerFrame == 0) {
        SMS_loadTileMapArea(x, y, ActTop_00, 4, 2);
        SMS_loadTileMapArea(x, y + 2, ActBottom, 4, 2);
        ActClackerFrame++;
        return;
    }
    // Draw frame
    switch (ActClackerFrame++) {
        case 1: SMS_loadTileMapArea(x, y, ActTop_01, 4, 2); break;
        case 2: SMS_loadTileMapArea(x, y, ActTop_02, 4, 2); break;
        default: SMS_loadTileMapArea(x, y, ActTop_00, 4, 2); break;
    }
}