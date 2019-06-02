#pragma once

#define MAX_ENTITIES (256*256)

enum ground_type {
	GROUND_TYPE_WATER,
	GROUND_TYPE_SAND,
	GROUND_TYPE_GRASS,
};

#define MAX_MAP_WIDTH  256
#define MAX_MAP_HEIGHT MAX_MAP_WIDTH
#define MAX_MAP_SIZE   (MAX_MAP_WIDTH * MAX_MAP_HEIGHT)

struct map {
	u16 width, height;
	enum ground_type ground[MAX_MAP_SIZE];
};

#define ENTITIES \
	/* begin entity list */ \
	ENTITY(GOBLIN,         1, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(SKELETON,       1, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(CYCLOPS,        2, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(SNAKE,          2, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(SLIME,          1, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(BAT,            1, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(MAN,            1, 1, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_SELECTABLE) \
	ENTITY(WOMAN,          1, 1, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_SELECTABLE) \
	ENTITY(KNIGHT,         1, 1, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_SELECTABLE) \
	ENTITY(WITCH,          1, 1, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_SELECTABLE) \
	ENTITY(PRIEST,         1, 1, ENTITY_FLAG_MOVABLE | ENTITY_FLAG_SELECTABLE) \
	ENTITY(NECROMANCER,    1, 1, ENTITY_FLAG_MOVABLE) \
	ENTITY(FORT,           2, 2, ENTITY_FLAG_SELECTABLE) \
	ENTITY(TOWER,          1, 2, ENTITY_FLAG_SELECTABLE) \
	ENTITY(SMALL_HOUSE,    1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(BIG_HOUSE,      1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(WELL,           1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(SMALL_FOUNTAIN, 1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(BIG_FOUNTAIN,   1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(TEMPLE,         1, 1, ENTITY_FLAG_SELECTABLE) \
	ENTITY(WINDMILL,       1, 2, ENTITY_FLAG_SELECTABLE) \
	ENTITY(STATUE,         1, 2, ENTITY_FLAG_SELECTABLE) \
	ENTITY(ARENA,          2, 2, ENTITY_FLAG_SELECTABLE) \
	ENTITY(DARK_FORT,      2, 3, 0) \
	/* end entity list */

enum entity_type {
	#define ENTITY(name, _width, _height, _flags) ENTITY_TYPE_ ## name,
	ENTITIES
	#undef ENTITY

	NUM_ENTITY_TYPES,
};

#define NO_ENTITY UINT32_MAX

struct command {
	enum {
		COMMAND_NONE,
		COMMAND_MOVE,
	} type;
	union {
		struct {
			u32 dijkstra_map;
		} move;
	};
};

#define MOVE_TIME 0.25f

enum entity_flags {
	ENTITY_FLAG_SELECTABLE = (1 << 0),
	ENTITY_FLAG_MOVABLE    = (1 << 1),
};

struct entity_state {
	enum {
		ENTITY_STATE_IDLE,
		ENTITY_STATE_MOVING,
	} state;
	union {
		struct {
			f32 end_time;
		} moving;
	};
};

struct entity {
	enum entity_type type;
	enum entity_flags flags;
	u32 id;
	v2_u8 pos;
	struct entity_state state;
	struct command cur_command;
};

u16 entity_sprite_index[] = {
	#define ENTITY(name, _width, _height, _flags) SPRITE_ ## name,
	ENTITIES
	#undef ENTITY
};

v2_u8 entity_dimensions[] = {
	#define ENTITY(_name, width, height, _flags) { .w = width, .h = height },
	ENTITIES
	#undef ENTITY
};

char *entity_type_name[] = {
	#define ENTITY(name, _width, _height, _flags) #name,
	ENTITIES
	#undef ENTITY
};

enum entity_flags entity_flags[] = {
	#define ENTITY(_name, _width, _height, flags) flags,
	ENTITIES
	#undef ENTITY
};

struct game_state {
	struct map map;
	u32 num_entities;
	u32 next_entity_id;
	struct entity entities[MAX_ENTITIES];
};


struct dijkstra_map {
	enum {
		OBJECTIVE_PURSUE,
		OBJECTIVE_FLEE,
	} objective;
	v2_u8 goal;
	f32 weight[MAX_MAP_SIZE];
};

#define MAX_DIJKSTRA_MAPS 32

struct dijkstra_maps {
	u32 num_maps;
	u32 reference_counts[MAX_DIJKSTRA_MAPS];
	struct dijkstra_map maps[MAX_DIJKSTRA_MAPS];
};




#define PATH_CALCULATE \
	void path_calculate( \
		struct dijkstra_map *path_map, \
		struct game_state *game_state)
PATH_CALCULATE;

#define GET_FREE_DIJKSTRA_MAP u32 get_free_dijkstra_map(struct dijkstra_maps *maps)
GET_FREE_DIJKSTRA_MAP;

#define GAME_ADD_ENTITY \
	void game_add_entity(struct game_state *game_state, enum entity_type entity_type, v2_u8 pos)
GAME_ADD_ENTITY;

#define GAME_ENTITY_BY_ID struct entity *game_entity_by_id(struct game_state *game_state, u32 id)
GAME_ENTITY_BY_ID;

#define GAME_UPDATE \
	void game_update( \
		struct game_state *game_state, \
		struct dijkstra_maps *dijkstra_maps, \
		struct anim_state *anim_state, \
		f32 time)
GAME_UPDATE;

#define MAP_GENERATE void map_generate(struct map *map, u16 width, u16 height)
MAP_GENERATE;

#define MAP_CREATE_ANIM_STATE void map_create_anim_state(struct map *map, struct anim_state *anim_state)
MAP_CREATE_ANIM_STATE;

#define ENTITY_ADD_ENTITY_ANIM \
	void entity_add_entity_anim(struct entity *entity, struct anim_state *anim_state)
ENTITY_ADD_ENTITY_ANIM;
