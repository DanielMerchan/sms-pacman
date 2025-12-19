// Player sprites
// First 4 tiles are reserve for the players
const unsigned char player_01_sprite[] = { 0, 0, 0, 8, 0, 2, METASPRITE_END };
const unsigned char player_02_sprite[] = { 0, 0, 4, 8, 0, 6, METASPRITE_END };

// Player 1 frames (Metasprite IDs)
const unsigned short player_01_right[] = { 0, 128, 256, 128 };
const unsigned short player_01_down[] = { 384, 512, 640, 512 };
const unsigned short player_01_left[] = { 768, 896, 1024, 896 };
const unsigned short player_01_up[] = { 1152, 1280, 1408, 1280 };
const unsigned short player_01_dying[] = { 512, 896, 1280, 128, 512, 896, 1280, 128, 512, 896, 1280 };
const unsigned short player_01_marker_down[] = { 4224 };
const unsigned short player_01_marker_up[] = { 4480 };

// Player 2 frames (Metasprite IDs), 1408 + 128 is the start of the next player
const unsigned short player_02_right[] = { 1536, 1664, 1792, 1664 };
const unsigned short player_02_down[] = { 1920, 2048, 2176, 2048 };
const unsigned short player_02_left[] = { 2304, 2432, 2560, 2432 };
const unsigned short player_02_up[] = { 2688, 2816, 2944, 2816 };
const unsigned short player_02_dying[] = { 2048, 2432, 2816, 1664, 2048, 2432, 2816, 1664, 2048, 2432, 2816 };
const unsigned short player_02_marker_down[] = { 4352 };
const unsigned short player_02_marker_up[] = { 4608 };

// Pac Man frames
const unsigned short pac_man_right[] = { 3072, 3200, 3328, 3200 };
const unsigned short pac_man_left[] = { 3456, 3584, 3712, 3584 };
const unsigned short pac_man_up[] = { 3840, 3968, 4096, 3968 };

// Resets all player-related global state (current player, default/each player's lives, stages, scores, pellet counts, start flags, ghost modes/timers). 
void reset_player_variables(void) {
	CurrentPlayer = INDEX_PLAYER_01;
	DefaultLives = 3;
	P1Lives = 3;
	P2Lives = 3;
	P1Stage = 0;
	P2Stage = 0;
	P1Score = 0;
	P2Score = 0;
	Stage = 0;
	P1PelletCount = 0;
	P2PelletCount = 0;
	P1Start = 1;
	P2Start = 1;
	P1GhostModeTimer = 0;
	P2GhostModeTimer = 0;
	P1GhostMode = ENTITY_STATE_SCATTERING;
	P2GhostMode = ENTITY_STATE_SCATTERING;
}

// Set player
void set_player(unsigned char index, unsigned char type, unsigned char dir, unsigned char state, unsigned short x, unsigned short y) {
    EntityX[index] = x;
	EntityY[index] = y;
	EntityType[index] = type;
    EntityState[index] = state;
	EntitySubState[index] = ENTITY_STATE_NONE;
	EntityDir[index] = dir;
	EntityTimer[index] = 0;
	EntityFrame[index] = 1;
	EntityFrameDuration[index] = -1;
}

// Streams player graphics into VRAM by mapping player_tiles_bin_bank and copying the appropriate player_tiles_bin frame(s) depending on the frame toggle. (Intended to alternate which player's tiles are written each frame so both players' tiles are available.) 
// Note: the if (FrameCounter & 1 == 1) expression intends to test odd frames.
void update_player_tiles(void) {
    SMS_mapROMBank(player_tiles_bin_bank);
	// Alternate drawing frames
	if (FrameCounter & 1 == 1) {
		UNSAFE_SMS_VRAMmemcpy128(0, &player_tiles_bin[PlayerFrame[0]]);
	} else {
		// If in team mode, current player is player 2, or if needed for something other than game play
		if (GameMode == GAME_MODE_TEAM_PLAY || CurrentPlayer == INDEX_PLAYER_02 || GameState != GAME_STATE_STAGE) {
			UNSAFE_SMS_VRAMmemcpy128(128, &player_tiles_bin[PlayerFrame[1]]);
		}
	}
}

