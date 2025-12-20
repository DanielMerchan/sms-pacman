// The chase
const unsigned short scene_the_chase_text[] = { 293, 284, 282, 279, 281, 284, 280, 292, 282 };
unsigned char scene_the_chase_hide[] = { 1, 1 };

// Reset hidden slots
void reset_the_chase_hidden(void) {
    scene_the_chase_hide[0] = 1;
    scene_the_chase_hide[1] = 1;
}

// Loads scene
void set_the_chase(void) {
    SMS_displayOff();
    // Clear VRAM, set all tiles to empty tile
    clear_VRAM();
    clear_screen(TILE_EMPTY);
    // Avoid grabage on the screen
    SMS_waitForVBlank();
    // Setup scene
	reset_global_variables();
	SMS_setSpriteMode (SPRITEMODE_TALL);
	SMS_useFirstHalfTilesforSprites(1);
    // Load palettes
    SMS_mapROMBank(title_bg_pal_bin_bank);
    SMS_loadBGPalette(title_bg_pal_bin);
	SMS_mapROMBank(main_spr_pal_bin_bank);
	SMS_loadSpritePalette(main_spr_pal_bin);
    // Bring in sprite tiles for sprites
	SMS_mapROMBank(sprite_tiles_bin_bank);
	SMS_loadTiles(sprite_tiles_bin, 8, sprite_tiles_bin_size);
    // Bring in the number 2 tile from stage tiles
	SMS_mapROMBank(stage_tiles_bin_bank);
	SMS_loadTiles(&stage_tiles_bin[160], 164, 32);
    // Bring in title tiles for text
	SMS_mapROMBank(title_tiles_bin_bank);
	SMS_loadTiles(title_tiles_bin, 279, title_tiles_bin_size);
    SMS_loadTileMapArea(14, 9, scene_the_chase_text, 9, 1);
    // Set pac man and ms pac man
	set_player(INDEX_PLAYER_01, ENTITY_PLAYER_01, DIRECTION_RIGHT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_PLAYER_01, 2, 0);
    set_player(INDEX_PLAYER_02, ENTITY_PAC_MAN, DIRECTION_RIGHT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_PLAYER_02, 2, 0);
    // Set up act clacker
    update_clacker(9, 7);
    // Reset hidden array
    reset_the_chase_hidden();
    // Done with setup
	PrevGameState = GameState;
    play_track(AUDIO_THE_CHASE, 0);
	SMS_displayOn();
}

// Accelerates sub speed
void accelerate_the_chase(unsigned char index) {
    // Increment sub speed when at timer threshold
    if (entity_increment_timer(index, 4) == 1) {
        EntitySubSpeed[index] += 7;
    }
}

// Update chased start up, when timer hits threshold
void set_chase_movement(unsigned char chased, unsigned short threshold) {
	// Wait to move chased
	if (increment_timer(threshold) == 1) {
		scene_the_chase_hide[chased] = 0;
		Timer = 0;
		SubState++;
	}
}

