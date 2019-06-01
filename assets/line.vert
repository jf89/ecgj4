#version 330 core

const vec2 tile_size = vec2(16.0f);

uniform float zoom;
uniform vec2 center;
uniform vec2 screen_resolution;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 color;

out vec3 line_color;

void main() {
	gl_Position = vec4((2.0f * zoom * (pos - center) * tile_size) / screen_resolution, 0.0f, 1.0f);
	line_color = color;
}
