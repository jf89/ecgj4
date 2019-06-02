ENTITY_ADD_ENTITY_ANIM {
	v2_u8 dim = entity_dimensions[entity->type];
	struct entity_anim entity_anim = {
		.type = ENTITY_ANIM_NONE,
		.id = entity->id,
		.sprite = entity_sprite_index[entity->type],
		.pos = (v2) { .x = entity->pos.x, .y = entity->pos.y },
		.dim = (v2) { .w = dim.w, .h = dim.h },
	};
	anim_state->entity_anims[anim_state->num_entity_anims++] = entity_anim;
}


MAP_GENERATE {
	u32 map_size = width * height;
	map->width = width;
	map->height = height;

	u16 buffer_1[MAX_MAP_SIZE], buffer_2[MAX_MAP_SIZE];
	memset(buffer_1, 0, sizeof(buffer_1));
	memset(buffer_2, 0, sizeof(buffer_2));

#define BORDER_SIZE 5

	// 0 is water
	// 1 is sand
	for (u32 j = BORDER_SIZE; j < height-BORDER_SIZE; ++j) {
		for (u32 i = BORDER_SIZE; i < width-BORDER_SIZE; ++i) {
			buffer_1[j*width + i] = (rand() % 11) < 6 ? 0 : 1;
		}
	}
	u16 *front = buffer_1, *back = buffer_2;
	for (u32 k = 0; k < 10; ++k) {
		SWAP(front, back);
		for (u32 j = BORDER_SIZE; j < height-BORDER_SIZE; ++j) {
			for (u32 i = BORDER_SIZE; i < width-BORDER_SIZE; ++i) {
				u32 num_zero_neighbours = 0;
				for (i32 dy = -1; dy <= 1; ++dy) {
					for (i32 dx = -1; dx <= 1; ++dx) {
						if (dx == 0 && dy == 0) {
							continue;
						}
						u32 idx = (j+dy)*width + (i+dx);
						if (back[idx] == 0) {
							++num_zero_neighbours;
						}
					}
				}
				u32 idx = j*width + i;
				if (back[idx] == 0) {
					front[idx] = num_zero_neighbours >= 4 ? 0 : 1;
				} else {
					front[idx] = num_zero_neighbours >= 6 ? 0 : 1;
				}
			}
		}
	}

	u32 changes_made;
	do {
		changes_made = 0;
		for (u32 j = BORDER_SIZE; j < height-BORDER_SIZE; ++j) {
			for (u32 i = BORDER_SIZE; i < width-BORDER_SIZE; ++i) {
				u32 idx = width*j + i;
				if (front[idx] == 0) {
					continue;
				}
				u16 north = front[width*(j+1) + i];
				u16 south = front[width*(j-1) + i];
				u16 east  = front[width*j + (i+1)];
				u16 west  = front[width*j + (i-1)];
				u16 ne = front[width*(j+1) + (i+1)];
				u16 nw = front[width*(j+1) + (i-1)];
				u16 se = front[width*(j-1) + (i+1)];
				u16 sw = front[width*(j-1) + (i-1)];
				if (north == 0 && south == 0) {
					++changes_made;
					front[idx] = 0;
				} else if (east == 0 && west == 0) {
					++changes_made;
					front[idx] = 0;
				} else if (ne == 0 && sw == 0) {
					++changes_made;
					front[idx] = 0;
				} else if (se == 0 && nw == 0) {
					++changes_made;
					front[idx] = 0;
				}
			}
		}
	} while (changes_made > 0);

	// change to one connected component
	memset(back, 0, sizeof(buffer_1));
	u16 canonical_color[65535] = { 0 };
	u32 section_size[65535]    = { 0 };
	u16 cur_color = 1;
	for (u32 j = BORDER_SIZE; j < height-BORDER_SIZE; ++j) {
		for (u32 i = BORDER_SIZE; i < width-BORDER_SIZE; ++i) {
			u32 idx = width*j + i;
			if (front[idx] == 0) {
				continue;
			}
			u16 above_color = back[width*(j-1) + i];
			while (above_color != canonical_color[above_color]) {
				above_color = canonical_color[above_color];
			}
			u16 left_color = back[width*j + (i-1)];
			while (left_color != canonical_color[left_color]) {
				left_color = canonical_color[left_color];
			}
			if (above_color == 0 && left_color == 0) {
				canonical_color[cur_color] = cur_color;
				section_size[cur_color] = 1;
				back[idx] = cur_color;
				++cur_color;
				assert(cur_color != 65535);
			} else if (above_color == 0) {
				++section_size[left_color];
				back[idx] = left_color;
			} else if (left_color == 0) {
				++section_size[above_color];
				back[idx] = above_color;
			} else if (above_color == left_color) {
				++section_size[left_color];
				back[idx] = left_color;
			} else {
				u16 canonical = MIN(left_color, above_color);
				u16 other     = MAX(left_color, above_color);
				canonical_color[other] = canonical;
				section_size[canonical] += section_size[other];
				back[idx] = canonical;
			}
		}
	}
	u32 largest_section_size = 0;
	u16 largest_color = 0;
	for (u16 i = 0; i < cur_color; ++i) {
		if (canonical_color[i] == i && section_size[i] > largest_section_size) {
			largest_section_size = section_size[i];
			largest_color = i;
		}
	}
	for (u32 j = BORDER_SIZE; j < height-BORDER_SIZE; ++j) {
		for (u32 i = BORDER_SIZE; i < width-BORDER_SIZE; ++i) {
			u32 idx = j*width + i;
			u16 color = back[idx];
			while (canonical_color[color] != color) {
				color = canonical_color[color];
			}
			back[idx] = color == largest_color ? 1 : 0;
		}
	}
	SWAP(front, back);

	for (u32 i = 0; i < map_size; ++i) {
		map->ground[i] = front[i] ? GROUND_TYPE_SAND : GROUND_TYPE_WATER;
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
			case GROUND_TYPE_SAND: {
					p->type = GROUND_ANIM_WATER;
					p->next_change_time = 0.0f;
					enum ground_type north = map->ground[width*(j+1) + i];
					enum ground_type south = map->ground[width*(j-1) + i];
					enum ground_type east  = map->ground[width*j + (i+1)];
					enum ground_type west  = map->ground[width*j + (i-1)];
					enum ground_type ne = map->ground[width*(j+1) + (i+1)];
					enum ground_type nw = map->ground[width*(j+1) + (i-1)];
					enum ground_type se = map->ground[width*(j-1) + (i+1)];
					enum ground_type sw = map->ground[width*(j-1) + (i-1)];
					if (north == GROUND_TYPE_WATER && east == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_BL_FULL;
					} else if (north == GROUND_TYPE_WATER && west == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_BR_FULL;
					} else if (south == GROUND_TYPE_WATER && east == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_TL_FULL;
					} else if (south == GROUND_TYPE_WATER && west == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_TR_FULL;
					} else if (north == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						switch (rand() % 2) {
						case 0: p->sprite = SPRITE_SAND_T_EMPTY_1; break;
						case 1: p->sprite = SPRITE_SAND_T_EMPTY_2; break;
						}
					} else if (south == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						switch (rand() % 2) {
						case 0: p->sprite = SPRITE_SAND_B_EMPTY_1; break;
						case 1: p->sprite = SPRITE_SAND_B_EMPTY_2; break;
						}
					} else if (east == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						switch (rand() % 2) {
						case 0: p->sprite = SPRITE_SAND_R_EMPTY_1; break;
						case 1: p->sprite = SPRITE_SAND_R_EMPTY_2; break;
						}
					} else if (west == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						switch (rand() % 2) {
						case 0: p->sprite = SPRITE_SAND_L_EMPTY_1; break;
						case 1: p->sprite = SPRITE_SAND_L_EMPTY_2; break;
						}
					} else if (ne == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_TR_EMPTY;
					} else if (nw == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_TL_EMPTY;
					} else if (se == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_BR_EMPTY;
					} else if (sw == GROUND_TYPE_WATER) {
						++p; ++anim_state->num_ground_anims;
						p->pos.x = (f32)i; p->pos.y = (f32)j;
						p->type = GROUND_ANIM_NONE;
						p->sprite = SPRITE_SAND_BL_EMPTY;
					} else {
						p->type = GROUND_ANIM_NONE;
						switch (rand() % 19) {
						case  0: case  1: case  2: case  3: case  4: p->sprite = SPRITE_SAND_FULL_1; break;
						case  5: case  6: case  7: case  8: case  9: p->sprite = SPRITE_SAND_FULL_2; break;
						case 10: case 11: case 12: case 13: case 14: p->sprite = SPRITE_SAND_FULL_3; break;
						case 15: p->sprite = SPRITE_SAND_FULL_RARE_1; break;
						case 16: p->sprite = SPRITE_SAND_FULL_RARE_2; break;
						case 17: p->sprite = SPRITE_SAND_FULL_RARE_3; break;
						case 18: p->sprite = SPRITE_SAND_FULL_RARE_4; break;
						}
					}
				} break;
			case GROUND_TYPE_GRASS:
				p->type = GROUND_ANIM_NONE;
				p->sprite = SPRITE_GRASS_FULL_1;
				break;
			}
		}
	}
}

