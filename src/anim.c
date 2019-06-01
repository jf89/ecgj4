DRAW_ANIM_STATE {
	struct tile_instance tile;
	for (u32 i = 0; i < anim_state->num_ground_anims; ++i) {
		struct ground_anim *p = &anim_state->ground_anims[i];
		switch (p->type) {
		case GROUND_ANIM_WATER:
			if (time > p->next_change_time) {
				p->next_change_time =
					time + 5.0f + 5.0f * ((f32)rand()) / ((f32)RAND_MAX);
				u32 next_sprite = rand() % 50;
				switch (next_sprite) {
				case 0:  p->sprite = SPRITE_WATER_1; break;
				case 1:  p->sprite = SPRITE_WATER_2; break;
				case 2:  p->sprite = SPRITE_WATER_3; break;
				case 3:  p->sprite = SPRITE_WATER_4; break;
				case 4:  p->sprite = SPRITE_WATER_5; break;
				default: p->sprite = SPRITE_WATER_6; break;
				}
			}
			break;
		case GROUND_ANIM_NONE:
			break;
		}
		tile.pos = p->pos;
		tile.sprite_bottom_left = sprite_rects[p->sprite].bottom_left;
		tile.sprite_extent      = sprite_rects[p->sprite].extent;
		draw_add_tile(draw_data, &tile);
	}

	for (u32 i = 0; i < anim_state->num_entity_anims; ++i) {
		struct entity_anim *p = &anim_state->entity_anims[i];
		v2 pos = p->pos;
		switch (p->type) {
		case ENTITY_ANIM_NONE:
			break;
		case ENTITY_ANIM_BOBBING:
			break;
		}
		tile.pos = pos;
		tile.sprite_bottom_left = sprite_rects[p->sprite].bottom_left;
		tile.sprite_extent      = sprite_rects[p->sprite].extent;
		draw_add_tile(draw_data, &tile);
	}
}
