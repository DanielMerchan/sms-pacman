// They meet
const unsigned short scene_they_meet_text[] = { 293, 284, 282, 296, 279, 287, 282, 282, 293 };
const unsigned char heart[] = { 0, 0, 228, 8, 0, 230, METASPRITE_END };
unsigned char scene_they_meet_hide[] = { 0, 0, 0, 0, 1, 0 };

// Reset hidden slots
void reset_hidden(void) {
    scene_they_meet_hide[0] = 0;
    scene_they_meet_hide[1] = 0;
    scene_they_meet_hide[2] = 0;
    scene_they_meet_hide[3] = 0;
    scene_they_meet_hide[4] = 1;
    scene_they_meet_hide[5] = 0;
}

// Loads scene
void SetTheyMeet(void) {
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
    // Bring in cut scene tiles
	SMS_mapROMBank(cut_scene_tiles_bin_bank);
	SMS_loadTiles(cut_scene_tiles_bin, 228, cut_scene_tiles_bin_size);
    // Bring in the number 1 tile from stage tiles
	SMS_mapROMBank(stage_tiles_bin_bank);
	SMS_loadTiles(&stage_tiles_bin[128], 164, 32);
    // Bring in title tiles for text
	SMS_mapROMBank(title_tiles_bin_bank);
	SMS_loadTiles(title_tiles_bin, 279, title_tiles_bin_size);
    SMS_loadTileMapArea(14, 9, scene_they_meet_text, 9, 1);
    // Set pac man and ms pac man
	set_player(INDEX_PLAYER_01, ENTITY_PLAYER_01, DIRECTION_LEFT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_PLAYER_01, 1, 0);
    set_player(INDEX_PLAYER_02, ENTITY_PAC_MAN, DIRECTION_RIGHT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_PLAYER_02, 1, 0);
	set_ghost(INDEX_PINKY, DIRECTION_LEFT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_PINKY, 1, 0);
	set_ghost(INDEX_INKY, DIRECTION_RIGHT, ENTITY_STATE_SCENE, 256, 200);
    set_entity_move_speed(INDEX_INKY, 1, 0);
    // Set up act clacker
    update_clacker(9, 7);
    // Reset hidden array
    reset_hidden();
    // Done with setup
	PrevGameState = GameState;
    play_track(AUDIO_THEY_MEET, 0);
	SMS_displayOn();
}

// Accelerates ghost speed slowly
void accelerate_they_meet_ghosts(void) {
    // Increment sub speed when at timer threshold
    if (entity_increment_timer(INDEX_PINKY, 4) == 1) {
        EntitySubSpeed[INDEX_PINKY]++;
        EntitySubSpeed[INDEX_INKY]++;
    }
}

// Move the given entity
void move_they_meet_entity(unsigned char index) {
    // Move based on direction
    switch (EntityDir[index]) {
        case DIRECTION_RIGHT:
            // If bouncing, lock facing, move in opposite direction
            if (scene_they_meet_hide[5] == 1) {
                EntityX[index] -= get_entity_move_speed(index);
            // Normal movement
            } else {
                EntityX[index] += get_entity_move_speed(index);
            }
        break;
        case DIRECTION_DOWN: EntityY[index] += get_entity_move_speed(index); break;
        case DIRECTION_LEFT: 
            // If bouncing, lock facing, move in opposite direction
            if (scene_they_meet_hide[5] == 1) {
                EntityX[index] += get_entity_move_speed(index);
            // Normal movement
            } else {
                EntityX[index] -= get_entity_move_speed(index);
            }
        break;
        case DIRECTION_UP: EntityY[index] -= get_entity_move_speed(index); break;
    }
}

// Move all entities
void move_they_meet_entities(void) {
    move_they_meet_entity(INDEX_PLAYER_01);
    move_they_meet_entity(INDEX_PLAYER_02);
    move_they_meet_entity(INDEX_INKY);
    move_they_meet_entity(INDEX_PINKY);
}

// Update scene entities
void update_they_meet_entities(void) {
    UNSAFE_SMS_copySpritestoSAT();
	SMS_initSprites();
    // If not ready to show sprites, return
    if (SubState == 0) {
        return;
    }
	update_player_tiles();
    // Update sprites
    if (scene_they_meet_hide[0] == 0) {
	    update_player(INDEX_PLAYER_01);
    }
    if (scene_they_meet_hide[1] == 0) {
        update_player(INDEX_PLAYER_02);
    }
    if (scene_they_meet_hide[2] == 0) {
	    update_ghost(INDEX_PINKY);
    }
    if (scene_they_meet_hide[3] == 0) {
	    update_ghost(INDEX_INKY);
    }
    // Update heart sprite
    if (scene_they_meet_hide[4] == 0) {
        SMS_addMetaSprite(120, 24, heart);
    }
}

