
void handle_game_state() {
    switch(GameState) {
        case GAME_STATE_NONE:
            SMS_displayOff();
            clear_VRAM();
            SMS_mapROMBank(black_bg_pal_bin_bank);
            SMS_loadBGPalette(black_bg_pal_bin);
            SMS_loadSpritePalette(black_bg_pal_bin);
            SMS_displayOn();
            GameState = GAME_STATE_TITLE;
            GameMode = GAME_MODE_ONE_PLAYER;
            break;
        case GAME_STATE_TITLE:
            if (PrevGameState != GameState) {
                set_title();
            } else {
                update_title();
            }
            break;
        case GAME_STATE_STAGE:
            if (PrevGameState != GameState) {
                set_stage();
            } else {
                update_stage();
            }
            break;
        case GAME_STATE_CUTSCENE_01:
            if (PrevGameState != GameState) {
                set_they_meet();
            } else {
                update_they_meet();
            }
            break;
        case GAME_STATE_CUTSCENE_02:
            if (PrevGameState != GameState) {
                set_the_chase();
            } else {
                update_the_chase();
            }
            break;
        case GAME_STATE_CUTSCENE_03:
            if (PrevGameState != GameState) {
                set_junior();
            } else {
                update_junior();
            }
            break;
    }
}