#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "gl_3_3.h"
#include "assets.h"

// #define DEBUG

#define GL_FUNC(return_type, name, ...) return_type (*name)(__VA_ARGS__) = NULL;
GL_3_3_FUNCTIONS
#undef GL_FUNC

#define FULL_ASSETS_FILE "bin/" ASSETS_FILE
#define ASSETS_DIR "assets/"
#define MAX_ASSET_FILENAME_LEN 1024

const char *shader_filenames[] = {
#define SHADER(_name, _type, filename) filename,
SHADERS
#undef SHADER
};

const GLenum shader_type[] = {
#define SHADER(_name, type, _filename) type,
SHADERS
#undef SHADER
};

const char *texture_filenames[] = {
#define TEXTURE(_name, filename) filename,
TEXTURES
#undef TEXTURE
};

char *base_path = NULL;

u32 align(u32 alignment, u32 position) {
	u32 offset = (1 << alignment) - 1;
	u32 mask   = ~offset;
	return position = (position + offset) & mask;
}

i32 main(i32 argc, char **argv) {
	base_path = SDL_GetBasePath();
	if (base_path == NULL) {
		SDL_Log("Failed to get base path.");
		return EXIT_FAILURE;
	}
	DEFER(SDL_free(base_path));

	char *assets_filename = malloc(strlen(base_path) + sizeof(FULL_ASSETS_FILE));
	if (assets_filename == NULL) {
		SDL_Log("Failed to allocate memory for filename.");
		return EXIT_FAILURE;
	}
	DEFER(free(assets_filename));
	strcpy(assets_filename, base_path);
	strcat(assets_filename, FULL_ASSETS_FILE);
#ifdef DEBUG
	SDL_Log("Assets filename: '%s'", assets_filename);
#endif

	char *asset_filename = malloc(strlen(base_path) + sizeof(ASSETS_DIR) + MAX_ASSET_FILENAME_LEN);
	if (asset_filename == NULL) {
		SDL_Log("Unable to allocate memory for asset filename.");
		return EXIT_FAILURE;
	}
	DEFER(free(asset_filename));
	strcpy(asset_filename, base_path);
	strcat(asset_filename, ASSETS_DIR);
	u32 asset_filename_last_char = strlen(asset_filename);

	struct asset_header tmp_header;
	u32 next_filepos = sizeof(struct asset_header);
	next_filepos = align(2, next_filepos);

	for (u32 i = 0; i < NUM_SHADERS; ++i) {
		asset_filename[asset_filename_last_char] = 0;
		strcat(asset_filename, shader_filenames[i]);
#ifdef DEBUG
		SDL_Log("Shader %u: '%s'", i, asset_filename);
#endif
		FILE *fp = fopen(asset_filename, "r");
		if (fp == NULL) {
			SDL_Log("Failed to open shader: '%s'", asset_filename);
			return EXIT_FAILURE;
		}
		DEFER(fclose(fp));
		fseek(fp, 0, SEEK_END);
		size_t filesize = ftell(fp);
		assert(filesize != -1);
		struct asset_shader *p = &tmp_header.shaders[i];
#ifdef DEBUG
		SDL_Log("%u", next_filepos);
#endif
		p->shader = 0;
		p->type = shader_type[i];
		p->file_pos = next_filepos;
		p->length  = filesize;
		p->source  = malloc(filesize + 1);
		fseek(fp, 0, SEEK_SET);
		size_t read_bytes = fread(p->source, 1, filesize, fp);
		p->source[filesize] = 0;
		assert(read_bytes == filesize);
		assert(p->source[filesize] == 0);
#ifdef DEBUG
		SDL_Log("Shader source:\n%s", p->source);
#endif
		next_filepos += filesize + 1;
		next_filepos = align(2, next_filepos);
	}

	// assert(NUM_TEXTURES == 0);
	for (u32 i = 0; i < NUM_TEXTURES; ++i) {
		asset_filename[asset_filename_last_char] = 0;
		strcat(asset_filename, texture_filenames[i]);
#ifdef DEBUG
		SDL_Log("Texture %u: '%s'", i, asset_filename);
#endif
		FILE *fp = fopen(asset_filename, "rb");
		if (fp == NULL) {
			SDL_Log("Failed to open texture: '%s'", asset_filename);
			return EXIT_FAILURE;
		}
		DEFER(fclose(fp));
		fseek(fp, 0, SEEK_END);
		size_t filesize = ftell(fp);
		assert(filesize != -1);
		fseek(fp, 0, SEEK_SET);

		struct asset_texture *p = &tmp_header.textures[i];
		size_t read_u16s = fread(&p->width, sizeof(p->width), 2, fp);
		assert(read_u16s == 2);
#ifdef DEBUG
		SDL_Log("Texture size: %hux%hu", p->width, p->height);
#endif

		size_t data_size = p->width * p->height * sizeof(*p->data);
		p->file_pos = next_filepos;
		p->data = malloc(data_size);
		assert(fread(p->data, data_size, 1, fp) == 1);
		assert(data_size == filesize - 4);

		next_filepos += data_size;
		next_filepos = align(2, next_filepos);
	}

	size_t output_filesize = next_filepos;
	void *assets_file = malloc(output_filesize);
	struct asset_header *header = assets_file;

	for (u32 i = 0; i < NUM_SHADERS; ++i) {
		char *data_loc = assets_file + tmp_header.shaders[i].file_pos;
		strcpy(data_loc, tmp_header.shaders[i].source);
		header->shaders[i] = tmp_header.shaders[i];
		header->shaders[i].source = NULL;
#ifdef DEBUG
		SDL_Log("Shader %u", i);
		SDL_Log("Source:\n%s", (char*)(assets_file + header->shaders[i].file_pos));
#endif
	}

	for (u32 i = 0; i < NUM_TEXTURES; ++i) {
		v4_u8 *data_loc = assets_file + tmp_header.textures[i].file_pos;
		size_t data_size = tmp_header.textures[i].width * tmp_header.textures[i].height * 4;
		memcpy(data_loc, tmp_header.textures[i].data, data_size);
		header->textures[i] = tmp_header.textures[i];
		header->textures[i].data = NULL;
#ifdef DEBUG
		SDL_Log("Texture: %u", i);
#endif
	}

	FILE *outfile = fopen(assets_filename, "wb");
	fwrite(assets_file, output_filesize, 1, outfile);
	fclose(outfile);

	return EXIT_SUCCESS;
}

#include "assets.c"
