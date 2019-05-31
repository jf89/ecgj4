MAP_GENERATE {
	u32 map_size = width * height;
	map->width = width;
	map->height = height;
	enum ground_type *p = map->ground;
	for (u32 i = 0; i < map_size; ++i, ++p) {
		*p = GROUND_TYPE_WATER;
	}
	for (u32 j = 3; j < height - 3; ++j) {
		for (u32 i = 3; i < width - 3; ++i) {
			map->ground[j*width + i] = GROUND_TYPE_SAND;
		}
	}
	for (u32 j = 6; j < height - 6; ++j) {
		for (u32 i = 6; i < width - 6; ++i) {
			map->ground[j*width + i] = GROUND_TYPE_GRASS;
		}
	}
}

MAP_CREATE_ANIM_STATE {
	u16 width = map->width, height = map->height;
	for (u32 j = 0; j < height; ++j) {
		for (u32 i = 0; i < width; ++i) {
			struct ground_anim *p =
				&anim_state->ground_anims[anim_state->num_ground_anims++];
			p->pos.x = (f32)i;
			p->pos.y = (f32)j;
			switch (map->ground[j*width + i]) {
			case GROUND_TYPE_WATER:
				p->type = GROUND_ANIM_WATER;
				p->next_change_time = 0.0f;
				break;
			case GROUND_TYPE_SAND:
				p->type = GROUND_ANIM_NONE;
				p->sprite = SPRITE_SAND_FULL_1;
				break;
			case GROUND_TYPE_GRASS:
				p->type = GROUND_ANIM_NONE;
				p->sprite = SPRITE_GRASS_FULL_1;
				break;
			}
		}
	}
}
