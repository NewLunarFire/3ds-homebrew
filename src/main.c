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

const static char level[] = {BRICK, 1, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 3, BRICK, 1, VOID, 7, BRICK, 1, VOID, 6, BRICK, 2, DOOR, 1, VOID, 2, BRICK, 1, VOID, 3, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, BRICK, 1, VOID, 1, BOX, 1, VOID, 4, BRICK, 21};

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	char background[20][5];
	size_t p = 0, b = 0;
	size_t levelSize = sizeof(level) / sizeof(char);
	
	static C2D_SpriteSheet spriteSheet;
	C2D_Image images[4];
	C2D_Sprite guy;
	C2D_Sprite boxes[2];

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

	for(size_t i = 0; i < 4; i++) {
		images[i] = C2D_SpriteSheetGetImage(spriteSheet, i);
	}

	for(size_t i = 0; i < levelSize; i+=2) {
		char tile = level[i];

		if(tile == VOID || tile == BRICK || tile == DOOR) {
			for(size_t j = 0; j < level[i+1]; j++, p++) {
				size_t x = p % 20;
				size_t y = p / 20;
				
				background[x][y] = tile;
			}
		} else if(tile == BOX) {
			size_t x = p % 20;
			size_t y = p / 20;
			background[x][y] = VOID;
			C2D_SpriteFromSheet(&boxes[b], spriteSheet, BOX);
			C2D_SpriteSetPos(&boxes[b], x * 20, (y * 20) + 140);
			C2D_SpriteScale(&boxes[b], 20.0 / 24.0, 20.0 / 24.0);
			b++, p++;
		}
	}
	
	printf("[Box 1] x: %03u, y: %03u\n", boxes[0].params.pos.x, boxes[0].params.pos.y);
	printf("[Box 2] x: %03u, y: %03u\n", boxes[1].params.pos.x, boxes[1].params.pos.y);

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.2f, 0.2f, 1.0f, 1.0f));
		C2D_SceneBegin(top);

		for(size_t x = 0; x < 20; x++) {
			for(size_t y = 0; y < 5; y++) {
				char tile = background[x][y];
				if(tile != VOID) {
					C2D_DrawImageAt(images[tile], x * 20, (y * 20) + 140, 1.0, NULL, 20.0 / 24.0, 20.0 / 24.0);
				}
			}
		}

		C2D_DrawSprite(&boxes[0]);
		C2D_DrawSprite(&boxes[1]);

		C3D_FrameEnd(0);
	}

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}
