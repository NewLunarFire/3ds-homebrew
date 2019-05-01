#include "../main.h"
#include "../level.h"

const static unsigned char compressedLevel[] = {BRICK, 1, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 3, BRICK, 1, VOID, 7, BRICK, 1, VOID, 6, BRICK, 2, DOOR, 1, VOID, 2, BRICK, 1, VOID, 3, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, GUY, 1, VOID, 2, BRICK, 21};

const LevelData level1 = {
    .data = compressedLevel,
    .width = 20,
    .height = 6,
};