// Updates player sprites
// If the entity is active, computes on-screen sprite coordinates from EntityX/Y and scroll state, applies scroll-offset exceptions (scroller vs auto-scroll), clamps markers for out-of-view top/bottom, and adds the correct metasprite (player_01_sprite or player_02_sprite) at the computed position.
void update_player_sprites(unsigned char index) {
	// If state is none, return
	if (EntityState[index] == ENTITY_STATE_NONE) {
		return;
	}
	// Set visibility
	set_entity_visibility(index);
	// Set sprite coordinates based on actual position
	unsigned char x = EntityX[index] - 8;
	unsigned char y = EntityY[index] - ScrollY - 8;
	// If this player is not the one causing the scrolling, add scroll offset
	if (Scroller != INDEX_NONE && Scroller != index) {
		y += ScrollYAmount;
	}
	// If out of view, on a game stage, and not auto scrolling, set as marker
	if (EntityVisibility[index] == OUT_OF_VIEW_TOP) {
		y = 0;
	} else if (EntityVisibility[index] == OUT_OF_VIEW_BOTTOM) {
		y = 176;
	}
	// Update sprite based on player
	if (index == INDEX_PLAYER_01) {
		SMS_addMetaSprite(x, y, player_01_sprite);
	} else {
		SMS_addMetaSprite(x, y, player_02_sprite);
	}
}

// Updates player animation frame
// Manages per-player animation timing and frame selection. 
// Respects team-play/paused conditions, decrements and resets EntityFrameDuration, advances EntityFrame, and sets the appropriate PlayerFrame[] (using direction/state-specific frame arrays). Handles DYING, DEAD, CHOMPED, IDLE, and default moving/scene frames, including off-screen marker frames.
void update_player_frame(unsigned char index) {
	// If team play and a player chomped or got chomped, don't process the other player's animation
	if (GameMode == GAME_MODE_TEAM_PLAY && Paused == 2 && EntityState[index] < ENTITY_STATE_DYING) {
		return;
	}
    // If not ready to update frame, return
    if (--EntityFrameDuration[index] > -1) {
		return;
	}
	// Increment frame based on state, reset if over max frame count
	EntityFrame[index] = EntityFrame[index] + 1 >= (EntityState[index] == ENTITY_STATE_DYING ? 11 : 4) ? 0 : EntityFrame[index] + 1;
	// Set player frame graphics
	switch (EntityState[index]) {
		case ENTITY_STATE_DEAD:
			EntityFrame[index] = 2;
		case ENTITY_STATE_DYING:
			// Reset duration
			EntityFrameDuration[index] = TIME_06;
			// Set frame of animation
			if (EntityType[index] == ENTITY_PLAYER_01) {
				PlayerFrame[0] = player_01_dying[EntityFrame[index]];
			} else {
				PlayerFrame[1] = player_02_dying[EntityFrame[index]];
			}
		break;
		// If chomped or idle, set to middle frame
		case ENTITY_STATE_CHOMPED:
		case ENTITY_STATE_IDLE:
			EntityFrame[index] = 1;
		// Default graphics
		default:
			// Reset duration
			EntityFrameDuration[index] = EntityState[index] == ENTITY_STATE_SCENE ? TIME_02 : TIME_01;
			switch (EntityType[index]) {
				// Player 1
				case ENTITY_PLAYER_01:
					// If off screen, set as marker, and return
					if (EntityVisibility[index] == OUT_OF_VIEW_TOP) {
						PlayerFrame[0] = player_01_marker_up[0];
						return;
					} else if (EntityVisibility[index] == OUT_OF_VIEW_BOTTOM) {
						PlayerFrame[0] = player_01_marker_down[0];
						return;
					}
					// Set graphic based on direction
					switch (EntityDir[index]) {
						case DIRECTION_RIGHT: PlayerFrame[0] = player_01_right[EntityFrame[index]]; return;
						case DIRECTION_DOWN: PlayerFrame[0] = player_01_down[EntityFrame[index]]; return;
						case DIRECTION_LEFT: PlayerFrame[0] = player_01_left[EntityFrame[index]]; return;
						case DIRECTION_UP: PlayerFrame[0] = player_01_up[EntityFrame[index]]; return;
					}
				break;
				// Player 2
				case ENTITY_PLAYER_02:
					// If off screen, set as marker, and return
					if (EntityVisibility[index] == OUT_OF_VIEW_TOP) {
						PlayerFrame[1] = player_02_marker_up[0];
						return;
					} else if (EntityVisibility[index] == OUT_OF_VIEW_BOTTOM) {
						PlayerFrame[1] = player_02_marker_down[0];
						return;
					}
					// Set graphic based on direction
					switch (EntityDir[index]) {
						case DIRECTION_RIGHT: PlayerFrame[1] = player_02_right[EntityFrame[index]]; return;
						case DIRECTION_DOWN: PlayerFrame[1] = player_02_down[EntityFrame[index]]; return;
						case DIRECTION_LEFT: PlayerFrame[1] = player_02_left[EntityFrame[index]]; return;
						case DIRECTION_UP: PlayerFrame[1] = player_02_up[EntityFrame[index]]; return;
					}
				break;
				// Pac man (Scenes)
				default:
					// Set graphic based on direction (Pac Man never faces down)
					switch (EntityDir[index]) {
						case DIRECTION_RIGHT: PlayerFrame[1] = pac_man_right[EntityFrame[index]]; return;
						case DIRECTION_LEFT: PlayerFrame[1] = pac_man_left[EntityFrame[index]]; return;
						case DIRECTION_UP: PlayerFrame[1] = pac_man_up[EntityFrame[index]]; return;
					}
				break;
			}
		break;
	}
}

