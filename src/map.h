#pragma once

enum ground_type {
	GROUND_TYPE_WATER,
	GROUND_TYPE_SAND,
	GROUND_TYPE_GRASS,
};

#define MAX_MAP_WIDTH  256
#define MAX_MAP_HEIGHT MAX_MAP_WIDTH
#define MAX_MAP_SIZE   (MAX_MAP_WIDTH * MAX_MAP_HEIGHT)

struct map {
	u16 width, height;
	enum ground_type ground[MAX_MAP_SIZE];
};

#define MAP_GENERATE void map_generate(struct map *map, u16 width, u16 height)
MAP_GENERATE;

#define MAP_CREATE_ANIM_STATE void map_create_anim_state(struct map *map, struct anim_state *anim_state)
MAP_CREATE_ANIM_STATE;
