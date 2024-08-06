#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}
//defining hitboxes
#define SCREEN_WIDTH	960
#define SCREEN_HEIGHT	680

#define block_height 30
#define block_width 60
#define ladder_height 30
#define ladder_width 50
#define eti_height 90

#define player_height 80
#define player_width 40

#define barrel_width 24

const double max_jump = 70;
//initalizing structures 
struct Platform
{
	double left_edge;
	double right_edge;
	double lower_edge;
	double upper_edge;
};

struct Ladder
{
	int left_edge;
	int right_edge;
	int lower_edge;
	int upper_edge;
};

struct Treasure {
	double left_edge;
	double right_edge;
	double lower_edge;
	double upper_edge;
	bool taken;
};

struct Map
{
	struct Ladder Ladders[32];
	struct Platform Platforms[32];
	struct Treasure treasure;

}map1, map2, map3;

struct xaxis
{
	double i;
};

struct yaxis
{
	double i;
};

struct Player
{
	SDL_Surface* bitmap;
	double x = SCREEN_WIDTH / 2;
	double y = SCREEN_HEIGHT - 10 - block_height - player_height / 2;
	struct xaxis xleft;
	struct xaxis xright;
	struct yaxis ydown;
	struct yaxis yup;
}player;

struct Barrel
{
	SDL_Surface* bitmap;
	double x = block_width / 2 + eti_height / 2;
	double y = 140 - eti_height;
	struct xaxis xleft;
	struct xaxis xright;
	struct yaxis ydown;
	struct yaxis yup;
}barrel;

void axis(Player* player)  //assigning the coordinates of edges od barrel
{
	player->xleft.i = player->x - player_width / 2;
	player->xright.i = player->x + player_width / 2;
	player->ydown.i = player->y + player_height / 2;
	player->yup.i = player->y - player_height / 2;
}

void axisB(Barrel* barrel)  //assigning the edges of barrel
{
	barrel->xleft.i = barrel->x - barrel_width / 2;
	barrel->xright.i = barrel->x + barrel_width / 2;
	barrel->ydown.i = barrel->y + barrel_width / 2;
	barrel->yup.i = barrel->y - barrel_width / 2;
}

// narysowanie napisu txt na powierzchni screen, zaczynaj�c od punktu (x, y)
// charset to bitmapa 128x128 zawieraj�ca znaki
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
// (x, y) to punkt �rodka obrazka sprite na ekranie
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	dest.x = x - sprite->w / 2;
	dest.y = y - sprite->h / 2;
	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o d�ugo�ci l w pionie (gdy dx = 0, dy = 1) 
// b�d� poziomie (gdy dx = 1, dy = 0)
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostok�ta o d�ugo�ci bok�w l i k
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

