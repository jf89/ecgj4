#pragma once

struct draw_data {
	GLuint tile_program;
	GLuint tile_vao;
	GLuint tile_static_buffer;
	GLuint tile_instance_buffer;
	u32 num_tiles_to_draw;
};

struct tile_static_vertex {
	v2 vertex_pos;
	v2 tex_coord;
};

struct tile_static_vertex tile_static_vertices[] = {
	{ .vertex_pos = { .x = -1.0f, .y = -1.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x =  1.0f, .y = -1.0f }, .tex_coord = { .x = 1.0f, .y = 0.0f } },
	{ .vertex_pos = { .x =  1.0f, .y =  1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },

	{ .vertex_pos = { .x = -1.0f, .y = -1.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x =  1.0f, .y =  1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },
	{ .vertex_pos = { .x = -1.0f, .y =  1.0f }, .tex_coord = { .x = 0.0f, .y = 1.0f } },
};

#define MAX_TILE_INSTANCES (256*256*4)

struct tile_instance {
	v2 pos;
	v2 sprite_bottom_left;
	v2 sprite_extent;
};

struct tile_instance tile_instances[MAX_TILE_INSTANCES] = { 0 };


#define DRAW_INIT bool draw_init(struct draw_data *draw_data, struct asset_header *assets)
DRAW_INIT;

#define DRAW_FREE void draw_free(struct draw_data *draw_data)
DRAW_FREE;

#define DRAW_RESET void draw_reset(struct draw_data *draw_data)
DRAW_RESET;

#define DRAW_ADD_TILE void draw_add_tile(struct draw_data *draw_data, struct tile_instance *tile_data)
DRAW_ADD_TILE;

#define DRAW_TILES void draw_tiles(struct draw_data *draw_data)
DRAW_TILES;

#define DRAW_UPDATE_SCREEN_RES \
	void draw_update_screen_res(struct draw_data *draw_data, u32 width, u32 height)
DRAW_UPDATE_SCREEN_RES;

#define DRAW_SET_ZOOM void draw_set_zoom(struct draw_data *draw_data, f32 zoom)
DRAW_SET_ZOOM;
