ASSETS_LOAD {
	char *asset_filename = malloc(strlen(base_path) + sizeof(ASSETS_FILE));
	if (asset_filename == NULL) {
		SDL_Log("Failed to allocate space for asset filename.");
		return TRUE;
	}
	DEFER(free(asset_filename));
	strcpy(asset_filename, base_path);
	strcat(asset_filename, ASSETS_FILE);

	FILE *fp = fopen(asset_filename, "rb");
	if (fp == NULL) {
		SDL_Log("Failed to open assets file '" ASSETS_FILE "'");
		return TRUE;
	}
	DEFER(fclose(fp));

	fseek(fp, 0, SEEK_END);
	size_t filesize = ftell(fp);
	void *asset_file = malloc(filesize);
	if (asset_file == NULL) {
		SDL_Log("Failed to allocate memory to read in asset file.");
		return TRUE;
	}
	DEFER(free(asset_file));

	fseek(fp, 0, SEEK_SET);
	fread(asset_file, filesize, 1, fp);
	memcpy(assets, asset_file, sizeof(*assets));

	struct asset_header *header = asset_file;
	for (u32 i = 0; i < NUM_SHADERS; ++i) {
		GLuint shader = glCreateShader(header->shaders[i].type);
		if (shader == 0) {
			SDL_Log("Failed to allocate shader.");
			return TRUE;
		}
		assets->shaders[i].shader = shader;
		assets->shaders[i].source = NULL;
		char *src = asset_file + header->shaders[i].file_pos;
		glShaderSource(shader, 1, (const GLchar**)&src, &header->shaders[i].length);
		glCompileShader(shader);
		GLint compile_status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
		if (compile_status == GL_FALSE) {
			SDL_Log("Failed to compile shader.");
			return TRUE;
		}
	}

	for (u32 i = 0; i < NUM_TEXTURES; ++i) {
		// GLuint texture;
		struct asset_texture *p = &header->textures[i];
		v4_u8 *data = asset_file + p->file_pos;
		glActiveTexture(GL_TEXTURE0 + i);
		glGenTextures(1, &p->texture);
		glBindTexture(GL_TEXTURE_2D, p->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, p->width, p->height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return FALSE;
}

ASSETS_FREE {
	for (u32 i = 0; i < NUM_SHADERS; ++i) {
		glDeleteShader(assets->shaders[i].shader);
		assets->shaders[i].shader = 0;
	}
	for (u32 i = 0; i < NUM_TEXTURES; ++i) {
		glDeleteTextures(1, &assets->textures[i].texture);
		assets->textures[i].texture = 0;
	}
}
