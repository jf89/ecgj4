#version 330 core

const vec2 tile_size = vec2(16.0f, 16.0f);

uniform float zoom;
uniform vec2 tex_size;
uniform vec2 center;
uniform vec2 screen_resolution;

layout (location = 0) in vec2 vertex_pos;
layout (location = 1) in vec2 tex_coord_in;
layout (location = 2) in vec2 pos;
layout (location = 3) in vec2 sprite_bottom_left;
layout (location = 4) in vec2 sprite_extent;

out vec2 tex_coord;

void main() {
	gl_Position = vec4(
		zoom * (2*(pos - center) * tile_size + vertex_pos * sprite_extent) / screen_resolution,
		0.0f, 1.0f);
	tex_coord = (sprite_bottom_left + sprite_extent * tex_coord_in) / tex_size;
}