// Set movement behavior for given direction
void update_chase_movement(unsigned char direction) {
	// Local variables
	unsigned char chased = direction == DIRECTION_RIGHT ? INDEX_PLAYER_02 : INDEX_PLAYER_01;
	unsigned char chaser = direction == DIRECTION_RIGHT ? INDEX_PLAYER_01 : INDEX_PLAYER_02;
	unsigned char chasedSlot = direction == DIRECTION_RIGHT ? 1 : 0;
	unsigned char chaserSlot = direction == DIRECTION_RIGHT ? 0 : 1;
	unsigned char threshold = direction == DIRECTION_RIGHT ? 64 : 192;
	// Accelerate chaser
	if (scene_the_chase_hide[chaserSlot] == 0 && SubState <= 6) {
		accelerate_the_chase(chaser);
	}
	// Move chaser when chased is 8 tiles into screen
	if (scene_the_chase_hide[chaserSlot] == 1 && 
		((direction == DIRECTION_RIGHT && EntityX[chased] >= threshold) || (direction == DIRECTION_LEFT && EntityX[chased] <= threshold))) {
		scene_the_chase_hide[chaserSlot] = 0;
	}
	// When chased reaches the end of the screen
	if ((direction == DIRECTION_RIGHT && EntityX[chased] >= 248) || (direction == DIRECTION_LEFT && EntityX[chased] <= 8)) {
		// Hide chased
		scene_the_chase_hide[chasedSlot] = 1;
	}
	// When chaser reaches the end of the screen
	if ((direction == DIRECTION_RIGHT && EntityX[chaser] >= 248) || (direction == DIRECTION_LEFT && EntityX[chaser] <= 8)) {
		// Hide chaser
		scene_the_chase_hide[chaserSlot] = 1;
		// Set entity start positions for next direction
		unsigned char y = 0;
		switch (SubState) {
			case 2: y = 140; break;
			case 4: y = 124; break;
			case 6: y = 72; break;
			case 8: y = 156; break;
		}
		EntityX[chaser] = direction == DIRECTION_RIGHT ? 255 : 8;
		EntityY[chaser] = y;
		EntityDir[chaser] = direction == DIRECTION_RIGHT ? DIRECTION_LEFT : DIRECTION_RIGHT;
		EntityX[chased] = direction == DIRECTION_RIGHT ? 255 : 8;
		EntityY[chased] = y;
		EntityDir[chased] = direction == DIRECTION_RIGHT ? DIRECTION_LEFT : DIRECTION_RIGHT;
		if (SubState < 6) {
			set_entity_move_speed(chaser, 2, 0);
			set_entity_move_speed(chased, 2, 0);
		} else {
			set_entity_move_speed(chaser, 6, 0);
			set_entity_move_speed(chased, 6, 0);
		}
		SubState++;
	}
}

// Move the given entity
void move_the_chase_entity(unsigned char index) {
    // Move based on direction
    switch (EntityDir[index]) {
        case DIRECTION_RIGHT: EntityX[index] += get_entity_move_speed(index); break;
        case DIRECTION_DOWN: EntityY[index] += get_entity_move_speed(index); break;
        case DIRECTION_LEFT: EntityX[index] -= get_entity_move_speed(index); break;
        case DIRECTION_UP: EntityY[index] -= get_entity_move_speed(index); break;
    }
}

// Update scene entities
void update_the_chase_entities(void) {
	UNSAFE_SMS_copySpritestoSAT();
	SMS_initSprites();
	update_player_tiles();
	// If ms pac man is not hidden, process
	if (scene_the_chase_hide[0] == 0) {
		update_player(INDEX_PLAYER_01);
		move_the_chase_entity(INDEX_PLAYER_01);
	}
	// If pac man is not hidden, process
	if (scene_the_chase_hide[1] == 0) {
    	update_player(INDEX_PLAYER_02);
		move_the_chase_entity(INDEX_PLAYER_02);
	}
}

// Updates the chase scene
void update_the_chase(void) {
	// Update entities
	update_the_chase_entities();
	switch (SubState) {
		// Clacker
		case 0:
            // Act number sprite
            update_act_number(93, 79);
            // Update clacker animation every nth frame
            if (increment_timer(6) == 1) {
                // Update animation
                update_clacker(9, 7);
                // If 12th frame, next sub state
                if (ActClackerFrame == 12) {
					// Clear screen
					SMS_displayOff();
					clear_screen(256);
					clear_sprites();
					// Set entity start positions
					EntityX[INDEX_PLAYER_01] = 8;
					EntityY[INDEX_PLAYER_01] = 56;
					EntityX[INDEX_PLAYER_02] = 8;
					EntityY[INDEX_PLAYER_02] = 56;
					SMS_displayOn();
					Timer = 0;
				    SubState++;
                }
			}
		break;
        case 1: set_chase_movement(1, 210); break;
		case 2: update_chase_movement(DIRECTION_RIGHT); break;
		case 3: set_chase_movement(0, 120); break;
		case 4: update_chase_movement(DIRECTION_LEFT); break;
		case 5: set_chase_movement(1, 120); break;
		case 6: update_chase_movement(DIRECTION_RIGHT); break;
		case 7: set_chase_movement(0, 120); break;
		case 8: update_chase_movement(DIRECTION_LEFT); break;
		case 9: set_chase_movement(1, 10); break;
		case 10: update_chase_movement(DIRECTION_RIGHT); break;
		case 11:
		    // Wait a bit to move to next game state
            if (increment_timer(180) == 1) {
                set_player_next_stage();
                PrevGameState = 0;
                GameState = GAME_STATE_STAGE;
            }
		break;
	}
}