// Sets current player's next stage
void set_player_next_stage(void) {
    if (CurrentPlayer == INDEX_PLAYER_01) {
        P1Stage++;
        P1PelletCount = 0;
        P1GhostMode = ENTITY_STATE_SCATTERING;
        P1GhostModeTimer = 0;
    } else {
        P2Stage++;
        P2PelletCount = 0;
        P2GhostMode = ENTITY_STATE_SCATTERING;
        P2GhostModeTimer = 0;
    }
}

// Changes screen BG tile and map tile, sets pellet count and checks for stage clear
void player_eat_pellet(unsigned char index) {
	// If pellet count reached 70 or 170 and it hasn't been rolled out yet, roll out the bonus
	if (EntityState[INDEX_BONUS] == ENTITY_STATE_NONE && (PelletCount == 70 || PelletCount == 170)) {
		BonusTypeIndex = PelletCount >= 170 ? 1 : 0;
		set_bonus(ENTITY_STATE_ENTERING);
	}
	// Set pellet tile empty in map and vram
	set_map_tile(index, TILE_EMPTY);
	// Increase player 2 pellet count, if in 2 player mode
	if (GameMode == GAME_MODE_TWO_PLAYER && CurrentPlayer == INDEX_PLAYER_02) {
		P2PelletCount++;
		PelletCount = P2PelletCount;
	// Increase player one pellet count
	} else {
		P1PelletCount++;
		PelletCount = P1PelletCount;
	}
	play_sfx(SFX_PELLET, SFX_CHANNEL2);
	// Stage cleared
	if (PelletCount >= PelletCounts[Stage]) {
		// Stop everything
		PSGStop();
		PSGSFXStop();
		Paused = 1;
		EntityState[index] = ENTITY_STATE_IDLE;
		// Reset for flash timing
		Accumulator = 0;
		// Set to stage clear
		SubState = 6;
	}
}

// Checks for pellet tile for the given player
unsigned char player_check_pellet(unsigned char index, unsigned short tile) {
	// If pellet tile
	if (tile == TILE_PELLET) {
		add_score(index, POINTS_10);
		player_eat_pellet(index);
		return 1;
	// If energizer tile
	} else if (tile == TILE_ENERGIZER) {
		EnergizerTimer = GhostFrightenedDurations[Stage];
		EnergizerTimerHalf = EnergizerTimer >> 1;
		GhostPointAcc = 0;
		add_score(index, POINTS_50);
		// If the frightened duration is greater than zero, play energizer sound
		if (GhostFrightenedDurations[Stage] > 0) {
			play_track(AUDIO_ENERGIZER, 1);
		}
		player_eat_pellet(index);
		set_all_ghost_states(ENTITY_STATE_FRIGHTENED);
		return 1;
	}
	return 0;
}

// Return sign of a number if it's over, under, or equal to zero
signed char get_sign(signed char num) {
	// If over zero, positive number
	if (num > 0) {
		return 1;
	// If less than zero negative number
	} else if (num < 0) {
		return -1;
	}
	// Zero
	return 0;
}

// Set inner tile pixel position for given entity
void set_tile_pixels(unsigned char index) {
	// Mod current pixel position to get tile pixel position
    EntityPixelX[index] = EntityX[index] & 7;
    EntityPixelY[index] = EntityY[index] & 7;
	// Reset horizontal pixel if under zero
    if (EntityPixelX[index] < 0) {
        EntityPixelX[index] += 8;
    }
	// Reset vertical pixel if under zero
    if (EntityPixelY[index] < 0) {
        EntityPixelY[index] += 8;
    }
    EntityDistX[index] = 4 - EntityPixelX[index];
    EntityDistY[index] = 4 - EntityPixelY[index];
}