// Updates they meet scene
void update_they_meet(void) {
	// Update entities
	update_they_meet_entities();
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
				    SubState++;
                }
			}
		break;
        // Clear screen/sprites, setup entities
        case 1:
            // Clear screen
            SMS_displayOff();
            clear_screen(256);
            clear_sprites();
            // Set entity start positions
            EntityX[INDEX_PLAYER_01] = 255;
            EntityY[INDEX_PLAYER_01] = 140;
            EntityX[INDEX_PLAYER_02] = 8;
            EntityY[INDEX_PLAYER_02] = 56;
            SMS_displayOn();
            SubState++;
        break;
        // Set pinky and blinky
        case 2:
            // Move entities
            move_they_meet_entity(INDEX_PLAYER_01);
            move_they_meet_entity(INDEX_PLAYER_02);
            // If pac man hasn't reach 48 pixels
            if (EntityX[INDEX_PLAYER_02] < 48) {
                return;
            }
            // Set up ghosts
            EntityX[INDEX_PINKY] = 255;
            EntityY[INDEX_PINKY] = 140;
            EntityX[INDEX_INKY] = 8;
            EntityY[INDEX_INKY] = 56;
            SubState++;
        break;
        // Move entities acroos the screen
        case 3:
            // If all hidden
            if (scene_they_meet_hide[0] + scene_they_meet_hide[1] + scene_they_meet_hide[2] + scene_they_meet_hide[3] == 4) {
                SubState++;
                return;
            }
            // Hide entities based on position
            if (EntityX[INDEX_PLAYER_01] <= 8) {
                scene_they_meet_hide[0] = 1;
            } else {
                move_they_meet_entity(INDEX_PLAYER_01);
            }
            if (EntityX[INDEX_PINKY] <= 8) {
                scene_they_meet_hide[2] = 1;
            } else {
                move_they_meet_entity(INDEX_PINKY);
            }
            if (EntityX[INDEX_PLAYER_02] >= 248) {
                scene_they_meet_hide[1] = 1;
            } else {
                move_they_meet_entity(INDEX_PLAYER_02);
            }
            if (EntityX[INDEX_INKY] >= 248) {
                scene_they_meet_hide[3] = 1;
            } else {
                move_they_meet_entity(INDEX_INKY);
            }
            // Accelerate the ghost movement slowly
            accelerate_they_meet_ghosts();
        break;
        // Set up the meeting portion
        case 4:
            EntityX[INDEX_PLAYER_01] = 8;
            EntityY[INDEX_PLAYER_01] = 108;
            EntityDir[INDEX_PLAYER_01] = DIRECTION_RIGHT;
            EntityX[INDEX_PLAYER_02] = 248;
            EntityY[INDEX_PLAYER_02] = 108;
            EntityDir[INDEX_PLAYER_02] = DIRECTION_LEFT;
            scene_they_meet_hide[0] = 0;
            scene_they_meet_hide[1] = 0;
            SubState++;
        break;
        // Move entities across again
        case 5:
            // Move entities
            move_they_meet_entities();
            // If Ms Pace Man has moved 48 pixels in, set ghosts
            if (EntityX[INDEX_PLAYER_01] == 48) {
                EntityX[INDEX_INKY] = 248;
                EntityY[INDEX_INKY] = 108;
                EntityDir[INDEX_INKY] = DIRECTION_LEFT;
                EntityX[INDEX_PINKY] = 8;
                EntityY[INDEX_PINKY] = 108;
                EntityDir[INDEX_PINKY] = DIRECTION_RIGHT;
                scene_they_meet_hide[2] = 0;
                scene_they_meet_hide[3] = 0;
            // If half way through the screen, move up
            } else if (EntityX[INDEX_PLAYER_01] == 120) {
                EntityDir[INDEX_PLAYER_01] = DIRECTION_UP;
                EntityDir[INDEX_PLAYER_02] = DIRECTION_UP;
                SubState++;
            }
        break;
        // Move across screen, have ghosts collide
        case 6:
            // Move entities
            move_they_meet_entities();
            // If vertical position is 48
            if (EntityY[INDEX_PLAYER_01] == 48) {
                // Freeze frame
                EntityDir[INDEX_PLAYER_01] = DIRECTION_RIGHT;
                EntityDir[INDEX_PLAYER_02] = DIRECTION_LEFT;
                // Show heart
                scene_they_meet_hide[4] = 0;
                SubState++;
            }
            // If ghosts collide, setup bounce
            if (scene_they_meet_hide[5] == 0 && EntityX[INDEX_PINKY] >= 120) {
                EntitySubState[INDEX_PINKY] = ENTITY_STATE_BOUNCING;
                EntitySubState[INDEX_INKY] = ENTITY_STATE_BOUNCING;
                set_entity_move_speed(INDEX_PINKY, 0, 128);
                set_entity_move_speed(INDEX_INKY, 0, 128);
                scene_they_meet_hide[5] = 1;
                PSGStop();
            // If bouncing, allow for 2 bounces
            } else if (scene_they_meet_hide[5] == 1 && EntityX[INDEX_PINKY] <= 102) {
                scene_they_meet_hide[2] = 1;
                scene_they_meet_hide[3] = 1;
            }
        break;
        // Finished, wait a bit, go to next stage
        case 7:
            // Freeze frame pac man and ms pac man
            EntityFrame[INDEX_PLAYER_01] = 0;
            EntityFrame[INDEX_PLAYER_02] = 0;
            // Wait a bit to move to next game state
            if (increment_timer(120) == 1) {
                set_player_next_stage();
                PrevGameState = 0;
                GameState = GAME_STATE_STAGE;
            }
        break;
	}
}