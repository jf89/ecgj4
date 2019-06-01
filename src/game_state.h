#pragma once

#define MAX_ENTITIES (256*256)

struct game_state {
	struct map map;
	u32 num_entities;
	struct entity entities[MAX_ENTITIES];
};

#define GAME_ADD_ENTITY void game_add_entity(struct game_state *game_state, struct entity *entity)
GAME_ADD_ENTITY;
