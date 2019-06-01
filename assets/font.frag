#version 330 core

uniform sampler2D font_tex;

in vec2 tex_coord;
in vec3 glyph_color;

layout (location = 0) out vec4 color;

void main() {
	vec4 tex_sample = texture(font_tex, tex_coord);
	color = vec4(glyph_color * tex_sample.rgb, tex_sample.a);
}
