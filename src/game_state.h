#pragma once

#define MAX_ENTITIES (256*256)

struct game_state {
	struct map map;
	u32 num_entities;
	u32 next_entity_id;
	struct entity entities[MAX_ENTITIES];
};

#define GAME_ADD_ENTITY void game_add_entity(struct game_state *game_state, struct entity *entity)
GAME_ADD_ENTITY;

#define GAME_GET_ENTITY_BY_MOUSE \
	u32 game_get_entity_by_mouse(struct game_state *game_state, v2 mouse_pos)
GAME_GET_ENTITY_BY_MOUSE;

#define GAME_ENTITY_BY_ID struct entity *game_entity_by_id(struct game_state *game_state, u32 id)
GAME_ENTITY_BY_ID;
