#include "lib\SMSlib.h"
#include "lib\PSGlib.h"

#include "banks\bank2.h"
#include "banks\bank3.h"
#include "banks\bank4.h"

#include "string.h"
#include "defines.h"
#include "globals.h"
#include "audio.h"
#include "common.h"
#include "hud.h"
#include "entity.h"
#include "ghost.h"
#include "bonus.h"
#include "player.h"
#include "title.h"
#include "scene_junior.h"
#include "scene_the_chase.h"
#include "scene_they_meet.h"
#include "stage.h"
#include "game.h"

void main(void) {
    play_track(AUDIO_READY, 1);
    while(1) {
        poll_input();
        SMS_waitForVBlank();
        play_audio();
        FrameCounter++;
        // Handle game state();
    }
}

// Header information
SMS_EMBED_SEGA_ROM_HEADER(777, 0);
SMS_EMBED_SDSC_HEADER(1, 0, 2025, 12, 7, "Daniel Merchan", "Ms Pac Man", "Action");
