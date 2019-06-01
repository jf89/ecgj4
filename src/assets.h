#pragma once

struct asset_shader {
	GLuint shader;
	GLenum type;
	u32 file_pos;
	i32 length;
	char *source;
};

#define SHADERS \
	/* begin shader list */ \
	SHADER(TILE_VERT, GL_VERTEX_SHADER,   "tile.vert") \
	SHADER(TILE_FRAG, GL_FRAGMENT_SHADER, "tile.frag") \
	SHADER(FONT_VERT, GL_VERTEX_SHADER,   "font.vert") \
	SHADER(FONT_FRAG, GL_FRAGMENT_SHADER, "font.frag") \
	/* end shader list */

enum asset_shaders {
#define SHADER(name, _type, _file) ASSET_SHADER_ ## name,
SHADERS
#undef SHADER

	NUM_SHADERS,
};

struct asset_texture {
	GLuint texture;
	u32 file_pos;
	u16 width, height;
	v4_u8 *data;
};

#define TEXTURES \
	/* begin texture list */ \
	TEXTURE(TILESET,    "tileset.bin") \
	TEXTURE(CP437_FONT, "codepage437.bin") \
	/* end texture list */

enum asset_textures {
#define TEXTURE(name, _file) ASSET_TEXTURE_ ## name,
TEXTURES
#undef TEXTURE

	NUM_TEXTURES,
};

struct asset_header {
	struct asset_shader  shaders[NUM_SHADERS];
	struct asset_texture textures[NUM_TEXTURES];
};

#define ASSETS_FILE "assets.bin"

#define ASSETS_LOAD bool assets_load(struct asset_header *assets)
ASSETS_LOAD;

#define ASSETS_FREE void assets_free(struct asset_header *assets)
ASSETS_FREE;