// Moves the given player in the given direction
// Returns speed of movement
unsigned char move_entity_player(unsigned char index, unsigned char dir) {
	// set_tile_pixels tile pixel amounts
	set_tile_pixels(index);
	// Check if we can move in the desired direction
	unsigned char canMove = entity_can_move(EntityTile[index], EntityDir[index]);
	// Get speed
	unsigned char speed = get_entity_move_speed(index);
	// Try and move entity, apply corner cutting
	if (dir == DIRECTION_RIGHT && (canMove == 1 || EntityDistX[index] > 0)) {
		EntityX[index] += speed;
		EntityY[index] += get_sign(EntityDistY[index]);
	} else if (dir == DIRECTION_DOWN && (canMove == 1 || EntityDistY[index] > 0)) {
		EntityY[index] += speed;
		EntityX[index] += get_sign(EntityDistX[index]);
	} else if (dir == DIRECTION_LEFT && (canMove == 1 || EntityDistX[index] < 0)) {
		EntityX[index] -= speed;
		EntityY[index] += get_sign(EntityDistY[index]);
	} else if (dir == DIRECTION_UP && (canMove == 1 || EntityDistY[index] < 0)) {
		EntityY[index] -= speed;
		EntityX[index] += get_sign(EntityDistX[index]);
	} else {
		return 0;
	}
	return speed;
}

// Moves player based on desired direction
void move_player(unsigned char index, unsigned char dir) {
	// Unlock vertical scroll lock
	ScrollLock = 0;
	// If paused or if pellet consumed
	if (Paused > 0 || player_check_pellet(index, get_map_tile_id(EntityTile[index])) == 1) {
		// If scrolling, lock in vertical scroll amount, so that it does not get reset
		if (ScrollYAmount != 0) {
			ScrollLock = 1;
		}
		return;
	}
	// Check moving in the desired direction, if available, set player to desired direction
	// Else we're moving in current direction
	if (entity_can_move(EntityTile[index], dir) == 1) {
		EntityDir[index] = dir;
	}
	// If we are not moving in set direction, set player to idle, return
	if (move_entity_player(index, EntityDir[index]) == 0) {
		EntityState[index] = ENTITY_STATE_IDLE;
		return;
	}
	// We are moving
	EntityState[index] = ENTITY_STATE_MOVING;
	// Check for tunnel wrap
	check_wrap(index);
}

#ifdef TIME_NTSC
	// Sets player speed
	void set_player_speed(unsigned char index) {
		// If not in actual game play, return
		if (GameState != GAME_STATE_STAGE) {
			return;
		}
		// If pac boost enabled, set double speed and return
		if (PacBoost == 1) {
			EntitySpeed[index] = 2;
			EntitySubSpeed[index] = 0;
			return;
		// Normal speed
		} else {
			EntitySpeed[index] = 1;
		}
		// Set sub pixel speed
		// If player energized
		if (EnergizerTimer > 0) {
			// lvl 1: 32, lvls 2-4: 48, lvls 5-20: 64, lvls 21+: 32
			if (Stage == 0) {
				EntitySubSpeed[index] = 32;
			} else if (Stage < 4) {
				EntitySubSpeed[index] = 48;
			} else if (Stage < 20) {
				EntitySubSpeed[index] = 64;
			} else {
				EntitySubSpeed[index] = 32;
			}
		// Normal state
		} else {
			// lvl 1: 0, lvls 2-4: 32, lvls 5-20: 64, lvls 21+: 32
			if (Stage == 0) {
				EntitySubSpeed[index] = 32;
			} else if (Stage < 4) {
				EntitySubSpeed[index] = 32;
			} else if (Stage < 20) {
				EntitySubSpeed[index] = 64;
			} else {
				EntitySubSpeed[index] = 32;
			}
		}
	}
