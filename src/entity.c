ENTITY_ADD_ENTITY_ANIM {
	struct entity_anim entity_anim = {
		.type = ENTITY_ANIM_NONE,
		.sprite = entity_sprite_index[entity->type],
		.pos = (v2) { .x = entity->pos.x, .y = entity->pos.y },
	};
	anim_state->entity_anims[anim_state->num_entity_anims++] = entity_anim;
}