GAME_ADD_ENTITY {
	u32 idx = game_state->num_entities++;
	entity->id = game_state->next_entity_id++;
	game_state->entities[idx] = *entity;
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

#define INFTY 1000000.0f

GAME_UPDATE {
	for (u32 i = 0; i < game_state->num_entities; ++i) {
		struct entity *e = &game_state->entities[i];
		switch (e->state.state) {
		case ENTITY_STATE_IDLE:
			switch (e->cur_command.type) {
			case COMMAND_NONE:
				break;
			case COMMAND_MOVE: {
				u32 dm_idx = e->cur_command.move.dijkstra_map;
				struct dijkstra_map *m = &dijkstra_maps->maps[dm_idx];
				if (e->pos.x == m->goal.x && e->pos.y == m->goal.y) {
					e->cur_command.type = COMMAND_NONE;
					--dijkstra_maps->reference_counts[dm_idx];
				} else {
					f32 best_weight = INFTY;
					u8 x = e->pos.x, y = e->pos.y;
					u32 w = game_state->map.width;
					v2_u8 best_pos = e->pos;
					for (i8 dx = -1; dx <= 1; ++dx) {
						for (i8 dy = -1; dy <= 1; ++dy) {
							u32 idx = w*(y + dy) + (x + dx);
							f32 w = m->weight[idx];
							if (w == -1.0f) {
								continue;
							}
							SDL_Log("%f", w);
							if (w < best_weight) {
								best_weight = w;
								best_pos.x = x+dx;
								best_pos.y = y+dy;
							}
						}
					}
					// SDL_Log("entity: %u, move: (%hhu, %hhu) -> (%hhu, %hhu)",
					// 	e->id, e->pos.x, e->pos.y, best_pos.x, best_pos.y);
					if (best_pos.x != e->pos.x || best_pos.y != e->pos.y) {
						// MOVE
						f32 end_time = time + MOVE_TIME;
						struct entity_anim *p
							= get_entity_anim_by_id(anim_state, e->id);
						assert(p != NULL);
						p->type = ENTITY_ANIM_MOVING;
						p->moving.start_pos = (v2) { .x=e->pos.x, .y=e->pos.y };
						p->moving.end_pos = (v2){ .x=best_pos.x,.y=best_pos.y };
						p->moving.start_time = time;
						e->pos.x = best_pos.x;
						e->pos.y = best_pos.y;
						e->state.state = ENTITY_STATE_MOVING;
						e->state.moving.end_time = end_time;
					}
				}
				} break;
			}
			break;
		case ENTITY_STATE_MOVING:
			if (time > e->state.moving.end_time) {
				e->state.state = ENTITY_STATE_IDLE;
			}
			break;
		}
	}
}


#define ROOT_2 1.414f

PATH_CALCULATE {
	struct map *map = &game_state->map;
	u16 width = map->width, height = map->height;
	for (u32 j = 0; j < height; ++j) {
		for (u32 i = 0; i < width; ++i) {
			u32 idx = j*height + i;
			enum ground_type g = map->ground[idx];
			switch (g) {
			case GROUND_TYPE_WATER: path_map->weight[idx] = -1.0f; break;
			case GROUND_TYPE_SAND:  path_map->weight[idx] = INFTY; break;
			case GROUND_TYPE_GRASS: path_map->weight[idx] = INFTY; break;
			}
		}
	}
	for (u32 i = 0; i < game_state->num_entities; ++i) {
		struct entity *e = &game_state->entities[i];
		v2_u8 pos = e->pos, dim = entity_dimensions[e->type];
		for (u32 dy = 0; dy < dim.h; ++dy) {
			for (u32 dx = 0; dx < dim.w; ++dx) {
				u32 idx = width*(pos.y + dy) + (pos.x + dx);
				path_map->weight[idx] = -1.0f;
			}
		}
	}
	struct {
		v2_u8 pos;
		f32 cost;
	} to_visit[MAX_MAP_SIZE] = { 0 };

	to_visit[0].pos = path_map->goal;
	to_visit[0].cost = 0.0f;
	path_map->weight[width*path_map->goal.y + path_map->goal.x] = 0.0f;

	u32 to_visit_idx = 0, to_visit_end = 1;
	while (to_visit_idx < to_visit_end) {
		v2_u8 cur_pos  = to_visit[to_visit_idx].pos;
		f32   cur_cost = to_visit[to_visit_idx].cost;
		++to_visit_idx;

		void visit_node(v2_u8 node, f32 cost) {
			u32 idx = width*node.y + node.x;
			f32 cur_weight = path_map->weight[idx];
			if (cur_weight == -1.0f) {
				return;
			}
			if (cur_weight == INFTY) {
				path_map->weight[idx] = cost;
				u32 to_visit_node_idx = ++to_visit_end;
				while (to_visit[to_visit_node_idx-1].cost > cost) {
					to_visit[to_visit_node_idx] = to_visit[to_visit_node_idx-1];
					--to_visit_node_idx;
				}
				to_visit[to_visit_node_idx].pos  = node;
				to_visit[to_visit_node_idx].cost = cost;
			} else if (cur_weight > cost) {
				path_map->weight[idx] = cost;
				u32 to_visit_node_idx = to_visit_end;
				while ((to_visit[to_visit_node_idx].pos.x != node.x) &&
				       (to_visit[to_visit_node_idx].pos.y != node.y)) {
					--to_visit_node_idx;
				}
				while (to_visit[to_visit_node_idx-1].cost > cost) {
					to_visit[to_visit_node_idx] = to_visit[to_visit_node_idx-1];
					--to_visit_node_idx;
				}
				to_visit[to_visit_node_idx].pos  = node;
				to_visit[to_visit_node_idx].cost = cost;
			}
		}

		visit_node((v2_u8) { .x = cur_pos.x - 1, .y =     cur_pos.y }, cur_cost + 1.0f);
		visit_node((v2_u8) { .x = cur_pos.x + 1, .y =     cur_pos.y }, cur_cost + 1.0f);
		visit_node((v2_u8) { .x =     cur_pos.x, .y = cur_pos.y + 1 }, cur_cost + 1.0f);
		visit_node((v2_u8) { .x =     cur_pos.x, .y = cur_pos.y - 1 }, cur_cost + 1.0f);
		visit_node((v2_u8) { .x = cur_pos.x - 1, .y = cur_pos.y - 1 }, cur_cost + ROOT_2);
		visit_node((v2_u8) { .x = cur_pos.x - 1, .y = cur_pos.y + 1 }, cur_cost + ROOT_2);
		visit_node((v2_u8) { .x = cur_pos.x + 1, .y = cur_pos.y - 1 }, cur_cost + ROOT_2);
		visit_node((v2_u8) { .x = cur_pos.x + 1, .y = cur_pos.y + 1 }, cur_cost + ROOT_2);
	}

	/* printf("================================================================================\n");
	for (u32 j = 0; j < 10; ++j) {
		for (u32 i = 0; i < 10; ++i) {
			printf("%f ", path_map->weight[width*(goal.y + j - 5) + (goal.x + i - 5)]);
		}
		printf("\n");
	} */
}

GET_FREE_DIJKSTRA_MAP {
	for (u32 i = 0; i < MAX_DIJKSTRA_MAPS; ++i) {
		if (maps->reference_counts[i] == 0) {
			return i;
		}
	}
	return UINT32_MAX;
}
