GAME_ADD_ENTITY {
	u32 idx = game_state->num_entities++;
	game_state->entities[idx] = *entity;
}
