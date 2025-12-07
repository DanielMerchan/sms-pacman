#include "libs/SMSlib.h"
#include "libs/PSGlib.h"

#include "banks/bank2.h"
#include "banks/bank3.h"
#include "banks/bank4.h"

#include "string.h"

#include "audio.h"
#include "bonus.h"
#include "common.h"
#include "defines.h"
#include "entity.h"
#include "game.h"
#include "ghost.h"
#include "globals.h"
#include "hud.h"
#include "player.h"
#include "scene_junior.h"
#include "scene_they_meet.h"
#include "scene_the_chase.h"
#include "stage.h"
#include "title.h"



void main(void) {
    while(1) {
        // PollInput();
        SMS_waitForVBlank();
        // PlayAudio();
        // FrameCounter++
        // Handle game state();
    }
}

// Header information
SMS_EMBED_SEGA_ROM_HEADER(777, 0);
SMS_EMBED_SDSC_HEADER(1, 0, 2025, 12, 7, "Daniel Merchan", "Ms Pac Man", "Action");
