GAME_ADD_ENTITY {
	u32 idx = game_state->num_entities++;
	entity->id = game_state->next_entity_id++;
	game_state->entities[idx] = *entity;
}

GAME_GET_ENTITY_BY_MOUSE {
	for (u32 i = 0; i < game_state->num_entities; ++i) {
		// TODO -- per pixel picking
		struct entity *e = &game_state->entities[i];
		v2_u8 dim = entity_dimensions[e->type];
		if ((e->pos.x < mouse_pos.x && mouse_pos.x < e->pos.x + dim.w) &&
		    (e->pos.y < mouse_pos.y && mouse_pos.y < e->pos.y + dim.h)) {
			return e->id;
		}
	}
	return NO_ENTITY;
}

GAME_ENTITY_BY_ID {
	for (u32 i = 0; i < game_state->num_entities; ++i) {
		struct entity *e = &game_state->entities[i];
		if (e->id == id) {
			return e;
		}
	}
	return NULL;
}
