#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define BACKGROUND_WIDTH 1024
#define BACKGROUND_HEIGHT 626

#define VOID	0
#define GUY		0
#define BOX		1
#define BRICK	2
#define DOOR	3
#define BACKGROUND 4

#define LEFT 1
#define RIGHT 0

#define GET_TILE(level, x, y) level.layout[(y * level.width) + x]
#define GET_TILE_P(level, p)  GET_TILE(level, p.x, p.y)

typedef struct Pos {
	uint x;
	uint y;
} Pos;

typedef struct Camera {
	int x;
	int y;
} Camera;

typedef struct Boxes {
	Pos positions[2];
	uint count;
	int held;
} Boxes;

typedef struct Guy {
	Pos position;
	uint direction;
} Guy;

typedef struct Level {
	unsigned char* layout;
	size_t width;
	size_t height;
} Level;

#endif