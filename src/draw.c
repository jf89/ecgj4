DRAW_INIT {
	draw_data->tile_program = glCreateProgram();
	if (draw_data->tile_program == 0) {
		SDL_Log("Failed to create OpenGL program.");
		return TRUE;
	}
	glAttachShader(draw_data->tile_program, assets->shaders[ASSET_SHADER_TILE_VERT].shader);
	glAttachShader(draw_data->tile_program, assets->shaders[ASSET_SHADER_TILE_FRAG].shader);
	glLinkProgram(draw_data->tile_program);

	GLint link_status;
	glGetProgramiv(draw_data->tile_program, GL_LINK_STATUS, &link_status);
	if (link_status == GL_FALSE) {
		SDL_Log("Failed to link program.");
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return FALSE;
}

DRAW_ADD_TILE {
	tile_instances[draw_data->num_tiles_to_draw++] = *tile_data;
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
}

DRAW_SET_ZOOM {
	glUseProgram(draw_data->tile_program);
	GLint zoom_location = glGetUniformLocation(draw_data->tile_program, "zoom");
	glUniform1f(zoom_location, zoom);
}

DRAW_RESET {
	draw_data->num_tiles_to_draw = 0;
}
