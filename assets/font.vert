#version 330 core

const vec2 glyph_size = vec2(  9.0f,  16.0f);
const vec2 tex_size   = vec2(288.0f, 128.0f);

uniform vec2 screen_resolution;

layout (location = 0) in vec2 vertex_pos;
layout (location = 1) in vec2 tex_coord_in;
layout (location = 2) in vec2 pos;
layout (location = 3) in vec2 glyph;
layout (location = 4) in vec3 glyph_color_in;

out vec2 tex_coord;
out vec3 glyph_color;

void main() {
	vec2 new_pos = vec2(pos.x, -pos.y);
	new_pos = 4.0f * (new_pos + vertex_pos) * glyph_size / screen_resolution;
	new_pos.x -= 1.0f;
	new_pos.y += 1.0f - 4.0f * glyph_size.y / screen_resolution.y;
	gl_Position = vec4(new_pos, 0.0f, 1.0f);
	tex_coord = (glyph + tex_coord_in) * glyph_size / tex_size;
	glyph_color = glyph_color_in;
}
