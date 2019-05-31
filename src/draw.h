#pragma once

struct draw_data {
	GLuint tile_program;
	GLuint tile_vao;
	GLuint tile_static_buffer;
	GLuint tile_instance_buffer;
};

struct tile_static_vertex {
	v2 pos;
	v2 tex_coord;
};

struct tile_static_vertex tile_static_vertices[] = {
	{ .pos = { .x = -1.0f, .y = -1.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .pos = { .x =  1.0f, .y = -1.0f }, .tex_coord = { .x = 1.0f, .y = 0.0f } },
	{ .pos = { .x =  1.0f, .y =  1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },

	{ .pos = { .x = -1.0f, .y = -1.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .pos = { .x =  1.0f, .y =  1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },
	{ .pos = { .x = -1.0f, .y =  1.0f }, .tex_coord = { .x = 0.0f, .y = 1.0f } },
};

#define MAX_TILE_INSTANCES (256*256*4)

struct tile_instance_vertex {
	v2 sprite_bottom_left;
	v2 sprite_extent;
};

struct tile_instance_vertex tile_instance_vertices[MAX_TILE_INSTANCES] = { 0 };


#define DRAW_INIT bool draw_init(struct draw_data *draw_data, struct asset_header *assets)
DRAW_INIT;

#define DRAW_FREE void draw_free(struct draw_data *draw_data)
DRAW_FREE;

#define DRAW_TILE void draw_tile(struct draw_data *draw_data)
DRAW_TILE;

#define DRAW_UPDATE_SCREEN_RES \
	void draw_update_screen_res(struct draw_data *draw_data, u32 width, u32 height)
DRAW_UPDATE_SCREEN_RES;

#define DRAW_SET_ZOOM void draw_set_zoom(struct draw_data *draw_data, f32 zoom)
DRAW_SET_ZOOM;