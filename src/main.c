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

//#define OFFSET(x, y) ((y * 20) + x)
//#define POS2OFFSET(p) OFFSET(p.x, p.y)
#define SCREEN(x, y) (x * 20), ((y * 20) + 140)
#define POS2SCREEN(p) SCREEN(p.x, p.y)

typedef struct Pos {
	uint x;
	uint y;
} Pos;

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


#define GET_TILE(level, x, y) level.layout[(y * level.width) + x]
#define GET_TILE_P(level, p)  GET_TILE(level, p.x, p.y)

const static unsigned char compressedLevel[] = {BRICK, 1, VOID, 18, BRICK, 2, VOID, 18, BRICK, 2, VOID, 3, BRICK, 1, VOID, 7, BRICK, 1, VOID, 6, BRICK, 2, DOOR, 1, VOID, 2, BRICK, 1, VOID, 3, BRICK, 1, VOID, 1, BOX, 1, VOID, 1, BRICK, 1, VOID, 1, BOX, 1, VOID, 4, BRICK, 21, VOID, 0};
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

void findBoxes(Level level, Boxes* boxes) {
	size_t x, y;
	uint b = 0;

	for(y = 0; y < level.height; y++) {
		for(x = 0; x < level.width; x++) {
			if(level.layout[y * level.width + x] == BOX) {
				boxes->positions[b].x = x;
				boxes->positions[b].y = y;
				b++;
			}
		}
	}

	boxes->count = b;
}

int checkCollisions(Pos a, Pos* b, size_t len_b) {
	int collisions = 0;
	for(size_t i = 0; i < len_b; i++) {
		if(a.x == b[i].x && a.y == b[i].y)
			collisions++;
	}
	return collisions;
}

int findCollision(Pos a, Pos* b, size_t len_b) {
	for(size_t i = 0; i < len_b; i++) {
		if(a.x == b[i].x && a.y == b[i].y)
			return i;
	}
	return -1;
}

void renderBackground(Level level, C2D_Image* images) {
	for(size_t x = 0; x < level.width; x++) {
		for(size_t y = 0; y < level.height; y++) {
			unsigned char tile = GET_TILE(level, x, y);
			if(tile != VOID) {
				C2D_DrawImageAt(images[tile], SCREEN(x, y), -1.0, NULL, 20.0 / 24.0, 20.0 / 24.0);
			}
		}
	}
}

void renderSprites(C2D_Sprite guy, C2D_Sprite* boxes, uint box_count) {
	for(size_t i = 0; i < box_count; i++) {
		C2D_DrawSprite(&boxes[i]);
	}
	
	C2D_DrawSprite(&guy);
}