#else
	// Sets player speed
	void set_player_speed(unsigned char index) {
		// If not in actual game play, return
		if (GameState != GAME_STATE_STAGE) {
			return;
		}
		// If pac boost enabled, set double speed and return
		if (PacBoost == 1) {
			EntitySpeed[index] = 2;
			EntitySubSpeed[index] = 0;
			return;
		// Normal speed
		} else {
			EntitySpeed[index] = 1;
		}
		// Set sub pixel speed
		// If player energized
		if (EnergizerTimer > 0) {
			// lvl 1: 32, lvls 2-4: 48, lvls 5-20: 64, lvls 21+: 32
			if (Stage == 0) {
				EntitySubSpeed[index] = 32;
			} else if (Stage < 4) {
				EntitySubSpeed[index] = 48;
			} else if (Stage < 20) {
				EntitySubSpeed[index] = 64;
			} else {
				EntitySubSpeed[index] = 32;
			}
		// Normal state
		} else {
			// lvl 1: 0, lvls 2-4: 32, lvls 5-20: 64, lvls 21+: 32
			if (Stage == 0) {
				EntitySubSpeed[index] = 0;
			} else if (Stage < 4) {
				EntitySubSpeed[index] = 32;
			} else if (Stage < 20) {
				EntitySubSpeed[index] = 64;
			} else {
				EntitySubSpeed[index] = 32;
			}
		}
	}
#endif

// Updates player state
void update_player_state(unsigned char index) {
	// Set player previous and current tile index
	EntityPrevTile[index] = EntityTile[index];
	EntityTile[index] = position_to_tile(EntityX[index], EntityY[index]);
	// Set player speed
	set_player_speed(index);
	// Do process based on state
	switch (EntityState[index]) {
		case ENTITY_STATE_NONE:
		case ENTITY_STATE_SCENE:
			return;
		break;
		// Wait for a bit, hide all entities except for this entity, set player to dying
		case ENTITY_STATE_CHOMPED:
			SubState = 5;
			// Auto scroll down to get player into view
			if (AutoScrollDir == DIRECTION_DOWN && ScrollY < 96) {
				Scroller = INDEX_AUTO_SCROLL;
				update_row_safe(DIRECTION_DOWN, 1);
			// Auto scroll up to get player into view
			} else if (AutoScrollDir == DIRECTION_UP && ScrollY > 0) {
				Scroller = INDEX_AUTO_SCROLL;
				update_row_safe(DIRECTION_UP, 1);
			// Player in view
			} else {
				if (entity_increment_timer(index, TIME_60) == 1) {
					set_all_ghost_states(ENTITY_STATE_NONE);
					EntityState[INDEX_BONUS] = ENTITY_STATE_NONE;
					if (index == INDEX_PLAYER_01) {
						EntityState[INDEX_PLAYER_02] = ENTITY_STATE_NONE;
					} else {
						EntityState[INDEX_PLAYER_01] = ENTITY_STATE_NONE;
					}
					EntityState[index] = ENTITY_STATE_DYING;
					EntityFrameDuration[index] = -1;
					EntityFrame[index] = 255;
					play_track(AUDIO_PLAYER_CHOMPED, 0);
				}
			}
		break;
		// Wait for animation to end, set player to dead
		case ENTITY_STATE_DYING:
			if (EntityFrame[index] == 10) {
				EntityState[index] = ENTITY_STATE_DEAD;
			}
		break;
		// Wait for a bit, then reset the stage
		case ENTITY_STATE_DEAD:
			if (entity_increment_timer(index, TIME_40) == 1) {
				FrameCounter = 0;
				SubState = 9;
			}
		break;
		// Everything else
		default:
			if (index == INDEX_PLAYER_01) {
				if (KeysHeld & PORT_A_KEY_RIGHT) {
					move_player(index, DIRECTION_RIGHT);
				} else if (KeysHeld & PORT_A_KEY_DOWN) {
					move_player(index, DIRECTION_DOWN);
				} else if (KeysHeld & PORT_A_KEY_LEFT) {
					move_player(index, DIRECTION_LEFT);
				} else if (KeysHeld & PORT_A_KEY_UP) {
					move_player(index, DIRECTION_UP);
				} else {
					move_player(index, EntityDir[index]);
				}
			} else {
				if (KeysHeld & PORT_B_KEY_RIGHT) {
					move_player(index, DIRECTION_RIGHT);
				} else if (KeysHeld & PORT_B_KEY_DOWN) {
					move_player(index, DIRECTION_DOWN);
				} else if (KeysHeld & PORT_B_KEY_LEFT) {
					move_player(index, DIRECTION_LEFT);
				} else if (KeysHeld & PORT_B_KEY_UP) {
					move_player(index, DIRECTION_UP);
				} else {
					move_player(index, EntityDir[index]);
				}
			}
		break;
	}
}

// Updates player
void update_player(unsigned char index) {
	update_player_sprites(index);
	update_player_state(index);
	update_player_frame(index);
}