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
#include "map.h"
#include "entity.h"
#include "game_state.h"

// GLOBAL VARIABLES
#define GL_FUNC(return_type, name, ...) return_type (*name)(__VA_ARGS__) = NULL;
GL_3_3_FUNCTIONS
#undef GL_FUNC

char *base_path = NULL;

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

	f32 zoom = 2.0f;
	v2 map_center = { .x = 0.0f, .y = 0.0f };

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
		enum entity_type entity_type = ENTITY_TYPE_FORT;
		dim = entity_dimensions[entity_type];
		dim.w += 2; dim.h += 2;
		do {
			fort_pos.x = rand() % (game_state.map.width  - dim.w);
			fort_pos.y = rand() % (game_state.map.height - dim.h);
		} while (is_blocked(fort_pos, dim));
		++fort_pos.x; ++fort_pos.y;
		struct entity entity = {
			.type = ENTITY_TYPE_FORT,
			.pos = fort_pos,
		};
		map_center = (v2) { .x = fort_pos.x, .y = fort_pos.y };
		game_add_entity(&game_state, &entity);
		for (u32 i = 0; i < 3; ++i) {
			v2_u8 pos;
			dim.w = 1; dim.h = 1;
			do {
				pos.x = fort_pos.x + (rand() % 11) - 5;
				pos.y = fort_pos.y + (rand() % 11) - 5;
			} while(is_blocked(pos, dim));
			entity.type = ENTITY_TYPE_MAN;
			entity.pos = pos;
			game_add_entity(&game_state, &entity);
		}
	}

	for (u32 i = 0; i < game_state.num_entities; ++i) {
		entity_add_entity_anim(&game_state.entities[i], &anim_state);
	}

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

#define MAX_SELECTED_ENTITIES 16
	struct  {
		u32 num_selected_entities;
		u32 selected_entities[MAX_SELECTED_ENTITIES];
	} selected_entities;
	selected_entities.num_selected_entities = 0;

	while (1) {
		v2_i32 screen_size = { .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT };
		v2_i32 mouse_screen_pos;
		SDL_GetMouseState(&mouse_screen_pos.x, &mouse_screen_pos.y);
		v2 mouse_world_pos = {
			.x = (mouse_screen_pos.x - screen_size.w/2)     / (zoom * 16.0f) + map_center.x,
			.y = (screen_size.h/2 - mouse_screen_pos.y - 1) / (zoom * 16.0f) + map_center.y,
		};
		u32 mouse_over_entity = game_get_entity_by_mouse(&game_state, mouse_world_pos);

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				return EXIT_SUCCESS;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_q:
					return EXIT_SUCCESS;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (e.button.button) {
				case SDL_BUTTON_MIDDLE:
					SDL_SetRelativeMouseMode(SDL_TRUE);
					break;
				case SDL_BUTTON_LEFT:
					if (mouse_over_entity != NO_ENTITY) {
						selected_entities.num_selected_entities = 1;
						selected_entities.selected_entities[0]
							= mouse_over_entity;
					} else {
						selected_entities.num_selected_entities = 0;
					}
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (e.button.button) {
				case SDL_BUTTON_MIDDLE:
					SDL_SetRelativeMouseMode(SDL_FALSE);
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (e.motion.state & SDL_BUTTON_MMASK) {
					map_center.x -= ((f32)e.motion.xrel) / (zoom * 16.0f);
					map_center.y += ((f32)e.motion.yrel) / (zoom * 16.0f);
				}
				break;
			case SDL_MOUSEWHEEL:
				#define WHEEL_SCALE_FACTOR 10.0f
				if (e.wheel.y > 0) {
					zoom = MIN(zoom + 1.0f, 4.0f);
				} else {
					zoom = MAX(zoom - 1.0f, 1.0f);
				}
				break;
			}
		}
		map_center.x = CLAMP(map_center.x,
			screen_size.w / (2.0f * zoom * 16.0f),
			game_state.map.width - screen_size.w / (2.0f * zoom * 16.0f));
		map_center.y = CLAMP(map_center.y,
			screen_size.h / (2.0f * zoom * 16.0f),
			game_state.map.width - screen_size.h / (2.0f * zoom * 16.0f));
		draw_reset(&draw_data);
		draw_set_zoom(&draw_data, zoom);
		draw_update_map_center(&draw_data, map_center);

		f32 time = ((f32)SDL_GetTicks()) / 1000.0f;
		draw_anim_state(&anim_state, &draw_data, time);

		if (mouse_over_entity != NO_ENTITY) {
			struct entity *e = game_entity_by_id(&game_state, mouse_over_entity);
			v2_u8 dim = entity_dimensions[e->type];
			v2_u8 pos = e->pos;
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

		if (selected_entities.num_selected_entities) {
			for (u32 i = 0; i < selected_entities.num_selected_entities; ++i) {
				u32 id = selected_entities.selected_entities[i];
				struct entity *e = game_entity_by_id(&game_state, id);
				v2_u8 dim = entity_dimensions[e->type];
				v2_u8 pos = e->pos;
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
#include "map.c"
#include "entity.c"
#include "game_state.c"
