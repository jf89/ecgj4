DRAW_INIT {
	draw_data->tile_program = glCreateProgram();
	if (draw_data->tile_program == 0) {
		SDL_Log("Failed to create tile OpenGL program.");
		return TRUE;
	}
	glAttachShader(draw_data->tile_program, assets->shaders[ASSET_SHADER_TILE_VERT].shader);
	glAttachShader(draw_data->tile_program, assets->shaders[ASSET_SHADER_TILE_FRAG].shader);
	glLinkProgram(draw_data->tile_program);

	GLint link_status;
	glGetProgramiv(draw_data->tile_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		SDL_Log("Failed to link tile program.");
		return TRUE;
	}

	glGenVertexArrays(1, &draw_data->tile_vao);
	glBindVertexArray(draw_data->tile_vao);

	glGenBuffers(1, &draw_data->tile_static_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->tile_static_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tile_static_vertices),
		tile_static_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct tile_static_vertex),
		(GLvoid*)offsetof(struct tile_static_vertex, vertex_pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct tile_static_vertex),
		(GLvoid*)offsetof(struct tile_static_vertex, tex_coord));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &draw_data->tile_instance_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->tile_instance_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tile_instances),
		NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct tile_instance),
		(GLvoid*)offsetof(struct tile_instance, pos));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(struct tile_instance),
		(GLvoid*)offsetof(struct tile_instance, sprite_bottom_left));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(struct tile_instance),
		(GLvoid*)offsetof(struct tile_instance, sprite_extent));
	glVertexAttribDivisor(4, 1);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);

	glUseProgram(draw_data->tile_program);
	GLint tileset_location = glGetUniformLocation(draw_data->tile_program, "tileset");
	glUniform1i(tileset_location, ASSET_TEXTURE_TILESET);
	GLint tex_size_location = glGetUniformLocation(draw_data->tile_program, "tex_size");
	glUniform2f(tex_size_location,
		assets->textures[ASSET_TEXTURE_TILESET].width,
		assets->textures[ASSET_TEXTURE_TILESET].height);

	draw_data->font_program = glCreateProgram();
	if (draw_data->font_program == 0) {
		SDL_Log("Failed to create font OpenGL program.");
		return TRUE;
	}
	glAttachShader(draw_data->font_program, assets->shaders[ASSET_SHADER_FONT_VERT].shader);
	glAttachShader(draw_data->font_program, assets->shaders[ASSET_SHADER_FONT_FRAG].shader);
	glLinkProgram(draw_data->font_program);

	glGetProgramiv(draw_data->font_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		SDL_Log("Failed to link font program.");
		return TRUE;
	}

	glGenVertexArrays(1, &draw_data->font_vao);
	glBindVertexArray(draw_data->font_vao);

	glGenBuffers(1, &draw_data->font_static_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->font_static_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(font_static_vertices),
		font_static_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct font_static_vertex),
		(GLvoid*)offsetof(struct font_static_vertex, vertex_pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct font_static_vertex),
		(GLvoid*)offsetof(struct font_static_vertex, tex_coord));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &draw_data->font_instance_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->font_instance_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(font_instances), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct font_instance),
		(GLvoid*)offsetof(struct font_instance, pos));
	glVertexAttribDivisor(2, 1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(struct font_instance),
		(GLvoid*)offsetof(struct font_instance, glyph));
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(struct font_instance),
		(GLvoid*)offsetof(struct font_instance, color));
	glVertexAttribDivisor(4, 1);
	glEnableVertexAttribArray(4);

	glUseProgram(draw_data->font_program);
	GLint font_tex_location = glGetUniformLocation(draw_data->font_program, "font_tex");
	glUniform1i(font_tex_location, ASSET_TEXTURE_CP437_FONT);

	glBindVertexArray(0);

	draw_data->line_program = glCreateProgram();
	if (draw_data->line_program == 0) {
		SDL_Log("Failed to create OpenGL line program.");
		return TRUE;
	}
	glAttachShader(draw_data->line_program, assets->shaders[ASSET_SHADER_LINE_VERT].shader);
	glAttachShader(draw_data->line_program, assets->shaders[ASSET_SHADER_LINE_FRAG].shader);
	glLinkProgram(draw_data->line_program);

	glGetProgramiv(draw_data->line_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		SDL_Log("Failed to link line program.");
		return FALSE;
	}

	glGenVertexArrays(1, &draw_data->line_vao);
	glBindVertexArray(draw_data->line_vao);

	glGenBuffers(1, &draw_data->line_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->line_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct line_vertex),
		(GLvoid*)offsetof(struct line_vertex, pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct line_vertex),
		(GLvoid*)offsetof(struct line_vertex, color));
	glEnableVertexAttribArray(1);

	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return FALSE;
}

