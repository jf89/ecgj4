#pragma once

#define ENTITIES \
	/* begin entity list */ \
	ENTITY(GOBLIN,         1, 1) \
	ENTITY(SKELETON,       1, 1) \
	ENTITY(CYCLOPS,        2, 1) \
	ENTITY(SNAKE,          2, 1) \
	ENTITY(SLIME,          1, 1) \
	ENTITY(BAT,            1, 1) \
	ENTITY(MAN,            1, 1) \
	ENTITY(WOMAN,          1, 1) \
	ENTITY(KNIGHT,         1, 1) \
	ENTITY(WITCH,          1, 1) \
	ENTITY(PRIEST,         1, 1) \
	ENTITY(NECROMANCER,    1, 1) \
	ENTITY(FORT,           2, 2) \
	ENTITY(TOWER,          1, 2) \
	ENTITY(SMALL_HOUSE,    1, 1) \
	ENTITY(BIG_HOUSE,      1, 1) \
	ENTITY(WELL,           1, 1) \
	ENTITY(SMALL_FOUNTAIN, 1, 1) \
	ENTITY(BIG_FOUNTAIN,   1, 1) \
	ENTITY(TEMPLE,         1, 1) \
	ENTITY(WINDMILL,       1, 2) \
	ENTITY(STATUE,         1, 2) \
	ENTITY(ARENA,          2, 2) \
	ENTITY(DARK_FORT,      2, 3) \
	/* end entity list */

enum entity_type {
#define ENTITY(name, _width, _height) ENTITY_TYPE_ ## name,
ENTITIES
#undef ENTITY

	NUM_ENTITY_TYPES,
};

#define NO_ENTITY UINT32_MAX

struct entity {
	enum entity_type type;
	u32 id;
	v2_u8 pos;
};

u16 entity_sprite_index[] = {
#define ENTITY(name, _width, _height) SPRITE_ ## name,
ENTITIES
#undef ENTITY
};

v2_u8 entity_dimensions[] = {
#define ENTITY(_name, width, height) { .w = width, .h = height },
ENTITIES
#undef ENTITY
};

char *entity_type_name[] = {
#define ENTITY(name, _width, _height) #name ,
ENTITIES
#undef ENTITY
};

#define ENTITY_ADD_ENTITY_ANIM \
	void entity_add_entity_anim(struct entity *entity, struct anim_state *anim_state)
ENTITY_ADD_ENTITY_ANIM;
