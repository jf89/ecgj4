#version 330 core

uniform sampler2D tileset;

in vec2 tex_coord;

layout (location = 0) out vec4 color;

void main() {
	color = texture(tileset, tex_coord);
}