//printing different maps
void printMap1(struct SDL_Surface* screen, struct SDL_Surface* platform, struct SDL_Surface* ladder, SDL_Surface* bad_guy, SDL_Surface* bad_guy1, SDL_Surface* eti, double &BarrelTimer)
{
	if (BarrelTimer > 15.00 && BarrelTimer < 16.00)
	{
		DrawSurface(screen, bad_guy1, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	else
	{
		DrawSurface(screen, bad_guy, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	DrawSurface(screen, eti, block_width / 2 + eti_height / 2, 140 - eti_height / 2);
	for (int i = 0; i < 16; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2);
	}
	for (int i = 0; i < 13; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2 - 510);
		DrawSurface(screen, platform, SCREEN_WIDTH - block_width / 2 - i * block_width, SCREEN_HEIGHT - block_height / 2 - 340);
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2 - 170);
	}
	for (int i = 0; i < 4; i++)
	{
		DrawSurface(screen, ladder, 2 * block_width + ladder_width / 2, ladder_height / 2 + i * ladder_height);
	}
	for (int i = 0; i < 5; i++)
	{
		DrawSurface(screen, ladder, 11 * block_width + ladder_width / 2, SCREEN_HEIGHT - 540 + ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, 4 * block_width + ladder_width / 2, SCREEN_HEIGHT - 370 + ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, 10 * block_width + ladder_width / 2, SCREEN_HEIGHT - 200 + ladder_height / 2 + i * ladder_height);
	}
}

void printMap2(struct SDL_Surface* screen, struct SDL_Surface* platform, struct SDL_Surface* ladder, SDL_Surface* bad_guy, SDL_Surface* bad_guy1, SDL_Surface* eti, double& BarrelTimer)
{
	if (BarrelTimer > 15.00 && BarrelTimer < 16.00)
	{
		DrawSurface(screen, bad_guy1, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	else
	{
		DrawSurface(screen, bad_guy, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	for (int i = 0; i < 16; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2);
	}
	for (int i = 0; i < 14; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2 - 510);
		DrawSurface(screen, platform, SCREEN_WIDTH - block_width / 2 - i * block_width, SCREEN_HEIGHT - block_height / 2 - 310);
	}
	for (int i = 0; i < 4; i++)
	{
		DrawSurface(screen, ladder, SCREEN_WIDTH - (5 * block_width + 38 + ladder_width / 2), ladder_height / 2 + i * ladder_height);
	}
	for (int i = 0; i < 10; i++)
	{
		DrawSurface(screen, ladder, SCREEN_WIDTH - (block_width + ladder_width / 2), 360 - (ladder_height / 2 - i * ladder_height));
	}
	for (int i = 0; i < 9; i++)
	{
		DrawSurface(screen, ladder, 4 * block_width + ladder_width / 2, 140 + ladder_height / 2 + i * ladder_height);
	}
}

void printMap3(struct SDL_Surface* screen, struct SDL_Surface* platform, struct SDL_Surface* ladder, SDL_Surface* bad_guy, SDL_Surface* bad_guy1, SDL_Surface* eti, double& BarrelTimer)
{
	if (BarrelTimer > 15.00 && BarrelTimer < 16.00)
	{
		DrawSurface(screen, bad_guy1, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	else
	{
		DrawSurface(screen, bad_guy, 3 * block_width / 2 + 3 * eti_height / 2, 140 - eti_height / 2);
	}
	for (int i = 0; i < 16; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - block_height / 2);
	}
	for (int i = 0; i < 7; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - 480 - block_height / 2);
		DrawSurface(screen, platform, SCREEN_WIDTH - block_width / 2 - i * block_width, SCREEN_HEIGHT - 510 - block_height / 2);
	}
	for (int i = 0; i < 6; i++)
	{
		DrawSurface(screen, platform, block_width / 2 + i * block_width, SCREEN_HEIGHT - 170 - 3 * block_height / 2);
		DrawSurface(screen, platform, 6 * block_width - block_width / 2 + i * block_width, SCREEN_HEIGHT - 340 - block_height / 2);
		DrawSurface(screen, platform, SCREEN_WIDTH - block_width / 2 - i * block_width, SCREEN_HEIGHT - 170 - block_height / 2);
	}
	for (int i = 0; i < 4; i++)
	{
		DrawSurface(screen, ladder, SCREEN_WIDTH - (2 * block_width + 38 + ladder_width / 2), ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, 5 * block_width + ladder_width / 2, SCREEN_HEIGHT - 370 + ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, 6 * block_width + ladder_width / 2, SCREEN_HEIGHT - 510 + ladder_height / 2 + i * ladder_height);
	}
	for (int i = 0; i < 5; i++)
	{
		DrawSurface(screen, ladder, SCREEN_WIDTH - (5 * block_width + ladder_width / 2), SCREEN_HEIGHT - 370 + ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, SCREEN_WIDTH - (6 * block_width + ladder_width / 2), SCREEN_HEIGHT - 540 + ladder_height / 2 + i * ladder_height);
		DrawSurface(screen, ladder, SCREEN_WIDTH - ladder_width / 2, SCREEN_HEIGHT - 200 + ladder_height / 2 + i * ladder_height);
	}
	for (int i = 0; i < 6; i++)
	{
		DrawSurface(screen, ladder, ladder_width / 2, SCREEN_HEIGHT - 230 + ladder_height / 2 + i * ladder_height);
	}
}

void initalize_platform(struct Map* map, int i, int width, int height, int n)//initalizing platform coordinates
{
	map->Platforms[i].left_edge = width;
	map->Platforms[i].right_edge = width + n * block_width;
	map->Platforms[i].lower_edge = height;
	map->Platforms[i].upper_edge = height - block_height;
}

void initalize_ladder(struct Map* map, int i, int width, int height, int n)//initalizing ladder coordinates
{
	map->Ladders[i].left_edge = width;
	map->Ladders[i].right_edge = width + ladder_width;
	map->Ladders[i].lower_edge = height;
	map->Ladders[i].upper_edge = height - n * ladder_height;
}
//initalizing hitboxes of ladders and platforms of particular maps
void initalize_map1(struct Map* map1)
{
	initalize_platform(map1, 0, 0, SCREEN_HEIGHT, 16);
	initalize_platform(map1, 1, 0, SCREEN_HEIGHT - (block_height + 140), 13);
	initalize_platform(map1, 2, 3 * block_width, SCREEN_HEIGHT - (2 * block_height + 2 * 140), 13);
	initalize_platform(map1, 3, 0, SCREEN_HEIGHT - (3 * block_height + 3 * 140), 13);
	initalize_ladder(map1, 0, 2 * block_width, 4 * ladder_height, 4);
	initalize_ladder(map1, 1, 11 * block_width, 140 + 5 * ladder_height, 5);
	initalize_ladder(map1, 2, 4 * block_width, 310 + 5 * ladder_height, 5);
	initalize_ladder(map1, 3, 10 * block_width, 480 + 5 * ladder_height, 5);
	map1->treasure.left_edge = 60;
	map1->treasure.right_edge = 100;
	map1->treasure.lower_edge = SCREEN_HEIGHT - 30;
	map1->treasure.upper_edge = SCREEN_HEIGHT - 70;
}

void initalize_map2(struct Map* map2)
{
	initalize_platform(map2, 0, 0, SCREEN_HEIGHT, 16);
	initalize_platform(map2, 1, 2 * block_width , SCREEN_HEIGHT - (2 * block_height + 250), 14);
	initalize_platform(map2, 2, 0, SCREEN_HEIGHT - (3 * block_height + 3 * 140), 14);
	initalize_ladder(map2, 0, SCREEN_WIDTH - block_width - ladder_width, 330 + 10 * ladder_height, 10);
	initalize_ladder(map2, 1, 4 * block_width, 140 + 9 * ladder_height, 9);
	initalize_ladder(map2, 2, 19 * block_width / 2, 4 * ladder_height, 4);
}


void initalize_map3(struct Map* map3)
{
	initalize_platform(map3, 0, 0, SCREEN_HEIGHT, 16);
	initalize_platform(map3, 1, 0, SCREEN_HEIGHT - (2 * block_height + 140), 6);
	initalize_platform(map3, 2, SCREEN_WIDTH - 6 * block_width, SCREEN_HEIGHT - (block_height + 140), 6);
	initalize_platform(map3, 3, 5 * block_width, SCREEN_HEIGHT - (2 * block_height + 2 * 140), 6);
	initalize_platform(map3, 4, 0, SCREEN_HEIGHT - (2 * block_height + 3 * 140), 7);
	initalize_platform(map3, 5, SCREEN_WIDTH - 7 * block_width, SCREEN_HEIGHT - (3 * block_height + 3 * 140), 7);
	initalize_ladder(map3, 0, 0, SCREEN_HEIGHT - 2 * block_height - 20, 5);
	initalize_ladder(map3, 1, SCREEN_WIDTH - ladder_width, SCREEN_HEIGHT - block_height - 20, 5);
	initalize_ladder(map3, 2, 5 * block_width, SCREEN_HEIGHT - 2 * block_height - 160, 5);
	initalize_ladder(map3, 3, SCREEN_WIDTH - 5 * block_width - ladder_width, SCREEN_HEIGHT - 2 * block_height - 160, 5);
	initalize_ladder(map3, 4, 6 * block_width, 9 * ladder_height + 20, 4);
	initalize_ladder(map3, 5, SCREEN_WIDTH - 7 * block_width, 9 * ladder_height + 20, 5);
	initalize_ladder(map3, 6, SCREEN_WIDTH - 7 * block_width / 2, 4 * ladder_height, 4);
}

//brak wychodzenia za plansze
void next_stage(Map* map, Player* player, int& Stage, bool& newGame, int& playerScore, double delta,double &textTimer, double textInterval, SDL_Surface* screen, SDL_Surface* charset, bool &nextStage)
{
	char text[128];              //changing the stage if player goes up the ladder
	axis(player);
	if (player->ydown.i < 0)
	{
		Stage += 1;
		newGame = true;
		playerScore += 4000;
		nextStage = true;
	}
}

int checking_horizontal(SDL_Event* event)//checking player input for moving horizontally
{
	static bool LeftKey = false;
	static bool RightKey = false;

	switch (event->type)
	{

	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			LeftKey = true;
		}
		else if (event->key.keysym.sym == SDLK_RIGHT)
		{
			RightKey = true;
		}
		break;
	case SDL_KEYUP:
		if (event->key.keysym.sym == SDLK_LEFT)
		{
			LeftKey = false;
		}
		else if (event->key.keysym.sym == SDLK_RIGHT)
		{
			RightKey = false;
		}
		break;
	}

	if (LeftKey && RightKey)
	{
		return 0;
	}
	if (LeftKey)
	{
		return -1;
	}
	if (RightKey)
	{
		return 1;
	}
	return 0;
}

int checking_vertical(SDL_Event* event)  //checking input for moving vertically
{
	static bool UpKey = false;
	static bool DownKey = false;

	switch (event->type)
	{

	case SDL_KEYDOWN:
		if (event->key.keysym.sym == SDLK_UP)
		{
			UpKey = true;
		}
		else if (event->key.keysym.sym == SDLK_DOWN)
		{
			DownKey = true;
		}
		break;
	case SDL_KEYUP:
		if (event->key.keysym.sym == SDLK_UP)
		{
			UpKey = false;
		}
		else if (event->key.keysym.sym == SDLK_DOWN)
		{
			DownKey = false;
		}
		break;
	}

	if (UpKey && DownKey)
	{
		return 0;
	}
	if (UpKey)
	{
		return -1;
	}
	if (DownKey)
	{
		return 1;
	}
	return 0;
}

int check_platformB(Map* map, Barrel* barrel, bool falling) //checking platform for collisions with barrel, to stop them from falling
{
	axisB(barrel);
	for (int i = 0; i < 8; i++)
	{
		if (map->Platforms[i].upper_edge - barrel->ydown.i < 1 && map->Platforms[i].upper_edge - barrel->ydown.i > -1)
		{
			if (map->Platforms[i].upper_edge < barrel->ydown.i && map->Platforms[i].lower_edge > barrel->ydown.i)
			{
				barrel->ydown.i = map->Platforms[i].upper_edge;
			}
			if (map->Platforms[i].left_edge > barrel->xright.i)
			{
				return 1;
			}
			if (map->Platforms[i].right_edge < barrel->xleft.i)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 1;
};

int check_barrel1(Map* map, Barrel* barrel)//checking barrel for stage 1 and 3
{
	for (int i = 0; i < 4; i++)
	{
		if (map->Platforms[i].upper_edge - barrel->ydown.i < 1 && map->Platforms[i].upper_edge - barrel->ydown.i > -1 && i % 2 == 1)
		{
			return 1;
		}
		else if (map->Platforms[i].upper_edge - barrel->ydown.i < 1 && map->Platforms[i].upper_edge - barrel->ydown.i > -1)
		{
			return -1;
		}
	}
}

int check_barrel2(Map* map, Barrel* barrel) //checking barrel for stage 2
{
	for (int i = 0; i < 4; i++)
	{
		if (map->Platforms[i].upper_edge - barrel->ydown.i < 1 && map->Platforms[i].upper_edge - barrel->ydown.i > -1 && i % 2 == 1)
		{
			return -1;
		}
		else if (map->Platforms[i].upper_edge - barrel->ydown.i < 1 && map->Platforms[i].upper_edge - barrel->ydown.i > -1)
		{
			return 1;
		}
	}
}

void move_barrel(Map* map, Barrel* barrel, double delta, bool& barrel_falling, int n)//moving barrel
{
	if (check_platformB(map, barrel, barrel_falling) == 1)
	{
		barrel_falling = true;
	}
	else
	{
		barrel_falling = false;
		if (n == 1)
		{
			barrel->x += check_barrel1(map, barrel) * delta * 200;
		}
		else if (n == 2)
		{
			barrel->x += check_barrel2(map, barrel) * delta * 200;
		}
	}
}

int check_platform(Map* map, Player* player, bool falling)//checking for platform to move horizontally
{
	axis(player);
	for (int i = 0; i < 8; i++)
	{
		if (map->Platforms[i].upper_edge - player->ydown.i < 1 && map->Platforms[i].upper_edge - player->ydown.i > -1)
		{
			if (map->Platforms[i].upper_edge < player->ydown.i && map->Platforms[i].lower_edge > player->ydown.i)
			{
				player->ydown.i = map->Platforms[i].upper_edge;
			}
			if (map->Platforms[i].left_edge > player->xright.i)
			{
				return 1;
			}
			if (map->Platforms[i].right_edge < player->xleft.i)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	return 1;
};

int check_ladder(Map* map, Player* player)  //checking for ladder to move vertically
{
	axis(player);
	if (player->yup.i == 0)
	{
		return -1;
	}
	for (int i = 0; i < 8; i++)
	{
		if (map->Ladders[i].left_edge < player->x && map->Ladders[i].right_edge > player->x)
		{
			if (map->Ladders[i].lower_edge < player->yup.i)
			{
				return 0;
			}
			else if (map->Ladders[i].upper_edge - player->ydown.i > 1)
			{
				return 0;
			}
			else if (map->Ladders[i].upper_edge - player->ydown.i < 1 && map->Ladders[i].upper_edge - player->ydown.i >= 0)
			{
				return -1;
			}
			else if (map->Ladders[i].lower_edge + 20 <= player->ydown.i)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
	}
	return 0;
};

int uploading_bitmaps(struct SDL_Surface** screen, struct SDL_Texture** scrtex, struct SDL_Window** window, struct SDL_Renderer** renderer, struct SDL_Surface** charset,
	struct SDL_Surface** bad_guy, struct SDL_Surface** bad_guy1, struct SDL_Surface** player, struct SDL_Surface** platform, struct SDL_Surface** ladder, struct SDL_Surface** player_standing, struct SDL_Surface** player_right, struct SDL_Surface** player_left, struct SDL_Surface** player_jump, struct SDL_Surface** player_climbing, struct SDL_Surface** barrel1, struct SDL_Surface** eti,
	struct SDL_Surface** player_right1, struct SDL_Surface** player_left1, struct SDL_Surface** player_climbing1)
{
	*charset = SDL_LoadBMP("./cs8x8.bmp");                               //uploading bitmaps
	if (charset == NULL) {
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		SDL_FreeSurface(*screen);
		SDL_DestroyTexture(*scrtex);
		SDL_DestroyWindow(*window);
		SDL_DestroyRenderer(*renderer);
		SDL_Quit();
		return 1;
	};
	SDL_SetColorKey(*charset, true, 0x000000);

	*bad_guy = SDL_LoadBMP("./bad_guy.bmp");

	*bad_guy1 = SDL_LoadBMP("./bad_guy1.bmp");

	*player = SDL_LoadBMP("./player.bmp");

	*platform = SDL_LoadBMP("./platform.bmp");

	*ladder = SDL_LoadBMP("./ladder.bmp");

	*player_standing = SDL_LoadBMP("./player_standing.bmp");

	*player_right = SDL_LoadBMP("./player_right.bmp");

	*player_right1 = SDL_LoadBMP("./player_right1.bmp");

	*player_left = SDL_LoadBMP("./player_left.bmp");

	*player_left1 = SDL_LoadBMP("./player_left1.bmp");

	*player_jump = SDL_LoadBMP("./player_jump.bmp");

	*player_climbing = SDL_LoadBMP("./player_climbing.bmp");

	*player_climbing1 = SDL_LoadBMP("./player_climbing1.bmp");

	*barrel1 = SDL_LoadBMP("./barrel1.bmp");

	*eti = SDL_LoadBMP("./eti.bmp");
}

void printPlayer(struct SDL_Surface* screen, SDL_Surface* player1, Player player)//printing player
{
	DrawSurface(screen, player1, player.x, player.y);
}

void printBarrel(struct SDL_Surface* screen, SDL_Surface* barrel1, Barrel barrel)//printing barrel
{
	DrawSurface(screen, barrel1, barrel.x, barrel.y);
}

void movePlayerH(Player* player, SDL_Event* event, double delta, Map* map, bool& falling, SDL_Surface** player_right, SDL_Surface** player_left, SDL_Surface** player_right1, SDL_Surface** player_left1)
{
	axis(player);
	if (checking_horizontal(event) == 1 && player->x > SCREEN_WIDTH)      //moving player horizontally
	{
		return;
	}
	else if (checking_horizontal(event) == -1 && player->x < 0)
	{
		return;
	}
	if (check_platform(map, player, falling) == 1)
	{
		falling = true;
		player->x += checking_horizontal(event) * delta * 50;
	}
	else
	{
		falling = false;
		if (checking_horizontal(event) == 1 && int(player->x) % 40 <= 20)
		{
			player->bitmap = *player_right;
		}
		else if (checking_horizontal(event) == 1 && int(player->x) % 40 > 20)
		{
			player->bitmap = *player_right1;
		}
		else if (checking_horizontal(event) == -1 && int(player->x) % 40 >= 20)
		{
			player->bitmap = *player_left;
		}
		else if (checking_horizontal(event) == -1 && int(player->x) % 40 < 20)
		{
			player->bitmap = *player_left1;
		}
		player->x += checking_horizontal(event) * delta * 200;
	}
}

void movePlayerV(Player* player, SDL_Event* event, double delta, Map* map, bool& falling, bool& jumping, SDL_Surface** player_climbing, SDL_Surface** player_climbing1)
{
	if (check_ladder(map, player) == 1 && checking_vertical(event) == 1)  // moving player vertically
	{
		jumping = false;
		falling = false;
		return;
	}
	else if (check_ladder(map, player) == -1 && checking_vertical(event) == -1)
	{
		jumping = false;
		falling = false;
		return;
	}
	else if (check_ladder(map, player) == 0)
	{
		return;
	}
	else
	{
		jumping = false;
		falling = false;
		if (checking_vertical(event) == -1 || checking_vertical(event) == 1 && int(player->y) % 40 <= 20)
		{
			player->bitmap = *player_climbing;
		}
		else if (checking_vertical(event) == -1 || checking_vertical(event) == 1 && int(player->y) % 40 < 20)
		{
			player->bitmap = *player_climbing1;
		}
		else
		{
			player->bitmap = *player_climbing;
		}
		player->y += checking_vertical(event) * delta * 120;
	}
}

void gravitation(bool falling, double delta, Player* player, SDL_Surface* player_jump, Barrel* barrel, bool barrel_falling)
{
	if (falling)  
	{
		player->bitmap = player_jump;
		player->y += delta * 200;
	}
	if (barrel_falling)
	{
		barrel->y += delta * 200;
	}
}

void jump(bool* falling, double delta, Player* player, Map* map, bool& jumping, double* current_jump, SDL_Surface* player_jump)
{
	if (jumping)                                  //jumping
	{
		player->bitmap = player_jump;
		player->y -= 2;
		*current_jump += 1;
	}
	if (*current_jump >= max_jump)
	{
		*current_jump = 0;
		jumping = false;
		*falling = true;
		return;
	}
}

struct Map ChooseStage(Map* map1, Map* map2, Map* map3, int Stage, SDL_Surface* screen, SDL_Surface* platform, SDL_Surface* ladder, SDL_Surface* bad_guy, SDL_Surface* bad_guy1, SDL_Surface* barrel1, SDL_Surface* eti, double &BarrelTime)
{
	if (Stage == 1)                                           //choosing Stage by pressing numerical keys
	{
		initalize_map1(map1);
		printMap1(screen, platform, ladder, bad_guy, bad_guy1, eti, BarrelTime);
		return *map1;
	}
	else if (Stage == 2)
	{
		initalize_map2(map2);
		printMap2(screen, platform, ladder, bad_guy, bad_guy1, eti, BarrelTime);
		return *map2;
	}
	else if (Stage == 3)
	{
		initalize_map3(map3);
		printMap3(screen, platform, ladder, bad_guy, bad_guy1, eti, BarrelTime);
		return *map3;
	}
	else
	{
		initalize_map1(map1);
		printMap1(screen, platform, ladder, bad_guy, eti, bad_guy1, BarrelTime);
		return *map1;
	}
}

void new_game(Player* player, bool& newGame,double& barrelTimer,int& score, double& worldTime)// new Game
{
	if (newGame)
	{
		player->x = SCREEN_WIDTH / 2;
		player->y = SCREEN_HEIGHT - block_height - player_height / 2;
		axis(player);
		barrelTimer = 0.0;
		newGame = false;
		worldTime = 0.0;
	}
}

void barrels(Map* map, int& Stage, Barrel* barrel, double delta, bool& barrel_falling) //checking the stages, because barrels work differently
{
	if (Stage == 1)
	{
		move_barrel(map, barrel, delta, barrel_falling, 1);
	}
	if (Stage == 2)
	{
		move_barrel(map, barrel, delta, barrel_falling, 2);
	}
	if (Stage == 3)
	{
		move_barrel(map, barrel, delta, barrel_falling, 1);
	}
}

void createBarrel(Barrel* barrel) //initalizing barrel
{   
	barrel->x = block_width / 2 + eti_height / 2;
	barrel->y = 140 - eti_height;
	barrel->xleft.i = barrel->x - barrel_width / 2;
	barrel->xright.i = barrel->x + barrel_width / 2;
	barrel->ydown.i = barrel->y + barrel_width / 2;
	barrel->yup.i = barrel->y - barrel_width / 2;
}

void checkTreasure(struct Map* map, Player* player, int Stage, bool &taken, int &score, int &treasure) //detecting for treasure colliding with player
{
	if (Stage == 1)
	{
		map->treasure.left_edge = block_width / 2;
		map->treasure.right_edge = block_width / 2 + eti_height;
		map->treasure.lower_edge = 140;
		map->treasure.upper_edge = 140 - eti_height;
	}
	if (player->x < map->treasure.right_edge && player->x > map->treasure.left_edge)
	{
		if (player->y < map->treasure.lower_edge && player->y > map->treasure.upper_edge)
		{
			taken = true;
			treasure = 1;
		}
	}
	treasure = 0;
}

void BarrelCollision(Barrel* barrel, Player* player, int &score, bool &newGame, double &barrelTimer, bool &BarrelJump)//detecting if barrel collided with player
{
	if (barrel->xleft.i > player->xleft.i && barrel->xright.i < player->xright.i)
	{
		if (barrel->yup.i > player->yup.i && barrel->ydown.i <= player->ydown.i)
		{
			score -= 100;
			newGame = true;
		}
		if (barrel->yup.i - player->ydown.i < 80 && barrel->yup.i > player->ydown.i)
		{
			score += 100;
			BarrelJump = true;
		}
	}
}

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
	int t1, t2, quit, frames, rc;
	double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
	SDL_Event event;
	SDL_Surface* screen, * charset;
	SDL_Surface* bad_guy;
	SDL_Surface* bad_guy1;
	SDL_Surface* barrel1;
	SDL_Surface* platform;
	SDL_Surface* ladder;
	SDL_Surface* player1;
	SDL_Surface* player_standing;
	SDL_Surface* player_right;
	SDL_Surface* player_right1;
	SDL_Surface* player_left;
	SDL_Surface* player_left1;
	SDL_Surface* player_jump;
	SDL_Surface* player_climbing;
	SDL_Surface* player_climbing1;
	SDL_Surface* eti;
	SDL_Texture* scrtex;
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool Falling = true;
	bool jumping = false;
	bool newGame = false;
	bool barrel_falling = true;
	int Stage = 1;
	bool barrelCreated = false;
	double barrelTimer = 0.0;
	double textTimer = 0.0;
	const double barrelCreationInterval = 17.0;
	const double textInterval = 3.0;
	bool taken = false;
	bool nextStage = false;
	bool BarrelJump = false;
	int treasure = 0;
	double current_jump = 0;

	printf("wyjscie printfa trafia do tego okienka\n");
	printf("printf output goes here\n");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	// tryb pe�noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	};

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	SDL_SetWindowTitle(window, "King Donkey");


	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);


	// wy��czenie widoczno�ci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	uploading_bitmaps(&screen, &scrtex, &window, &renderer, &charset, &bad_guy, &bad_guy1, &player1, &platform, &ladder, &player_standing, &player_right, &player_left, &player_jump, &player_climbing, &barrel1, &eti, &player_right1, &player_left1, &player_climbing1);
	// wczytanie obrazka cs8x8.bmp
	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	frames = 0;
	fpsTimer = 0;
	fps = 0;
	quit = 0;
	worldTime = 0;
	distance = 0;
	etiSpeed = 1;

	int score = 0;

	player.bitmap = player_standing;

	while (!quit) {
		if (!jumping && !Falling)
			player.bitmap = player_standing;

		barrel.bitmap = barrel1;
		t2 = SDL_GetTicks();

		delta = (t2 - t1) * 0.001;
		t1 = t2;

		worldTime += delta;

		distance += etiSpeed * delta;

		SDL_FillRect(screen, NULL, czarny);


		barrelTimer += delta;
		if (barrelTimer >= barrelCreationInterval) {
			createBarrel(&barrel);
			barrelCreated = true;
			barrelTimer = 0.0;
		}
		struct Map map = ChooseStage(&map1, &map2, &map3, Stage, screen, platform, ladder, bad_guy, bad_guy1, barrel1, eti, barrelTimer);
		axis(&player);
		axisB(&barrel);
		movePlayerH(&player, &event, delta, &map, Falling, &player_right, &player_left, &player_right1, &player_left1);
		movePlayerV(&player, &event, delta, &map, Falling, jumping, &player_climbing, &player_climbing1);
		printPlayer(screen, player.bitmap, player);
		printBarrel(screen, barrel.bitmap, barrel);
		checkTreasure(&map, &player, Stage, taken, score, treasure);
		jump(&Falling, delta, &player, &map, jumping, &current_jump, player_jump);
		new_game(&player, newGame, barrelTimer, score, worldTime);
		barrels(&map, Stage, &barrel, delta, barrel_falling);
		BarrelCollision(&barrel, &player, score, newGame, barrelTimer, BarrelJump);
		next_stage(&map, &player, Stage, newGame, score, delta, textTimer, textInterval, screen, charset, nextStage);
		gravitation(Falling, delta, &player, player_jump, &barrel, barrel_falling);
		fpsTimer += delta;
		if (fpsTimer > 0.5) {
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		};

		barrelTimer += delta;
		if (barrelTimer >= barrelCreationInterval) {
			createBarrel(&barrel);
			barrelCreated = true;
			barrelTimer = 0.0;
		}
		if (taken)
		{
			textTimer += delta;
			if (textTimer <= textInterval)
			{
				sprintf(text, "+2500");
				DrawString(screen, player.x - 20, player.yup.i - 20, text, charset);
			}
			else 
			{
				taken = false;
			}
		}
		else if (nextStage)
		{
			textTimer += delta;
			if (textTimer <= textInterval)
			{
				sprintf(text, "+4000");
				DrawString(screen, player.x - 20, player.yup.i - 20, text, charset);
			}
			else
			{
				nextStage = false;
			}
		}
		else if (BarrelJump)
		{
			textTimer += delta;
			if (textTimer <= textInterval)
			{
				sprintf(text, "+100");
				DrawString(screen, player.x - 20, player.yup.i - 20, text, charset);
			}
			else
			{
				BarrelJump = false;
			}
		}
		if (treasure == 1)
		{
			score += 3000;
		}
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
		//            "template for the second project, elapsed time = %.1lf s  %.0lf frames / s"
		sprintf(text, "King Donkey, czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

		sprintf(text, "Esc - escape, \030 - move up, \031 - move down, \032 - move left, \033 - move right, n - new game");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		sprintf(text, "Player score: %d", score);
		DrawString(screen, screen->w / 2 - strlen(text) + 350, 10, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);

		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_SPACE && !Falling) jumping = true;
				else if (event.key.keysym.sym == SDLK_n) newGame = true;
				else if (event.key.keysym.sym == SDLK_1) Stage = 1;
				else if (event.key.keysym.sym == SDLK_2) Stage = 2;
				else if (event.key.keysym.sym == SDLK_3) Stage = 3;
				break;
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		};
		frames++;
	};

	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
};