DRAW_ADD_TILE {
	tile_instances[draw_data->num_tiles_to_draw++] = *tile_data;
}

DRAW_ADD_GLYPH {
	font_instances[draw_data->num_chars_to_draw++] = *glyph;
}

DRAW_ADD_STRING {
	struct font_instance glyph = {
		.pos = pos,
		.glyph = (v2) { .x = 0.0f, .y = 0.0f },
		.color = color,
	};
	for (u8 *p = (u8*)string; *p; ++p) {
		glyph.glyph.x = (*p) % 32;
		glyph.glyph.y = 7 - ((*p) / 32);
		draw_add_glyph(draw_data, &glyph);
		glyph.pos.x += 1.0f;
	}
}

DRAW_FREE {
	glDeleteVertexArrays(1, &draw_data->tile_vao);
	glDeleteBuffers(1, &draw_data->tile_static_buffer);
	glDeleteBuffers(1, &draw_data->tile_instance_buffer);
	glDeleteProgram(draw_data->tile_program);
}

DRAW_TILES {
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->tile_instance_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		draw_data->num_tiles_to_draw*sizeof(struct tile_instance),
		tile_instances);
	glUseProgram(draw_data->tile_program);
	glBindVertexArray(draw_data->tile_vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, ARRAY_LENGTH(tile_static_vertices),
		draw_data->num_tiles_to_draw);
	glBindVertexArray(0);
}

DRAW_UPDATE_SCREEN_RES {
	glUseProgram(draw_data->tile_program);
	GLint screen_resolution_location = glGetUniformLocation(draw_data->tile_program,
		"screen_resolution");
	glUniform2f(screen_resolution_location, width, height);

	glUseProgram(draw_data->font_program);
	screen_resolution_location = glGetUniformLocation(draw_data->font_program, "screen_resolution");
	glUniform2f(screen_resolution_location, width, height);

	glUseProgram(draw_data->line_program);
	screen_resolution_location = glGetUniformLocation(draw_data->line_program, "screen_resolution");
	glUniform2f(screen_resolution_location, width, height);
}

DRAW_SET_ZOOM {
	glUseProgram(draw_data->tile_program);
	GLint zoom_location = glGetUniformLocation(draw_data->tile_program, "zoom");
	glUniform1f(zoom_location, zoom);

	glUseProgram(draw_data->line_program);
	zoom_location = glGetUniformLocation(draw_data->line_program, "zoom");
	glUniform1f(zoom_location, zoom);
}

DRAW_RESET {
	draw_data->num_tiles_to_draw = 0;
	draw_data->num_chars_to_draw = 0;
	draw_data->num_lines_to_draw = 0;
}

DRAW_TEXT {
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->font_instance_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		draw_data->num_chars_to_draw * sizeof(struct font_instance),
		font_instances);
	glUseProgram(draw_data->font_program);
	glBindVertexArray(draw_data->font_vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, ARRAY_LENGTH(font_static_vertices),
		draw_data->num_chars_to_draw);
	glBindVertexArray(0);
}

DRAW_UPDATE_MAP_CENTER {
	glUseProgram(draw_data->tile_program);
	GLint center_location = glGetUniformLocation(draw_data->tile_program, "center");
	glUniform2f(center_location, center.x, center.y);

	glUseProgram(draw_data->line_program);
	center_location = glGetUniformLocation(draw_data->line_program, "center");
	glUniform2f(center_location, center.x, center.y);
}

DRAW_LINES {
	glBindBuffer(GL_ARRAY_BUFFER, draw_data->line_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0,
		draw_data->num_lines_to_draw * sizeof(struct line_vertex), line_vertices);
	glUseProgram(draw_data->line_program);
	glBindVertexArray(draw_data->line_vao);
	glDrawArrays(GL_LINES, 0, draw_data->num_lines_to_draw);
}

DRAW_ADD_LINE {
	line_vertices[draw_data->num_lines_to_draw++] = (struct line_vertex) {
		.pos = start,
		.color = color,
	};
	line_vertices[draw_data->num_lines_to_draw++] = (struct line_vertex) {
		.pos = end,
		.color = color,
	};
}
