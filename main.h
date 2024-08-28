#pragma once
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "SDL.h"
#include "SDL_image.h"

#define screen_width 533
#define screen_height 300
#define texture_width 64
#define texture_height 64

enum texture
{
	wall_ship_0,
	wall_ship_1,
	wall_ship_2,
	wall_ship_3,
	wall_ship_4,
	wall_ship_5,
	wall_ship_6,
	wall_ship_7,
	floor_desert,
	ceiling,
	well,
	barrel,
	obelisk,
	altar
};
struct controls
{
	bool move_up = false;
	bool move_down = false;
	bool move_left = false;
	bool move_right = false;
	double mouse_sensitivity = 100;
};
struct color
{
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 0;
};
struct vector2D
{
	double x = 0;
	double y = 0;
};
struct sprite
{
	vector2D pos;
	int texture;
};
struct entity
{
	vector2D pos;
	vector2D global_pos;
	vector2D direction = { -1, 0 };
	double rotation = 0;
	float speed = 3.5;
};

entity player;
controls player_controls;
vector2D plane = { 0, 0.66 }; // Camera plane

std::vector<uint32_t> frame_buffer(screen_height * screen_width);
size_t frame_buffer_size = screen_height * screen_width * sizeof(uint32_t);
std::vector<double> z_buffer(screen_width);

std::vector<SDL_Surface*> texture;
SDL_Color rgb;
uint32_t color;
uint32_t data;

uint32_t current_time = 0;
uint32_t old_time = 0;
double frame_time = 0;

const int16_t map_width = 24;
const int16_t map_height = 24;
std::vector<std::vector<uint8_t>> map
{
	{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
	{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	{4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	{4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
	{4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
	{4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
	{4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
	{4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	{4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
	{4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
	{4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
	{4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
	{6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
	{6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
	{4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	{4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
	{4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
	{4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
	{4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
	{4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

#define sprite_count 13
int sprite_order[sprite_count];
double sprite_distance[sprite_count];
sprite sprites[sprite_count] =
{
	{3, 10.5, well},
	{4.5, 22.5, barrel},
	{3.5, 22.5, barrel},
	{2.5, 22.5, barrel},
	{1.5, 22.5, barrel},
	{1.5, 15.5, obelisk},
	{4.5, 15.5, obelisk},
	{7.5, 22.5, altar},
	{9.5, 22.5, altar},
	{22.5, 11.5, altar},
	{13.5, 1.5, altar},
	{13.5, 22.5, altar},
};

int handle_input(SDL_Window*& window);
void raycastFloorAndCeiling();
void raycastWalls();
void raycastSprites();
void sort_sprites(int* order, double* dist, int amount);
void handle_player_movement();

Uint32 getPixel(SDL_Surface* surface, int x, int y);