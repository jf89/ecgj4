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
	draw_set_zoom(&draw_data, 2.0f);

	struct anim_state anim_state;
	struct map map;

	map_generate(&map, 200, 200);
	map_create_anim_state(&map, &anim_state);

	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	while (1) {
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
			}
		}
		draw_reset(&draw_data);
		f32 time = ((f32)SDL_GetTicks()) / 1000.0f;
		draw_anim_state(&anim_state, &draw_data, time);

		glClear(GL_COLOR_BUFFER_BIT);
		draw_tiles(&draw_data);
		SDL_GL_SwapWindow(window);
	}

	return EXIT_SUCCESS;
}

#include "assets.c"
#include "draw.c"
#include "anim.c"
#include "map.c"
