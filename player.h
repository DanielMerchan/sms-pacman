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

// TO BE CONTINUED
//...