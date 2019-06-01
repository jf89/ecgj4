#define ROOT_2 1.414f
#define INFTY 1000000.0f

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
