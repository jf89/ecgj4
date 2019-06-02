#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <time.h>

#include "gl_3_3.h"
#include "assets.h"
#include "draw.h"
#include "anim.h"
#include "game_state.h"
#include "ui.h"

// GLOBAL VARIABLES
#define GL_FUNC(return_type, name, ...) return_type (*name)(__VA_ARGS__) = NULL;
GL_3_3_FUNCTIONS
#undef GL_FUNC

char *base_path = NULL;

// Too big for the stack
struct dijkstra_maps dijkstra_maps;

i32 main(i32 argc, char **argv) {
	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		SDL_Log("Failed to init SDL: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	DEFER(SDL_Quit());

	base_path = SDL_GetBasePath();
	if (base_path == NULL) {
		SDL_Log("Failed getting base path.");
		return EXIT_FAILURE;
	}
	DEFER(SDL_free(base_path));

	if (SDL_GL_LoadLibrary(NULL)) {
		SDL_Log("Failed to load OpenGL library: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	DEFER(SDL_GL_UnloadLibrary());

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)) {
		SDL_Log("Failed to set OpenGL context major version: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
		SDL_Log("Failed to set OpenGL context minor version: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
		SDL_Log("Failed to set OpenGL profile mask to core: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window *window = SDL_CreateWindow(APPLICATION_NAME " -- jf89 -- Extra Credits Game Jam 4",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL);
	if (window == NULL) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	DEFER(SDL_DestroyWindow(window));

	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (gl_context == NULL) {
		SDL_Log("Unable to create OpenGL context: %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	DEFER(SDL_GL_DeleteContext(gl_context));

#define GL_FUNC(_return_type, name, ...) \
	name = SDL_GL_GetProcAddress(#name); \
	if (name == NULL) { \
		SDL_Log("Unable to load OpenGL procedure address for procedure '" #name "'"); \
		return EXIT_FAILURE; \
	}
GL_3_3_FUNCTIONS
#undef GL_FUNC

	struct asset_header assets;
	if (assets_load(&assets)) {
		SDL_Log("Failed to load assets file.");
		return EXIT_FAILURE;
	}
	DEFER(assets_free(&assets));

	struct draw_data draw_data;
	if (draw_init(&draw_data, &assets)) {
		SDL_Log("Failed to initialise draw.");
		return EXIT_FAILURE;
	}
	DEFER(draw_free(&draw_data));
	draw_update_screen_res(&draw_data, SCREEN_WIDTH, SCREEN_HEIGHT);

	struct anim_state anim_state;
	struct game_state game_state;

	map_generate(&game_state.map, 200, 200);
	map_create_anim_state(&game_state.map, &anim_state);

	struct ui_state ui_state = { 0 };
	ui_state.zoom = 2.0f;

	// add troops to map
	{
		bool is_blocked(v2_u8 pos, v2_u8 dim) {
			for (u32 y = pos.y; y < pos.y + dim.h; ++y) {
				for (u32 x = pos.x; x < pos.x + dim.w; ++x) {
					u32 idx = y*game_state.map.width + x;
					enum ground_type g = game_state.map.ground[idx];
					if (g == GROUND_TYPE_WATER) {
						return TRUE;
					}
				}
			}
			for (u32 i = 0; i < game_state.num_entities; ++i) {
				enum entity_type e_type = game_state.entities[i].type;
				v2_u8 e_dim = entity_dimensions[e_type];
				v2_u8 e_pos = game_state.entities[i].pos;
				v2_u8 overlap_bl, overlap_tr;
				overlap_bl.x = MAX(pos.x, e_pos.x);
				overlap_bl.y = MAX(pos.y, e_pos.y);
				overlap_tr.x = MIN(pos.x + dim.x, e_pos.x + e_dim.x);
				overlap_tr.y = MIN(pos.y + dim.y, e_pos.y + e_dim.y);
				if (overlap_bl.x < overlap_tr.x && overlap_bl.y < overlap_tr.y) {
					return TRUE;
				}
			}
			return FALSE;
		}
		v2_u8 fort_pos, dim;
		dim = entity_dimensions[ENTITY_TYPE_FORT];
		dim.w += 2; dim.h += 2;
		do {
			fort_pos.x = rand() % (game_state.map.width  - dim.w);
			fort_pos.y = rand() % (game_state.map.height - dim.h);
		} while (is_blocked(fort_pos, dim));
		++fort_pos.x; ++fort_pos.y;
		ui_state.map_center = (v2) { .x = fort_pos.x, .y = fort_pos.y };
		game_add_entity(&game_state, ENTITY_TYPE_FORT, fort_pos);
		for (u32 i = 0; i < 3; ++i) {
			v2_u8 pos;
			dim = entity_dimensions[ENTITY_TYPE_MAN];
			do {
				pos.x = fort_pos.x + (rand() % 11) - 5;
				pos.y = fort_pos.y + (rand() % 11) - 5;
			} while(is_blocked(pos, dim));
			game_add_entity(&game_state, ENTITY_TYPE_MAN, pos);
		}
	}

	for (u32 i = 0; i < game_state.num_entities; ++i) {
		entity_add_entity_anim(&game_state.entities[i], &anim_state);
	}

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

	while (1) {
		if (process_input(&ui_state, &game_state, &anim_state, &dijkstra_maps)) {
			return EXIT_SUCCESS;
		}

		draw_reset(&draw_data);
		draw_set_zoom(&draw_data, ui_state.zoom);
		draw_update_map_center(&draw_data, ui_state.map_center);

		f32 time = ((f32)SDL_GetTicks()) / 1000.0f;
		game_update(&game_state, &dijkstra_maps, &anim_state, time);
		draw_anim_state(&anim_state, &draw_data, time);

		if (ui_state.mouse_over_entity != NO_ENTITY) {
			struct entity_anim *e = get_entity_anim_by_id(
				&anim_state, ui_state.mouse_over_entity);
			v2 pos = e->pos;
			v2 dim = e->dim;
			v2 bl = { .x =         pos.x, .y =         pos.y };
			v2 br = { .x = pos.x + dim.w, .y =         pos.y };
			v2 tl = { .x =         pos.x, .y = pos.y + dim.h };
			v2 tr = { .x = pos.x + dim.w, .y = pos.y + dim.h };
			v3 color = { .r = 0.0f, .g = 1.0f, .b = 1.0f };
			draw_add_line(&draw_data, bl, br, color);
			draw_add_line(&draw_data, br, tr, color);
			draw_add_line(&draw_data, tr, tl, color);
			draw_add_line(&draw_data, tl, bl, color);
		}

		if (ui_state.num_selected_entities) {
			for (u32 i = 0; i < ui_state.num_selected_entities; ++i) {
				u32 id = ui_state.selected_entities[i];
				struct entity_anim *e = get_entity_anim_by_id(&anim_state, id);
				v2 pos = e->pos;
				v2 dim = e->dim;
				v2 bl = { .x =         pos.x, .y =         pos.y };
				v2 br = { .x = pos.x + dim.w, .y =         pos.y };
				v2 tl = { .x =         pos.x, .y = pos.y + dim.h };
				v2 tr = { .x = pos.x + dim.w, .y = pos.y + dim.h };
				v3 color = { .r = 0.0f, .g = 1.0f, .b = 0.0f };
				draw_add_line(&draw_data, bl, br, color);
				draw_add_line(&draw_data, br, tr, color);
				draw_add_line(&draw_data, tr, tl, color);
				draw_add_line(&draw_data, tl, bl, color);
			}
		}

		switch (ui_state.state) {
		case UI_STATE_DRAGGING_SELECTION: {
				v2 s = ui_state.selecting.start_pos, e = ui_state.selecting.end_pos;
				v2 bl = { .x = MIN(s.x, e.x), .y = MIN(s.y, e.y) };
				v2 tr = { .x = MAX(s.x, e.x), .y = MAX(s.y, e.y) };
				v2 br = { .x = tr.x, .y = bl.y };
				v2 tl = { .x = bl.x, .y = tr.y };
				v3 color = { .r = 0.0f, .g = 1.0f, .b = 0.0f };
				draw_add_line(&draw_data, bl, br, color);
				draw_add_line(&draw_data, br, tr, color);
				draw_add_line(&draw_data, tr, tl, color);
				draw_add_line(&draw_data, tl, bl, color);
			} break;
		}

		draw_add_string(&draw_data, ui_state_names[ui_state.state], (v2) { .x=0.0f, .y=0.0f },
			(v3) { .r=1.0f, .g=0.0f, .b=0.0f });

		glClear(GL_COLOR_BUFFER_BIT);
		draw_tiles(&draw_data);
		draw_lines(&draw_data);
		draw_text(&draw_data);
		SDL_GL_SwapWindow(window);
	}

	return EXIT_SUCCESS;
}

#include "assets.c"
#include "draw.c"
#include "anim.c"
#include "game_state.c"
#include "ui.c"
