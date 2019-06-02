void get_entities_in_box(
	struct anim_state *anim_state,
	u32 *count,
	u32 *buffer,
	u32 length,
	v2 start,
	v2 end
) {
	v2 bl = { .x = MIN(start.x, end.x), .y = MIN(start.y, end.y) };
	v2 tr = { .x = MAX(start.x, end.x), .y = MAX(start.y, end.y) };
	*count = 0;

	struct entity_anim *p = anim_state->entity_anims;
	for (u32 i = 0; i < anim_state->num_entity_anims; ++i, ++p) {
		v2 ebl = { .x =            p->pos.x, .y =            p->pos.y };
		v2 etr = { .x = p->pos.x + p->dim.w, .y = p->pos.y + p->dim.h };
		v2 overlap_bl = { .x = MAX(bl.x, ebl.x), .y = MAX(bl.y, ebl.y) };
		v2 overlap_tr = { .x = MIN(tr.x, etr.x), .y = MIN(tr.y, etr.y) };
		if (overlap_bl.x < overlap_tr.x && overlap_bl.y < overlap_tr.y) {
			*buffer++ = p->id;
			++(*count);
			if (--length == 0) {
				break;
			}
		}
	}
}

PROCESS_INPUT {
	v2_i32 screen_size = { .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT };
	v2_i32 mouse_screen_pos;
	SDL_GetMouseState(&mouse_screen_pos.x, &mouse_screen_pos.y);
	v2 mouse_world_pos = {
		.x = (mouse_screen_pos.x - screen_size.w/2) / (ui_state->zoom * 16.0f)
			+ ui_state->map_center.x,
		.y = (screen_size.h/2 - mouse_screen_pos.y-1) / (ui_state->zoom * 16.0f)
			+ ui_state->map_center.y,
	};
	ui_state->mouse_over_entity = entity_by_mouse_pos(anim_state, mouse_world_pos);

	switch (ui_state->state) {
	case UI_STATE_DRAGGING_SELECTION:
		ui_state->selecting.end_pos = mouse_world_pos;
		break;
	}

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			return TRUE;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym) {
			case SDLK_q:
				return TRUE;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (e.button.button) {
			case SDL_BUTTON_MIDDLE:
				SDL_SetRelativeMouseMode(SDL_TRUE);
				ui_state->dragging_map.prev_state = ui_state->state;
				ui_state->state = UI_STATE_DRAGGING_MAP;
				break;
			case SDL_BUTTON_LEFT:
				switch (ui_state->state) {
				case UI_STATE_MAN_SELECTED:
				case UI_STATE_FORT_SELECTED:
				case UI_STATE_MULTIPLE_SELECTED:
				case UI_STATE_DEFAULT:
					ui_state->state = UI_STATE_DRAGGING_SELECTION;
					ui_state->selecting.start_pos = mouse_world_pos;
					ui_state->selecting.end_pos   = mouse_world_pos;
					break;
				}
				break;
			case SDL_BUTTON_RIGHT:
				if (ui_state->num_selected_entities) {
					u32 dm_idx = get_free_dijkstra_map(dijkstra_maps);
					dijkstra_maps->maps[dm_idx].objective = OBJECTIVE_PURSUE;
					dijkstra_maps->maps[dm_idx].goal = (v2_u8) {
						.x = (u8)mouse_world_pos.x,
						.y = (u8)mouse_world_pos.y,
					};
					path_calculate(&dijkstra_maps->maps[dm_idx], game_state);

					for (u32 i = 0;
					     i < ui_state->num_selected_entities;
					     ++i) {
						struct entity *e = game_entity_by_id(game_state,
							ui_state->selected_entities[i]);
						e->cur_command.type = COMMAND_MOVE;
						e->cur_command.move.dijkstra_map = dm_idx;
						++dijkstra_maps->reference_counts[dm_idx];
					}

				}
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (e.button.button) {
			case SDL_BUTTON_LEFT:
				switch (ui_state->state) {
				case UI_STATE_DRAGGING_SELECTION: {
					u32 count = 0;
					u32 entities[MAX_SELECTED_ENTITIES];
					ui_state->selecting.end_pos.x += 0.0001f;
					ui_state->selecting.end_pos.y += 0.0001f;
					get_entities_in_box(anim_state,
						&count, entities, MAX_SELECTED_ENTITIES,
						ui_state->selecting.start_pos,
						ui_state->selecting.end_pos);
					SDL_Keymod keymod = SDL_GetModState();
					if (!(keymod & KMOD_SHIFT)) {
						ui_state->num_selected_entities = 0;
					}
					for (u32 i = 0; i < count; ++i) {
						if (ui_state->num_selected_entities
						    == MAX_SELECTED_ENTITIES) {
							break;
						}
						u32 id = entities[i];
						struct entity *e = game_entity_by_id(
							game_state, id);
						if (!(e->flags & ENTITY_FLAG_SELECTABLE)) {
							continue;
						}
						bool should_add = TRUE;
						for (u32 j = 0;
						     j < ui_state->num_selected_entities;
						     ++j) {
							if (ui_state->selected_entities[j] == id) {
								should_add = FALSE;
								break;
							}
						}
						if (should_add) {
							ui_state->selected_entities[
								ui_state->num_selected_entities++] = id;
						}
					}
					if (ui_state->num_selected_entities == 0) {
						ui_state->state = UI_STATE_DEFAULT;
					} else if (ui_state->num_selected_entities == 1) {
						u32 id = ui_state->selected_entities[0];
						enum entity_type type = game_entity_by_id(
							game_state, id)->type;
						switch (type) {
						case ENTITY_TYPE_MAN:
							ui_state->state = UI_STATE_MAN_SELECTED;
							break;
						case ENTITY_TYPE_FORT:
							ui_state->state = UI_STATE_FORT_SELECTED;
							break;
						}
					} else {
						ui_state->state = UI_STATE_MULTIPLE_SELECTED;
					}
					} break;
				}
				break;
			case SDL_BUTTON_MIDDLE:
				if (ui_state->state == UI_STATE_DRAGGING_MAP) {
					ui_state->state = ui_state->dragging_map.prev_state;
					SDL_SetRelativeMouseMode(SDL_FALSE);
				}
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			switch (ui_state->state) {
			case UI_STATE_DRAGGING_MAP:
				ui_state->map_center.x -= ((f32)e.motion.xrel)/(ui_state->zoom * 16.0f);
				ui_state->map_center.y += ((f32)e.motion.yrel)/(ui_state->zoom * 16.0f);
				break;
			}
			break;
		case SDL_MOUSEWHEEL:
			#define WHEEL_SCALE_FACTOR 10.0f
			if (e.wheel.y > 0) {
				ui_state->zoom = MIN(ui_state->zoom + 1.0f, 4.0f);
			} else {
				ui_state->zoom = MAX(ui_state->zoom - 1.0f, 1.0f);
			}
			break;
		}
	}

	ui_state->map_center.x = CLAMP(ui_state->map_center.x,
		screen_size.w / (2.0f * ui_state->zoom * 16.0f),
		game_state->map.width - screen_size.w / (2.0f * ui_state->zoom * 16.0f));
	ui_state->map_center.y = CLAMP(ui_state->map_center.y,
		screen_size.h / (2.0f * ui_state->zoom * 16.0f),
		game_state->map.width - screen_size.h / (2.0f * ui_state->zoom * 16.0f));

	return FALSE;
};
