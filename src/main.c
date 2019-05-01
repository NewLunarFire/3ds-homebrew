// Block Dude for TI-83 clone for 3DS Homebrew
// Tommy Savaria, 2019
#include <citro2d.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SPRITES   768
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#define VOID	0
#define GUY		0
#define BOX		1
#define BRICK	2
#define DOOR	3

#define LEFT 1
#define RIGHT 0

#define OFFSET(x, y) ((y * 20) + x)

typedef struct {
	uint x;
	uint y;
} Pos;

const static unsigned char level[] = {BRICK, 1, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 3, BRICK, 1, VOID, 7, BRICK, 1, VOID, 6, BRICK, 2, DOOR, 1, VOID, 2, BRICK, 1, VOID, 3, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, BRICK, 1, VOID, 1, BOX, 1, VOID, 4, BRICK, 21, VOID, 0};
const size_t level_width = 20;
const size_t level_height = 5;

unsigned char* uncompressLevel(const unsigned char* level, uint width, uint height) {
	size_t i, j, k;
	unsigned char* result = malloc(sizeof(char) * width * height);

	for(i = 0, k = 0; level[i+1] != 0; i+=2) {
		for(j = 0; j < level[i+1]; j++, k++)
			result[k] = level[i];
	}
	
	return result;
}

uint findBoxes(unsigned char* level, Pos* boxes, size_t width, size_t height) {
	size_t x, y;
	uint b = 0;

	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			if(level[y * width + x] == BOX) {
				boxes[b].x = x;
				boxes[b].y = y;
				b++;
			}
		}
	}

	return b;
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	//unsigned char background[20][5];
	unsigned char *background;
	uint box_count;
	uint guy_dir = LEFT;
	int box_held = -1;

	C2D_SpriteSheet spriteSheet;
	C2D_Image images[4];
	C2D_Sprite guy;
	C2D_Sprite boxes[2];

	Pos box_pos[2];
	Pos guy_pos = { .x = 17, .y = 3 };

	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Load images
	for(size_t i = 0; i < 4; i++) {
		images[i] = C2D_SpriteSheetGetImage(spriteSheet, i);
	}

	// Uncompress the RLC level
	background = uncompressLevel(level, 20, 5);
	
	// Find boxes and remove them from level
	box_count = findBoxes(background, box_pos, 20, 5);
	for(size_t i = 0; i < box_count; i++) {
		background[OFFSET(box_pos[i].x, box_pos[i].y)] = VOID;
	}

	for(size_t i = 0; i < box_count; i++) {
		C2D_SpriteFromImage(&boxes[i], images[BOX]);
		C2D_SpriteSetPos(&boxes[i], box_pos[i].x * 20, (box_pos[i].y * 20) + 140);
		C2D_SpriteScale(&boxes[i], 20.0 / 24.0, 20.0 / 24.0);
	}

	C2D_SpriteFromSheet(&guy, spriteSheet, GUY);
	C2D_SpriteSetPos(&guy, guy_pos.x * 20, (guy_pos.y * 20) + 140);
	C2D_SpriteScale(&guy, -20.0 / 24.0, 20.0 / 24.0);
	
	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		u32 kUp = hidKeysUp();
		if(kUp & KEY_LEFT) {
			if(!(box_pos[0].x == (guy_pos.x -1) || box_pos[1].x == (guy_pos.x -1) || background[OFFSET(guy_pos.x -1, guy_pos.y)] == BRICK)) {
				if(guy_pos.x > 0) {
					guy_pos.x--;
					C2D_SpriteSetPos(&guy, guy_pos.x * 20, (guy_pos.y * 20) + 140);
				}	
			}

			if(guy_dir == RIGHT) {
				C2D_SpriteScale(&guy, -1.0, 1.0);
				guy_dir = LEFT;
			}
		}

		if(kUp & KEY_RIGHT) {
			if(!(box_pos[0].x == (guy_pos.x +1) || box_pos[1].x == (guy_pos.x +1) || background[OFFSET(guy_pos.x +1, guy_pos.y)] == BRICK)) {
				if(guy_pos.x < 19) {
					guy_pos.x++;
					C2D_SpriteSetPos(&guy, guy_pos.x * 20, (guy_pos.y * 20) + 140);
				}
			}

			if(guy_dir == LEFT) {
				C2D_SpriteScale(&guy, -1.0, 1.0);
				guy_dir = RIGHT;
			}
		}

		if(kUp & KEY_A) {
			if(box_held == -1) {
				// Pick up box
				uint x = guy_pos.x + (guy_dir == LEFT ? -1 : 1);
				uint y = guy_pos.y;

				if(box_pos[0].x == x && box_pos[0].y == y) {
					box_held = 0;
				} else if(box_pos[1].x == x && box_pos[1].y == y) {
					box_held = 1;
				}

				if(box_held != -1) {
					box_pos[box_held].x = guy_pos.x;
					box_pos[box_held].y = guy_pos.y - 1;
				}

				C2D_SpriteSetPos(&boxes[box_held], box_pos[box_held].x * 20, (box_pos[box_held].y * 20) + 140);
			}
		}

		printf("\x1b[2;1H[Box 1] x: %03.3f, y: %03.3f\x1b[K", boxes[0].params.pos.x, boxes[0].params.pos.y);
		printf("\x1b[3;1H[Box 2] x: %03.3f, y: %03.3f\x1b[K", boxes[1].params.pos.x, boxes[1].params.pos.y);
		printf("\x1b[1;1H[ Guy ] x: %03.3f, y: %03.3f\x1b[K", guy.params.pos.x, guy.params.pos.y);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.2f, 0.2f, 1.0f, 1.0f));
		C2D_SceneBegin(top);

		for(size_t x = 0; x < 20; x++) {
			for(size_t y = 0; y < 5; y++) {
				unsigned char tile = background[OFFSET(x, y)];
				if(tile != VOID) {
					C2D_DrawImageAt(images[tile], x * 20, (y * 20) + 140, -1.0, NULL, 20.0 / 24.0, 20.0 / 24.0);
				}
			}
		}

		for(size_t i = 0; i < box_count; i++) {
			C2D_DrawSprite(&boxes[i]);
		}
		C2D_DrawSprite(&guy);

		C3D_FrameEnd(0);
	}

	// Free level
	free(background);

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();

	return 0;
}
