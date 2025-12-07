// Sprite index mapping:
// 0 = Blinky
// 1 = Pinky
// 2 = Inky
// 3 = Sue
// 4 = Bonus
// 5 = Player 1
// 6 = Player 2
// 7 = Misc (e.g., INDEX_PLAYER_01)
static unsigned char EntityType[8];             // The type of entity 
static unsigned short EntityTile[8];            // The tile index based on position
static unsigned short EntityPrevTile[8];        // The previous tile index based on position
static unsigned short EntityX[8];               // Current horizontal position in pixels
static unsigned short EntityY[8];               // Current vertical position in pixels
static signed char EntityPixelX[8];             // Horizontal tile pixel amount
static signed char EntityPixelX[8];             // Vertical tile pixel amount
static signed char EntityDistX[8];              // Horizontal distance to mid tile in pixels
static signed char EntityDistY[8];              // Vertical distance to mid tile in pixels
static unsigned char EntityDir[8];              // Entity state constant eg. DIRECTION_RIGHT
static unsigned char EntityState[8];            // Entity state constant eg. ENTITY_STATE_IDLE
static unsigned char EntitySubState[8];         // Entity sub state constant eg. ENTITY_STATE_ENERGIZED
static unsigned char EntitySpeed[8];            // Entity speed in pixels
static unsigned char EntitySubSpeed[8];         // Max sub speed to be applied to accumulate sub speed
static unsigned char EntityAccSubSpeed[8];      // Current accumulative sub pixel speed
static unsigned char EntityFrame[8];            // Current animation frame index
static signed char EntityFrameDuraration[8];    // The amount of time to show the current frame of animation
static unsigned short EntityTimer[8];           // Entity general use timer
static unsigned char EntityLutIndex[8];         // Look up table index
static unsigned char EntityTarget[8];           // Target entity
static unsigned char EntityTargetTile[8];       // Tile the entity is moving towards
static unsigned char EntityVisibility[8];       // Entity visibility type 0 - in view, 1 - offscreen, 2 = offscreen bottom

// Streaming tiles for players
static unsigned short PlayerFrame[2];