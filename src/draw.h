#pragma once

struct draw_data {
	GLuint tile_program;
	GLuint tile_vao;
	GLuint tile_static_buffer;
	GLuint tile_instance_buffer;
	u32 num_tiles_to_draw;

	GLuint font_program;
	GLuint font_vao;
	GLuint font_static_buffer;
	GLuint font_instance_buffer;
	u32 num_chars_to_draw;

	GLuint line_program;
	GLuint line_vao;
	GLuint line_buffer;
	u32 num_lines_to_draw;
};

struct tile_static_vertex {
	v2 vertex_pos;
	v2 tex_coord;
};

struct tile_static_vertex tile_static_vertices[] = {
	{ .vertex_pos = { .x = 0.0f, .y = 0.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 2.0f, .y = 0.0f }, .tex_coord = { .x = 1.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 2.0f, .y = 2.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },

	{ .vertex_pos = { .x = 0.0f, .y = 0.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 2.0f, .y = 2.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },
	{ .vertex_pos = { .x = 0.0f, .y = 2.0f }, .tex_coord = { .x = 0.0f, .y = 1.0f } },
};

#define MAX_TILE_INSTANCES (256*256*4)

struct tile_instance {
	v2 pos;
	v2 sprite_bottom_left;
	v2 sprite_extent;
};

struct tile_instance tile_instances[MAX_TILE_INSTANCES] = { 0 };


struct font_static_vertex {
	v2 vertex_pos;
	v2 tex_coord;
};

struct font_static_vertex font_static_vertices[] = {
	{ .vertex_pos = { .x = 0.0f, .y = 0.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 1.0f, .y = 0.0f }, .tex_coord = { .x = 1.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 1.0f, .y = 1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },

	{ .vertex_pos = { .x = 0.0f, .y = 0.0f }, .tex_coord = { .x = 0.0f, .y = 0.0f } },
	{ .vertex_pos = { .x = 1.0f, .y = 1.0f }, .tex_coord = { .x = 1.0f, .y = 1.0f } },
	{ .vertex_pos = { .x = 0.0f, .y = 1.0f }, .tex_coord = { .x = 0.0f, .y = 1.0f } },
};

struct font_instance {
	v2 pos;
	v2 glyph;
	v3 color;
};

#define MAX_LINES 1024

struct line_vertex {
	v2 pos;
	v3 color;
};

struct line_vertex line_vertices[2*MAX_LINES] = { 0 };

#define MAX_FONT_INSTANCES (10*1024)

struct font_instance font_instances[MAX_FONT_INSTANCES] = { 0 };


#define DRAW_INIT bool draw_init(struct draw_data *draw_data, struct asset_header *assets)
DRAW_INIT;

#define DRAW_FREE void draw_free(struct draw_data *draw_data)
DRAW_FREE;

#define DRAW_RESET void draw_reset(struct draw_data *draw_data)
DRAW_RESET;

#define DRAW_ADD_TILE void draw_add_tile(struct draw_data *draw_data, struct tile_instance *tile_data)
DRAW_ADD_TILE;

#define DRAW_ADD_GLYPH void draw_add_glyph(struct draw_data *draw_data, struct font_instance *glyph)
DRAW_ADD_GLYPH;

#define DRAW_ADD_STRING void draw_add_string(struct draw_data *draw_data, char *string, v2 pos, v3 color)
DRAW_ADD_STRING;

#define DRAW_TILES void draw_tiles(struct draw_data *draw_data)
DRAW_TILES;

#define DRAW_UPDATE_SCREEN_RES \
	void draw_update_screen_res(struct draw_data *draw_data, u32 width, u32 height)
DRAW_UPDATE_SCREEN_RES;

#define DRAW_UPDATE_MAP_CENTER void draw_update_map_center(struct draw_data *draw_data, v2 center)
DRAW_UPDATE_MAP_CENTER;

#define DRAW_SET_ZOOM void draw_set_zoom(struct draw_data *draw_data, f32 zoom)
DRAW_SET_ZOOM;

#define DRAW_TEXT void draw_text(struct draw_data *draw_data)
DRAW_TEXT;

#define DRAW_LINES void draw_lines(struct draw_data *draw_data)
DRAW_LINES;

#define DRAW_ADD_LINE void draw_add_line(struct draw_data *draw_data, v2 start, v2 end, v3 color)
DRAW_ADD_LINE;