void move_guy(uint dir, Level level, Guy* guy, Boxes* boxes) {
	Pos next_pos = guy->position;

	next_pos.x += (dir == LEFT ? -1 : 1);
	if(checkCollisions(next_pos, boxes->positions, boxes->count) == 0 && GET_TILE_P(level, next_pos) != BRICK) {
		do {
			next_pos.y++;
		} while(checkCollisions(next_pos, boxes->positions, boxes->count) == 0 && GET_TILE_P(level, next_pos) != BRICK);

		guy->position.x = next_pos.x;
		guy->position.y = next_pos.y - 1;

		if(boxes->held != -1) {
			boxes->positions[boxes->held].x = guy->position.x;
			boxes->positions[boxes->held].y = guy->position.y - 1;
		}
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	Level level = {
		.layout = NULL,
		.width = 20,
		.height = 5
	};

	Guy guy = {
		.position = {
			.x = 17,
			.y = 3
		},
		.direction = LEFT
	};

	Boxes boxes;
	boxes.held = -1;

	C2D_SpriteSheet spriteSheet;
	C2D_Image images[4];
	C2D_Sprite guy_sprite;
	C2D_Sprite box_sprites[2];

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
	level.layout = uncompressLevel(compressedLevel, 20, 5);
	
	// Find boxes and remove them from level
	findBoxes(level, &boxes);
	for(size_t i = 0; i < boxes.count; i++) {
		GET_TILE_P(level, boxes.positions[i]) = VOID;
	}

	for(size_t i = 0; i < boxes.count; i++) {
		C2D_SpriteFromImage(&box_sprites[i], images[BOX]);
		C2D_SpriteSetPos(&box_sprites[i], POS2SCREEN(boxes.positions[i]));//box_pos[i].x * 20, (box_pos[i].y * 20) + 140);
		C2D_SpriteScale(&box_sprites[i], 20.0 / 24.0, 20.0 / 24.0);
	}

	C2D_SpriteFromSheet(&guy_sprite, spriteSheet, GUY);
	C2D_SpriteSetPos(&guy_sprite, guy.position.x * 20, (guy.position.y * 20) + 140);
	C2D_SpriteScale(&guy_sprite, -20.0 / 24.0, 20.0 / 24.0);
	
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
			if(guy.direction != LEFT) {
				C2D_SpriteScale(&guy_sprite, -1.0, 1.0);
				guy.direction = LEFT;
			}

			move_guy(LEFT, level, &guy, &boxes);
		}

		if(kUp & KEY_RIGHT) {
			if(guy.direction != RIGHT) {
				C2D_SpriteScale(&guy_sprite, -1.0, 1.0);
				guy.direction = RIGHT;
			}

			move_guy(RIGHT, level, &guy, &boxes);
		}

		if(kUp & KEY_UP) {
			Pos pos = {
				.x = guy.position.x + (guy.direction == LEFT ? -1 : 1),
				.y = guy.position.y
			};

			if(GET_TILE_P(level, pos) == BRICK || checkCollisions(pos, boxes.positions, boxes.count) > 0) {
				pos.y--;
				if(GET_TILE_P(level, pos) != BRICK && checkCollisions(pos, boxes.positions, boxes.count) == 0) {
					guy.position = pos;
					if(boxes.held != -1) {
						boxes.positions[boxes.held].x = guy.position.x;
						boxes.positions[boxes.held].y = guy.position.y - 1;
					}
				}
			}
		}

		if(kUp & KEY_A) {
			if(boxes.held == -1) {
				// Pick up box
				Pos pos = {
					.x = guy.position.x + (guy.direction == LEFT ? -1 : 1),
					.y = guy.position.y
				};

				boxes.held = findCollision(pos, boxes.positions, boxes.count);

				if(boxes.held != -1) {
					boxes.positions[boxes.held].x = guy.position.x;
					boxes.positions[boxes.held].y = guy.position.y - 1;
				}
			} else {
				// Drop box
				Pos pos = {
					.x = guy.position.x + (guy.direction == LEFT ? -1 : 1),
					.y = guy.position.y
				};

				if(GET_TILE_P(level, pos) == VOID) {
					boxes.positions[boxes.held] = pos;
					boxes.held = -1;
				}
			}
		}

		if(kUp & KEY_SELECT) {
			free(level.layout);

			// Uncompress the RLC level
			level.layout = uncompressLevel(compressedLevel, 20, 5);
			
			// Find boxes and remove them from level
			findBoxes(level, &boxes);
			for(size_t i = 0; i < boxes.count; i++) {
				GET_TILE_P(level, boxes.positions[i]) = VOID;
			}

			// Reset guy to beginning
			guy.position.x = 17;
			guy.position.y = 3;
		}

		//Update sprites position
		for(size_t i = 0; i < boxes.count; i++) {
			C2D_SpriteSetPos(&box_sprites[i], POS2SCREEN(boxes.positions[i]));
		}
		C2D_SpriteSetPos(&guy_sprite, POS2SCREEN(guy.position));

		//printf("\x1b[2;1H[Box 1] x: %03.3f, y: %03.3f\x1b[K", boxes[0].params.pos.x, boxes[0].params.pos.y);
		//printf("\x1b[3;1H[Box 2] x: %03.3f, y: %03.3f\x1b[K", boxes[1].params.pos.x, boxes[1].params.pos.y);
		//printf("\x1b[1;1H[ Guy ] x: %03.3f, y: %03.3f\x1b[K", guy.params.pos.x, guy.params.pos.y);

		printf("\x1b[1;1HLeft / Right: Move\x1b[K");
		printf("\x1b[2;1HUp: Climb\x1b[K");
		printf("\x1b[3;1HA: Pick up box\x1b[K");
		printf("\x1b[4;1HSelect: Reset level\x1b[K");
		printf("\x1b[5;1HStart: Quit\x1b[K");

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, C2D_Color32f(0.2f, 0.2f, 1.0f, 1.0f));
		C2D_SceneBegin(top);

		renderBackground(level, images);
		renderSprites(guy_sprite, box_sprites, boxes.count);

		C3D_FrameEnd(0);
	}

	// Free level
	free(level.layout);

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();

	return 0;
}
